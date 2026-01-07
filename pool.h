#ifndef CPP_UTILS_POOL
#define CPP_UTILS_POOL

namespace mem {

template <typename T>
struct PoolNode {
    T data = {};
    PoolNode<T> *prev = nullptr;
    PoolNode<T> *next = nullptr;
};

template <typename T>
struct Pool {
    PoolNode<T> *free_list_head = nullptr;
    PoolNode<T> *used_list_head = nullptr;

    Pool(size_t default_capacity = 0) {
        for (size_t i = 0; i < default_capacity; ++i) {
            auto node = new PoolNode<T>();
            node->next = this->free_list_head;
            this->free_list_head = node;
        }
    }

    ~Pool() {
        delete_list(this->free_list_head);
        delete_list(this->used_list_head);
    }

    void delete_list(PoolNode<T> *head) {
        while (head != nullptr) {
            auto next = head->next;
            delete head;
            head = next;
        }
    }

    T *alloc() {
        PoolNode<T> *node = nullptr;

        if (this->free_list_head == nullptr) {
            this->free_list_head = new PoolNode<T>();
        }
        node = this->free_list_head;
        this->free_list_head = node->next;
        node->next = this->used_list_head;
        if (node->next != nullptr) {
            node->next->prev = node;
        }
        this->used_list_head = node;
        return (T*)node;
    }

    void release(T *data) {
        auto node = (PoolNode<T>*)data;
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        } else {
            assert(this->used_list_head = node);
            this->used_list_head = node->next;
        }
        if (node->next != nullptr) {
            node->next->prev = node->prev;
        }
        node->prev = nullptr;
        node->next = this->free_list_head;
        this->free_list_head = node;
    }
};

} // end namespace mem

#endif
