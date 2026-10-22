#pragma once

#include <array>
#include <cstddef>
#include <memory>

namespace holt::core
{
    template <typename T, std::size_t N>
    class MemoryPool
    {
        static_assert(N>0, "MemoryPool Capacity must be positive");
        static_assert(sizeof(T)>= sizeof(void *), "Object storage must fit an intrusive free-list pointer.");

        struct alignas((alignof(T) > 64U) ? alignof(T) : 64U) Slot
        {
            std::array<std::byte, sizeof(T)> storage{};
        };

        struct FreeNode
        {
            FreeNode *next;
        };

    private:
        std::unique_ptr<Slot[]> slots_;
        FreeNode* free_list_head{nullptr};
        std::size_t allocated_count{0};

        void InitializeFreeList() noexcept
        {
            free_list_head = nullptr;
            for (std::size_t index = 0; index < N; ++index)
            {
                auto *node = reinterpret_cast<FreeNode *>(slots_[index].storage.data());
                node->next = free_list_head;
                free_list_head = node;
            }
        };
    public:

        MemoryPool() :
            slots_(std::make_unique<Slot[]>(N))
        { InitializeFreeList(); }

        MemoryPool(const MemoryPool&) = delete;
        MemoryPool &operator=(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&&) = delete;
        MemoryPool &operator=(MemoryPool&&) = delete;

        T *allocate() noexcept
        {
            if (free_list_head == nullptr) [[unlikely]] { return nullptr; }

            FreeNode *node = free_list_head;
            free_list_head = free_list_head->next;

            ++allocated_count;
            return reinterpret_cast<T *>(node);

        }
        void *deallocate(T *object) noexcept
        {
            if (object == nullptr) [[unlikely]] { return nullptr; }

            std::destroy_at(object);
            auto *node = reinterpret_cast<FreeNode *>(object);
            node->next = free_list_head;
            free_list_head = node;
            --allocated_count;
        }

        [[nodiscard]] constexpr std::size_t capacity() const noexcept { return N; }
        [[nodiscard]] bool empty() const noexcept {
            return free_list_head == nullptr;
        }
    };
}