#ifndef SET
#define SET

#include <utility>
#include <type_traits>
#include <iterator>
#include <memory>
#include <optional>

template<typename T>
struct set {
private:
    struct node {
        std::optional<T> data;
        std::shared_ptr<node> left, right;
        std::weak_ptr<node> parent;

        node() = default;
        node(T const& value, std::shared_ptr<node>& parent): data(value), parent(parent) {};

        ~node() = default;
    };

    size_t _size;
    std::shared_ptr<node> root;
public:
    struct iterator: public std::iterator<std::bidirectional_iterator_tag, T const> {
        iterator(): ptr(nullptr) {}
        iterator(std::shared_ptr<node> const& ptr): ptr(ptr) {}

        T const& operator*() const {
            return ptr.get()->data.value();
        }

        T const* operator->() const {
            return &ptr.get()->data.value();
        }

        iterator operator++() {
            if (ptr->right) {
                ptr = ptr->right;
                while (ptr->left)
                    ptr = ptr->left;
            } else {
                std::shared_ptr<node> w = ptr;
                ptr = ptr->parent.lock();
                while (ptr && ptr->right == w) {
                    w = ptr;
                    ptr = ptr->parent.lock();
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
                std::shared_ptr<node> w = ptr;
                ptr = ptr->parent.lock();
                while (ptr && ptr->left == w) {
                    w = ptr;
                    ptr = ptr->parent.lock();
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

        friend bool operator==(iterator const& a, iterator const& b) {
            return a.ptr == b.ptr;
        }

        friend bool operator!=(iterator const& a, iterator const& b) {
            return a.ptr != b.ptr;
        }
    private:
        std::shared_ptr<node> ptr;

        friend class set;
    };

    typedef iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    set(): _size(0), root(std::make_shared<node>()) {}

    set(const set& other): set() {
        try {
            for (auto &e: other)
                insert(e);
        } catch (...) {
            clear();
            throw;
        }
    }

    set& operator=(set other) {
        swap(*this, other);
        return *this;
    }

    ~set() = default;

    const_iterator begin() const {
        std::shared_ptr<node> ptr = root;
        while (ptr->left)
            ptr = ptr->left;
        return ptr;
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator end() const {
        return const_iterator(root);
    }

    const_iterator cend() const {
        return end();
    }

    const_reverse_iterator rbegin() const {
        return std::make_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const {
        return rbegin();
    }
    const_reverse_iterator rend() const {
        return std::make_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const {
        return rend();
    }

    std::pair<iterator, bool> insert(T const& value) {
        std::shared_ptr<node> v = root;

        while (true) {
            if (!v->data || value < v->data) {
                if (v->left) {
                    v = v->left;
                } else {
                    v->left = std::make_shared<node>(value, v);
                    ++_size;
                    return std::make_pair(iterator(v->left), true);
                }
            } else if (v->data < value) {
                if (v->right) {
                    v = v->right;
                } else {
                    v->right = std::make_shared<node>(value, v);
                    ++_size;
                    return std::make_pair(iterator(v->right), true);
                }
            } else {
                return std::make_pair(iterator(v), false);
            }
        }
    }

    const_iterator find(T const& value) const {
        std::shared_ptr<node> v = root;

        while (v) {
            if (!v->data || value < v->data) {
                v = v->left;
            } else if (v->data < value) {
                v = v->right;
            } else {
                return v;
            }
        }

        return root;
    }

    const_iterator lower_bound(T const& value) const {
        std::shared_ptr<node> v = root;

        while (true) {
            if (!v->data || value < v->data) {
                if (!v->left)
                    return v;
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
    }

    const_iterator upper_bound(T const& value) const {
        std::shared_ptr<node> v = root;

        while (true) {
            if (!v->data || value < v->data) {
                if (!v->left)
                    return v;
                v = v->left;
            } else {
                if (!v->right) {
                    const_iterator result(v);
                    return ++result;
                }
                v = v->right;
            }
        }
    }

    iterator erase(const_iterator it) {
        if (it.ptr == root) {
            throw std::runtime_error("trying to delete root");
        }

        --_size;

        iterator result = it;
        ++result;

        std::shared_ptr<node> v = it.ptr;
        std::shared_ptr<node> p = v->parent.lock();
        if (!v->left) {
            if (p->left == v)
                p->left = v->right;
            else
                p->right = v->right;

            if (v->right)
                v->right->parent = p;
        } else if (!v->right) {
            if (p->left == v)
                p->left = v->left;
            else
                p->right = v->left;

            if (v->left)
                v->left->parent = p;
        } else {
            ++it;
            std::shared_ptr<node> next = it.ptr;

            std::shared_ptr<node> next_parent = next->parent.lock();
            if (next_parent->left == next)
                next_parent->left = next->right;
            else
                next_parent->right = next->right;

            next->right->parent = next_parent;

            v->left->parent = next;
            v->right->parent = next;
            next->parent = p;

            if (p->left == v)
                p->left = next;
            else
                p->right = next;

            next->left = v->left;
            next->right = v->right;
        }

        return result;
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    void clear() {
        root = std::make_shared<node>();
        _size = 0;
    }

    friend void swap(set<T>& a, set<T>& b) {
        std::swap(a.root, b.root);
        std::swap(a._size, b._size);
    }
};

#endif // SET
