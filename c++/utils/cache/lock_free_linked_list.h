#pragma once
#include <atomic>
#include <utility>
#include <memory>
#include <functional>

struct MarkNode {
    MarkNode() : ptr(0), mark(0), flag(0) { }
    template<typename T>
    MarkNode(T *p, bool m, bool f) :
            ptr(reinterpret_cast<int64_t>(p)), mark(m), flag(f) { }

    template<typename T>
    T *GetPtr() { return reinterpret_cast<T*>(ptr); }
    bool IsMarked() { return mark; }
    bool IsFlagged() { return flag; }

    bool operator==(const MarkNode &other_node) {
        return ptr == other_node.ptr &&
               mark == other_node.mark &&
               flag == other_node.flag;
    }

    int64_t ptr : 62;
    int64_t mark : 1;
    int64_t flag : 1;
};

template<typename T> struct Node {
    template<typename ...Args>
    Node(Args ...args) : value(new T(std::forward<Args>(args)...)) { }
    Node(T *t) : value(t) { }

    bool IsMarked() { return next.load().IsMarked(); }
    bool IsFlagged() { return next.load().IsFlagged(); }
    Node<T> *Next() { return next.load().template GetPtr<Node<T>>(); }
    void SetPrev(Node<T> *p) { prev = p; }

    std::unique_ptr<T> value;
    Node<T> *prev;
    std::atomic<MarkNode> next;
};

template<typename T, typename Equal=std::equal_to<T>>
struct List {
    List() : head() { }

    void HelpMarked(Node<T> *prev_node, Node<T> *del_node) {
        MarkNode dn(del_node, false, true), mn(del_node->Next(), false, false);
        prev_node->next.compare_exchange_strong(dn, mn);
    }

    std::pair<MarkNode, Node<T> *> SearchFrom(const T &k, Node<T> *cur_node) {
        Node<T> *next_node{nullptr};
        bool equal{true};
        while ((next_node = cur_node->Next()) &&
               !(equal = Equal()(*(next_node->value), k))) {
            while (next_node->IsMarked() &&
                   (!cur_node->IsMarked() ||
                    cur_node->Next() != next_node)) {
                if (cur_node->Next() == next_node) {
                    HelpMarked(cur_node, next_node);
                }
                next_node = cur_node->Next();
            }
            if (!equal) {
                cur_node = next_node;
                next_node = cur_node->Next();
            }
        }
        bool already_equal = (equal && cur_node != &head);
        if (already_equal) {
            cur_node = next_node;
            next_node = cur_node->Next();
        }
        return std::make_pair(MarkNode(cur_node, already_equal, 0),
                              next_node);
    }

    std::pair<Node<T> *, bool> TryFlag(Node<T> *prev_node, Node<T> *target_node) {
        // Attempts to flag the predecessor of target_node.
        // Prev_node is the last node known to be the predecessor.
        while (true) {
            if (prev_node->next.load() == MarkNode(target_node, false, true)) {
                return std::make_pair(prev_node, false);
            }
            MarkNode result(target_node, false, false);
            prev_node->next.compare_exchange_strong(result, MarkNode(target_node, false, true));
            if (result == MarkNode(target_node, false, false)) {
                return std::make_pair(prev_node, true);
            }
            if (result == MarkNode(target_node, false, true)) {
                return std::make_pair(prev_node, false);
            }
            while (prev_node->IsMarked()) { prev_node = prev_node->prev; }
        }
        auto ret = SearchFrom(*target_node->value, prev_node);
        if (ret.second != target_node) { return MarkNode(); }
    }

    void TryMark(Node<T> *del_node) {
        // Attempts to mark the node del_node
        do {
            auto *next_node = del_node->Next();
            MarkNode expect_next(next_node, false, false);
            MarkNode new_next(next_node, true, false);
            del_node->next.compare_exchange_strong(expect_next, new_next);
            if (!expect_next.IsMarked() && expect_next.IsFlagged()) {
                HelpFlagged(del_node, expect_next.GetPtr<Node<T>>());
            }
        } while (del_node->IsMarked());
    }

    void HelpFlagged(Node<T> *prev_node, Node<T> *del_node) {
        // Atempts to mark and physically delete node del_node,
        // which is the successor of the flagged node prev_node.
        del_node->prev = prev_node;
        if (!del_node->IsMarked()) { TryMark(del_node); }
        HelpMarked(prev_node, del_node);
    }

    Node<T> *Search(const T &k) {
        auto ret = SearchFrom(k, &head);
        if (!ret.first.IsMarked()) { return nullptr; }
        return ret.first.template GetPtr<Node<T>>();
    }

    Node<T> *Delete(const T &k) {
        auto ret = SearchFrom(k, &head);
        if (!ret.first.IsMarked()) { return nullptr; }
        auto res = TryFlag(ret.first.template GetPtr<Node<T>>(), ret.second);
        if (res.template GetPtr<Node<T>>()) {
            HelpFlagged(res.template GetPtr<Node<T>>(), ret.second);
        }
        if (!res.second) { return nullptr; }
        return ret.second;
    }

    std::pair<Node<T> *, bool> Insert(const T &k) {
        // Attempts to insert a new node with supplied key.
        auto ret = SearchFrom(k, &head);
        Node<T> *prev_node = ret.first.template GetPtr<Node<T>>();
        Node<T> *next_node = ret.second;
        if (ret.first.IsMarked()) {
            return std::make_pair(prev_node, false);
        }
        Node<T> *new_node = new Node<T>(k);
        while (true) {
            MarkNode prev_succ = prev_node->next.load();
            if (prev_succ.IsFlagged()) {
                HelpFlagged(prev_node, prev_succ.template GetPtr<Node<T>>());
            } else {
                MarkNode tmp(next_node, false, false), new_mnode(new_node, false, false);
                new_node->next.store(tmp);
                prev_node->next.compare_exchange_strong(tmp, new_mnode);
                if (tmp == MarkNode(next_node, false, false)) {
                    return std::make_pair(new_node, true);
                } else {
                    if (!tmp.IsMarked() && tmp.IsFlagged()) {
                        HelpFlagged(prev_node, tmp.template GetPtr<Node<T>>());
                    }
                    while (prev_node->IsMarked()) {
                        prev_node = prev_node->prev;
                    }
                }
            }
            ret = SearchFrom(k, prev_node);
            if (ret.first.IsMarked()) {
                delete new_node;
                return std::make_pair(ret.first.template GetPtr<Node<T>>(), false);
            }
        }
    }

    Node<T> *Begin() { return head.Next(); }
    Node<T> *End() { return nullptr; }

    Node<T> head;
};
