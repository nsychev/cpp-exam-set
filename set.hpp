#ifndef SET
#define SET

#include <utility>
#include <type_traits>
#include <iterator>
#include <optional>
#include <cassert>

template<typename T>
struct set {
private:
    struct node;

    struct base_node {
        node *left = nullptr, *right = nullptr;

        ~base_node() {
            if (left)
                delete left;
            if (right)
                delete right;
        }
    };

    struct node: base_node {
        T data;
        base_node *parent;

        node() = delete;
        node(T const& value, base_node* parent): data(value), parent(parent) {};
    };

    size_t _size;
    base_node root;
public:
    struct iterator: public std::iterator<std::bidirectional_iterator_tag, T const> {
        iterator() noexcept: ptr(nullptr) {}
        iterator(base_node const* ptr) noexcept: ptr(ptr) {}

        T const& operator*() const {
            return static_cast<node const*>(ptr)->data;
        }

        T const* operator->() const {
            return &(static_cast<node const*>(ptr)->data);
        }

        iterator operator++() {
            if (ptr->right) {
                ptr = ptr->right;
                while (ptr->left)
                    ptr = ptr->left;
            } else {
                base_node const* w = ptr;
                ptr = static_cast<node const*>(ptr)->parent;
                while (ptr && ptr->right == w) {
                    w = ptr;
                    ptr = static_cast<node const*>(ptr)->parent;
                }
            }

            return *this;
        }

        iterator operator--() {
            if (ptr->left) {
                ptr = ptr->left;
                while (ptr->right)
                    ptr = ptr->right;
            } else {
                base_node const *w = ptr;
                ptr = static_cast<node const*>(ptr)->parent;
                while (ptr && ptr->left == w) {
                    w = ptr;
                    ptr = static_cast<node const*>(ptr)->parent;
                }
            }

            return *this;
        }

        iterator const operator++(int) {
            iterator other = *this;
            ++*this;
            return other;
        }

        iterator const operator--(int) {
            iterator other = *this;
            --*this;
            return other;
        }

        friend bool operator==(iterator const& a, iterator const& b) noexcept {
            return a.ptr == b.ptr;
        }

        friend bool operator!=(iterator const& a, iterator const& b) noexcept {
            return a.ptr != b.ptr;
        }
    private:
        base_node const *ptr;

        friend class set;
    };

    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    set() noexcept: _size(0), root() {
    }

    set(const set& other): set() {
        try {
            for (auto &e: other)
                insert(e);
        } catch (...) {
            clear();
            throw;
        }
    }

    set& operator=(set other) noexcept {
        swap(*this, other);
        return *this;
    }

    ~set() = default;

    const_iterator begin() const noexcept {
        base_node const *ptr = &root;
        while (ptr->left)
            ptr = ptr->left;
        return ptr;
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator end() const noexcept {
        return &root;
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    const_reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    std::pair<iterator, bool> insert(T const& value) {
        node *v = root.left;
        base_node *p = &root;

        while (v) {
            p = v;
            if (value < v->data) {
                v = v->left;
            } else if (v->data < value) {
                v = v->right;
            } else {
                return std::make_pair(iterator(v), false);
            }
        }

        _size++;
        if (p == &root || value < static_cast<node*>(p)->data) {
            v = p->left = new node(value, p);
        } else {
            v = p->right = new node(value, p);
        }
        return std::make_pair(iterator(v), true);
    }

    const_iterator find(T const& value) const noexcept {
        node *v = root.left;

        while (v) {
            if (value < v->data) {
                v = v->left;
            } else if (v->data < value) {
                v = v->right;
            } else {
                return v;
            }
        }

        return &root;
    }

    const_iterator lower_bound(T const& value) const noexcept {
        node *v = root.left;

        while (v) {
            if (value < v->data) {
                if (!v->left) {
                    return v;
                }
                v = v->left;
            } else if (v->data < value) {
                if (!v->right) {
                    const_iterator result(v);
                    return ++result;
                }
                v = v->right;
            } else {
                return v;
            }
        }

        return &root;
    }

    const_iterator upper_bound(T const& value) const {
        node *v = root.left;

        while (v) {
            if (value < v->data) {
                if (!v->left) {
                    return v;
                }
                v = v->left;
            } else {
                if (!v->right) {
                    const_iterator result(v);
                    return ++result;
                }
                v = v->right;
            }
        }

        return &root;
    }

    iterator erase(const_iterator it) {
        --_size;

        iterator result = it;
        ++result;

        node *v = const_cast<node*>(static_cast<node const*>(it.ptr));
        base_node *p = v->parent;

        if (!v->left) {
            if (p->left == v)
                p->left = v->right;
            else
                p->right = v->right;

            if (v->right) {
                v->right->parent = p;
                v->right = nullptr;
            }
        } else if (!v->right) {
            if (p->left == v)
                p->left = v->left;
            else
                p->right = v->left;

            if (v->left) {
                v->left->parent = p;
                v->left = nullptr;
            }
        } else {
            node *next = const_cast<node*>(static_cast<node const*>(result.ptr));
            assert(next->left == nullptr);
            if (next->parent == v) {
                if (p->left == v)
                    p->left = next;
                else
                    p->right = next;

                v->left->parent = next;
                next->parent = p;
                next->left = v->left;
            } else {
                if (next->parent->left == next) {
                    next->parent->left = next->right;
                } else {
                    next->parent->right = next->right;
                }

                if (next->right)
                    next->right->parent = next->parent;

                next->left = v->left;
                next->right = v->right;

                v->left->parent = next;
                v->right->parent = next;
                next->parent = p;

                if (p->left == v)
                    p->left = next;
                else
                    p->right = next;
            }

            v->left = nullptr;
            v->right = nullptr;
        }

        delete v;

        return result;
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    void clear() {
        if (root.left)
            delete root.left;
        root.left = nullptr;
        _size = 0;
    }

    friend void swap(set<T>& a, set<T>& b) {
        std::swap(a._size, b._size);
        std::swap(a.root.left, b.root.left);

        if (a.root.left)
            a.root.left->parent = &a.root;

        if (b.root.left)
            b.root.left->parent = &b.root;
    }
};

#endif // SET
