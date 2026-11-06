
<p align="center">
  An ultra-low latency, deterministic <strong>High-Frequency Trading (HFT) Matching Engine</strong> implemented in <strong>C++20</strong>, engineered for lock-free order processing, zero dynamic heap allocations on the hot path, and cacheline-aligned data structures.
</p>

<p align="center">
  <a href="#quick-start"><img src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-0078D6?logo=linux&logoColor=white" alt="Platform"></a>
  <a href="#tech-stack"><img src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B&logoColor=white" alt="C++20"></a>
  <a href="#tech-stack"><img src="https://img.shields.io/badge/Build-CMake%203.24%2B-064F8C?logo=cmake&logoColor=white" alt="CMake"></a>
  <a href="#architecture"><img src="https://img.shields.io/badge/Hot--Path-Zero--Allocation-success" alt="Zero Allocation"></a>
  <a href="#architecture"><img src="https://img.shields.io/badge/Concurrency-Lock--Free%20SPSC-purple" alt="Lock Free SPSC"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License MIT"></a>
</p>

---

## Overview

In electronic trading and high-frequency exchange environments, sub-microsecond determinism is paramount. Uncontrolled heap allocations, OS lock contention, and CPU cache misses introduce unacceptable latency spikes (tail latency).

**Holt** is a lightweight, bare-metal matching engine designed from first principles:
- **Zero dynamic memory allocation** on the critical path using fixed-capacity intrusive memory pools and monotonic PMR buffers.
- **Lock-free inter-thread communication** via an SPSC circular ring buffer with cacheline padding to eliminate false sharing.
- **Cache-aligned order book** structured around intrusive doubly-linked price levels for guaranteed $O(1)$ order placement and cancellation.
- **Fixed-point arithmetic** avoiding floating-point imprecision and non-deterministic FPU cycles.

---

## Architecture & Internals

```
                    ┌─────────────────────────┐
                    │      Order Ingress      │
                    │  (Limit / Market / IOC) │
                    └────────────┬────────────┘
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │   Lock-Free SPSC Queue  │  alignas(128) Cursors
                    │  (Acquire-Release Sync) │  Power-of-Two Ring Buffer
                    └────────────┬────────────┘
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │   Matching Engine Core  │
                    ├─────────────────────────┤
                    │ • Price-Time Priority   │  alignas(64) Order & Level
                    │ • Pre-allocated MemPool │  Zero-Heap Allocations
                    │ • Monotonic PMR Buffers │  O(1) Insert / Cancel
                    └────────────┬────────────┘
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │   Zero-Copy Event Bus   │  Trivially Copyable Unions
                    │  (Trade, Filled, Rest)  │  Nanosecond Timestamps
                    └─────────────────────────┘
```

---

## Core Features & Design Principles

### 1. Lock-Free SPSC Ring Buffer (`spsc_queue.h`)
- Implements a single-producer single-consumer circular queue with bitwise power-of-two mask indexing.
- Cursor positions are padded to `alignas(128)` bytes to ensure read and write cursors occupy distinct cache lines, completely avoiding false sharing between producer and consumer cores.
- Explicit `std::memory_order_acquire` and `std::memory_order_release` semantics guarantee sequential memory visibility without mutex locks or kernel context switches.

### 2. Pre-Allocated Intrusive Memory Pool (`memory_pool.h`)
- Fixed-capacity memory pool managing hot-path order descriptors.
- Free slots form an intrusive singly-linked list embedded within the storage blocks themselves.
- Slot alignment is bounded by `alignas(64)` to ensure every allocated order object lands on an independent CPU cache boundary.

### 3. Price-Time Priority Order Book (`order_book.h`, `level.h`, `order.h`)
- **Price Levels (`Level`)**: Cache-aligned structure maintaining aggregated depth and order count. Houses an intrusive doubly-linked list (`prev`/`next` pointers inside `Order`), enabling guaranteed $O(1)$ order insertions at the tail and $O(1)$ cancellations from arbitrary book positions.
- **Order Model (`Order`)**: Strict `alignas(64)` layout bounded by `static_assert(sizeof(Order) <= 128)`. Supports multiple execution conditions:
  - `LIMIT`, `MARKET`
  - Immediate-or-Cancel (`IOC`), Fill-or-Kill (`FOK`), Good-'Til-Cancelled (`GTC`)
  - Stop & Stop-Limit orders (`STOP`, `STOP_LIMIT`)
  - Iceberg orders (`ICEBERG`) with automated display and hidden quantity tranche replenishment
  - Post-Only orders (`POST_ONLY`)

### 4. Zero-Copy Event Model (`events.h`)
- Flat, trivially copyable event union (`Event`) encapsulating execution states:
  - `OrderAccepted`, `OrderRested`, `OrderRejected`, `OrderReduced`, `OrderPartiallyFilled`, `OrderFilled`, `OrderCanceled`, `Trade`
- Every event carries monotonically increasing sequence numbers and high-precision timestamps (`now_ns()` / `LogicalClock`) for audit logs and market data replication.

### 5. Strict Systems Compilation Flags
- Compiles with `-fno-exceptions`, `-Wall`, `-Wextra`, and `-Wpedantic` in C++20 mode.
- Avoids runtime exception handling overhead and RTTI, maintaining deterministic execution branches.

---

## Technical Specifications

| Parameter | Specification | Implementation Detail |
| :--- | :--- | :--- |
| **Language Standard** | C++20 | Zero-overhead abstractions, `std::is_trivially_copyable_v` |
| **Pricing Precision** | Fixed-point `int64_t` | `PRICE_SCALE = 10000` (4 decimal places) |
| **Queue Concurrency** | Wait-Free SPSC | Acquire-release atomics, 128-byte cursor separation |
| **Memory Policy** | Zero hot-path heap allocation | Intrusive memory pool & monotonic PMR resources |
| **Order Book Priority** | Price-Time (FIFO) | Doubly-linked intrusive nodes with $O(1)$ book manipulation |
| **Cache Optimization** | 64-byte / 128-byte cacheline alignment | Eliminates false sharing and optimizes L1/L2 data prefetching |

---

## Quick Start

### Requirements
- C++20 capable compiler (`clang++` 14+, `g++` 11+, or MSVC v143+)
- CMake 3.24+

### Build

```powershell
git clone https://github.com/Umair-Manzoor-47/Holt.git
cd Holt

# Configure release build with optimizations
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Run Executables

```powershell
# Run main Holt driver
./build/src/holt

# Run trade processing utility
./build/tools/holt_trades

# Run liquidity generator utility
./build/tools/holt_liquidity
```

---

## Project Roadmap

- [x] Cache-aligned data structures (`Order`, `Level`, `Event`)
- [x] Lock-free SPSC ring buffer with false-sharing protection
- [x] Intrusive pre-allocated memory pool and monotonic PMR buffer
- [x] Price level $O(1)$ intrusive list operations
- [ ] Multi-symbol order book dispatching
- [ ] Comprehensive microbenchmark harness (nanosecond tick-to-trade measurement)
- [ ] Network gateway ingestion (ITCH/OUCH or binary protocol parser)

---

## Links

- **Author**: [Umair Manzoor](https://www.linkedin.com/in/umair-manzoor/)
- **GitHub**: [github.com/Umair-Manzoor-47/Holt](https://github.com/Umair-Manzoor-47/Holt)
- **License**: [MIT](LICENSE)
