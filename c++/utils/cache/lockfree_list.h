#include <iostream>
#include <atomic>

template<typename T>
struct Node {
    Node() = default;
    Node(T *v) : value(v) { }
    std::unique_ptr<T> value {nullptr};
    std::atomic<Node<T> *> next {nullptr};
};

template<typname T>
class List {
 public:
    List() : head(new Node<T>()), tail(new Node<T>()) {
        head.next = tail;
    }

    bool Insert(T *v) {
        Node<T> *node  = new Node<T>(v);
        Node<T> *prev, *after;
        do {

        } while (true);
    }

    void Search(T *value, Node<T> **prev, Node<T> **cur) {
        Node<T> *prev_next;
        do {
            Node<T> *t = &head;
            Node<T> *t_next = head->next;
            do {
                if (!IsMarkedRef(t_next)) {
                    *prev = t;
                    prev_next = t_next;
                }
                t = UnmarkedRef(t_next);
                if (t == &tail) {
                    break;
                }
                t_next = t->next;
            } while (IsMarkedRef(t_next) || compare)

        } while (true);
    }

    Node<T> *MarkedRef(Node<T> *origin) {
        return static_cast<Node<T> *>(static_cast<int64_t>(origin) | 0x1l);
    }

    Node<T> *UnmarkedRef(Node<T> *marked) {
        return static_cast<Node<T> *>(static_cast<int64_t>(marked) & ~(int64_t(0x1l)));
    }

    int IsMarkedRef(Node<T> *ref) {
        return (static_cast<int64_t>(ref) & 0x1l);
    }

 private:
    Node<T> head;
    Node<T> tail;
};

