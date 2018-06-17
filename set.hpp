#ifndef SET
#define SET

#include <utility>
#include <type_traits>
#include <iterator>
#include <optional>

template<typename T>
struct set {
private:
    struct node {
        std::optional<T> data;
        node *left = nullptr, *right = nullptr, *parent = nullptr;

        node() = default;
        node(T const& value, node* parent): data(value), parent(parent) {};

        ~node() {
            delete left;
            delete right;
        }
    };

    size_t _size;
    node root;
public:
    struct iterator: public std::iterator<std::bidirectional_iterator_tag, T const> {
        iterator(): ptr(nullptr) {}
        iterator(node* ptr): ptr(ptr) {}

        T const& operator*() const {
            return ptr->data.value();
        }

        T const* operator->() const {
            return &(ptr->data.value());
        }

        iterator operator++() {
            if (ptr->right) {
                ptr = ptr->right;
                while (ptr->left)
                    ptr = ptr->left;
            } else {
                node *w = ptr;
                ptr = ptr->parent;
                while (ptr && ptr->right == w) {
                    w = ptr;
                    ptr = ptr->parent;
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
                node *w = ptr;
                ptr = ptr->parent;
                while (ptr && ptr->left == w) {
                    w = ptr;
                    ptr = ptr->parent;
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
        node *ptr;

        friend class set;
    };

    typedef iterator const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef reverse_iterator const_reverse_iterator;

    set(): _size(0), root() {}

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
        node *ptr = const_cast<node*>(&root);
        while (ptr->left)
            ptr = ptr->left;
        return ptr;
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator end() const {
        return const_iterator(const_cast<node*>(&root));
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
        node *v = &root;

        while (true) {
            if (!v->data || value < v->data) {
                if (v->left) {
                    v = v->left;
                } else {
                    v->left = new node(value, v);
                    ++_size;
                    return std::make_pair(iterator(v->left), true);
                }
            } else if (v->data < value) {
                if (v->right) {
                    v = v->right;
                } else {
                    v->right = new node(value, v);
                    ++_size;
                    return std::make_pair(iterator(v->right), true);
                }
            } else {
                return std::make_pair(iterator(v), false);
            }
        }
    }

    const_iterator find(T const& value) const {
        node *v = const_cast<node*>(&root);

        while (v) {
            if (!v->data || value < v->data) {
                v = v->left;
            } else if (v->data < value) {
                v = v->right;
            } else {
                return v;
            }
        }

        return const_cast<node*>(&root);
    }

    const_iterator lower_bound(T const& value) const {
        node *v = const_cast<node*>(&root);

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
        node *v = const_cast<node*>(&root);

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
        --_size;

        iterator result = it;
        ++result;

        node *v = it.ptr;
        node *p = v->parent;
        if (!v->left) {
            if (p->left == v)
                p->left = v->right;
            else
                p->right = v->right;

            if (v->right)
                v->right->parent = p;

            v->right = nullptr;
        } else if (!v->right) {
            if (p->left == v)
                p->left = v->left;
            else
                p->right = v->left;

            if (v->left)
                v->left->parent = p;

            v->left = nullptr;
        } else {
            ++it;
            node *next = it.ptr;

            node *next_parent = next->parent;

            if (next_parent == v) {
                if (p->left == v)
                    p->left = next;
                else
                    p->right = next;

                v->left->parent = next;
                next->parent = p;
                next->left = v->left;
            } else  {
                if (next_parent->left == next)
                    next_parent->left = next->right;
                else
                    next_parent->right = next->right;

                if (next->right)
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
        root = node();
        _size = 0;
    }

    friend void swap(set<T>& a, set<T>& b) {
        std::swap(a.root.data, b.root.data);
        std::swap(a.root.left, b.root.left);
        std::swap(a._size, b._size);

        if (a.root.left)
            a.root.left->parent = &a.root;


        if (b.root.left)
            b.root.left->parent = &b.root;
    }
};

#endif // SET
