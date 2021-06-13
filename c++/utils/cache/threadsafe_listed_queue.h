#include <iostream>
#include <atomic>

template<typename T>
struct ListedQueueNode {
    ListedQueueNode() = default;
    ListedQueueNode(T *v) : value(v) { }
    ListedQueueNode(const T &t) : value(new T(t)) { }
    ListedQueueNode(T &&t) : value(new T(t)) { }
    std::unique_ptr<T> value {nullptr};
    std::atomic<ListedQueueNode<T> *> next {nullptr};
};

template<typename T>
class ListedQueue {
 public:
    using Node = ListedQueueNode<T>;
    ListedQueue() : head_(), tail_(&head_) { }

    void PushBack(Node *node) {
        Node *tail {nullptr};
        while (true) {
            tail = tail_.load(std::memory_order_acquire);
            auto *next = tail->next.load(std::memory_order_acquire);
            // std::cout << "put tail:" << tail << ", next:" << next << ", node:" << node <<std::endl;
            if (next) { continue; }
            if (!tail->next.compare_exchange_weak(next, node, std::memory_order_release)) {
                // std::cout << "put 2" << std::endl;
                continue;
            }
               // std::cout << "put tail next:" << tail->next.load() << std::endl;
            if (tail_.compare_exchange_strong(tail, node, std::memory_order_release)) {
                std::cout << "put " << *(node->value) << std::endl;
                break;
            }
        }
    }

    Node *PopHead() {
        Node *p{nullptr}, *dummy{nullptr};
        while (true) {
            dummy = p = head_.next.load(std::memory_order_acquire);
            // std::cout << "init p:" << p << std::endl;
            if (!p) { break; }
            auto *next = p->next.load(std::memory_order_acquire);
            if (tail_.compare_exchange_strong(dummy, &head_, std::memory_order_release)) {
                if (!head_.next.compare_exchange_strong(dummy, nullptr, std::memory_order_release)) {
                    // std::cout << "pop 1" << std::endl;
                    continue;
                }
            } else {
                // p = head_.next.load(std::memory_order_acquire);
                if (!head_.next.compare_exchange_strong(p, next, std::memory_order_release)) {
                    // std::cout << "pop 2" << std::endl;
                    continue;
                }
            }
            break;
        }
        if (p) {
            std::cout << "get " << *(p->value) << std::endl;
        } else {
            std::cout << "get null" << std::endl;
        }
        return p;
    }

 private:
    Node head_;
    std::atomic<Node *> tail_{nullptr};
};
