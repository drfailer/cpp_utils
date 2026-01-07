// source: https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

#ifndef CPP_UTILS_ARENA_H
#define CPP_UTILS_ARENA_H
#include <cassert>
#include <cstring>
#include <cstdint>

namespace mem {

struct ArenaRegion {
    size_t pos = 0;
    size_t mem_len;
    char *mem = nullptr;
    ArenaRegion *next = nullptr;
    ArenaRegion *prev = nullptr;

    ArenaRegion(size_t mem_len, ArenaRegion *prev = nullptr)
        : mem_len(mem_len), mem(new char[mem_len]), prev(prev) {}
    ~ArenaRegion() { delete[] mem; }

    bool is_power_of_two(uintptr_t x) {
        return (x & (x-1)) == 0;
    }

    uintptr_t align_forward(uintptr_t ptr, size_t align) {
        assert(is_power_of_two(align));
        uintptr_t a = (uintptr_t)align;
        uintptr_t mod = ptr & (a - 1);
        if (mod != 0) {
            ptr += a - mod;
        }
        return ptr;
    }

    void *alloc(size_t size, size_t align) {
        uintptr_t addr = align_forward((uintptr_t)&this->mem[this->pos], align);
        size_t pos = (size_t)(addr - (uintptr_t)this->mem);

        if (pos + size <= this->mem_len) {
            void *ptr = &this->mem[pos];
            this->pos = pos + size;
            memset(ptr, 0, size);
            return ptr;
        }
        return nullptr;
    }
};

struct Arena {
    ArenaRegion *head = nullptr;
    ArenaRegion *tail = nullptr;
    size_t region_size = 0;

    Arena(size_t region_size) : head(new ArenaRegion(region_size)), region_size(region_size) {
        this->tail = this->head;
    }

    ~Arena() {
        auto cur = this->head;
        while (cur != nullptr) {
            auto next = cur->next;
            delete cur;
            cur = next;
        }
    }

    template <typename T>
    T *alloc(size_t size = 1, size_t align = 2*sizeof(void*)) {
        assert(size <= this->region_size);

        auto region = this->head;
        while (region != nullptr) {
            void *ptr = region->alloc(size * sizeof(T), align);
            if (ptr != nullptr) {
                return (T*)ptr;
            }
            region = region->next;
        }

        region = new ArenaRegion(this->region_size);
        this->tail->next = region;
        region->prev = this->tail;
        this->tail = region;
        return (T*)region->alloc(size * sizeof(T), align);
    }
};

} // end namespace mem

#endif
