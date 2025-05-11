#ifndef ESET_HPP
#define ESET_HPP
#include <algorithm>
#include <utility>
template <typename Key>
class ESet {
private:
    struct Node {
        Key key;
        Node* _left;
        Node* _right;
        int height;

        Node(const Key& k) : key(k), _left(nullptr), _right(nullptr), height(1) {}
        Node(Key&& k) : key(std::move(k)), _left(nullptr), _right(nullptr), height(1) {}
    };

    Node* root;
    size_t count;
    int height(Node* node) const {
        return node ? node->height : 0;
    }
    int balance_factor(Node* node) const {
        return node ? height(node->_left) - height(node->_right) : 0;
    }
    void update_height(Node* node) {
        if (node) {
            node->height = 1 + std::max(height(node->_left), height(node->_right));
        }
    }
    Node* rotateR(Node* y) {
        Node* x = y->_left;
        Node* T2 = x->_right;
        x->_right = y;
        y->_left = T2;
        update_height(y);
        update_height(x);
        return x;
    }

    Node* roteteL(Node* x) {
        Node* y = x->_right;
        Node* T2 = y->_left;

        y->_left = x;
        x->_right = T2;

        update_height(x);
        update_height(y);

        return y;
    }
    Node* rebalance(Node* node) {
        update_height(node);
        int bf = balance_factor(node);
        // LL
        if (bf > 1 && balance_factor(node->_left) >= 0) {
            return rotateR(node);
        }
        // LR
        if (bf > 1 && balance_factor(node->_left) < 0) {
            node->_left = roteteL(node->_left);
            return rotateR(node);
        }
        // RR
        if (bf < -1 && balance_factor(node->_right) <= 0) {
            return roteteL(node);
        }
        // RL
        if (bf < -1 && balance_factor(node->_right) > 0) {
            node->_right = rotateR(node->_right);
            return roteteL(node);
        }
        return node;
    }
    Node* insert(Node* node, const Key& key, bool& inserted) {
        if (!node) {
            count++;
            inserted = true;
            return new Node(key);
        }
        if (key < node->key) {
            node->_left = insert(node->_left, key, inserted);
        } else if (node->key < key) {
            node->_right = insert(node->_right, key, inserted);
        } else {
            inserted = false;
            return node;
        }
        return rebalance(node);
    }
    Node* insert(Node* node, Key&& key, bool& inserted) {
        if (!node) {
            count++;
            inserted = true;
            return new Node(std::move(key));
        }
        if (key < node->key) {
            node->_left = insert(node->_left, std::move(key), inserted);
        } else if (node->key < key) {
            node->_right = insert(node->_right, std::move(key), inserted);
        } else {
            inserted = false;
            return node;
        }
        return rebalance(node);
    }

    Node* min_value_node(Node* node) const {
        Node* current = node;
        while (current && current->_left) {
            current = current->_left;
        }
        return current;
    }

    Node* remove(Node* node, const Key& key, bool& removed) {
        if (!node) {
            removed = false;
            return node;
        }

        if (key < node->key) {
            node->_left = remove(node->_left, key, removed);
        } else if (node->key < key) {
            node->_right = remove(node->_right, key, removed);
        } else {
            if (!node->_left || !node->_right) {
                Node* temp = node->_left ? node->_left : node->_right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
                count--;
                removed = true;
            } else {
                Node* temp = min_value_node(node->_right);
                node->key = temp->key;
                node->_right = remove(node->_right, temp->key, removed);
            }
        }

        if (!node) {
            return node;
        }

        return rebalance(node);
    }

    Node* find_node(Node* node, const Key& key) const {
        if (!node) {
            return nullptr;
        }

        if (key < node->key) {
            return find_node(node->_left, key);
        } else if (node->key < key) {
            return find_node(node->_right, key);
        } else {
            return node;
        }
    }

    size_t range_count(Node* node, const Key& l, const Key& r) const {
        if (!node) {
            return 0;
        }

        if (node->key < l) {
            return range_count(node->_right, l, r);
        } else if (r < node->key) {
            return range_count(node->_left, l, r);
        } else {
            return 1 + range_count(node->_left, l, r) + range_count(node->_right, l, r);
        }
    }

    void clear(Node* node) {
        if (node) {
            clear(node->_left);
            clear(node->_right);
            delete node;
        }
    }

public:
    class iterator {
    private:
        Node* current;
        Node* root;
        Node* find_leftmost(Node* node) {
            while (node && node->_left) {
                node = node->_left;
            }
            return node;
        }
        Node* find_successor(Node* node) {
            if (node->_right) {
                return find_leftmost(node->_right);
            }
            Node* succ = nullptr;
            Node* ancestor = root;
            while (ancestor != node) {
                if (node->key < ancestor->key) {
                    succ = ancestor;
                    ancestor = ancestor->_left;
                } else {
                    ancestor = ancestor->_right;
                }
            }
            return succ;
        }
    public:
        iterator(Node* node, Node* r) : current(node), root(r) {}

        const Key& operator*() const {
            return current->key;
        }
        
        iterator& operator++() {
            if (current) {
                current = find_successor(current);
            }
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return current == other.current;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    ESet() : root(nullptr), count(0) {}
    ~ESet() { clear(root); }

    ESet& operator=(ESet&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            count = other.count;
            other.root = nullptr;
            other.count = 0;
        }
        return *this;
    }

    iterator begin() const {
        Node* node = root;
        while (node && node->_left) {
            node = node->_left;
        }
        return iterator(node, root);
    }

    iterator end() const {
        return iterator(nullptr, root);
    }

    iterator find(const Key& key) const {
        return iterator(find_node(root, key), root);
    }

    size_t range(const Key& l, const Key& r) const {
        if (l > r) return 0;
        return range_count(root, l, r);
    }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        Key key(std::forward<Args>(args)...);
        bool inserted = false;
        root = insert(root, std::forward<Key>(key), inserted);
        return {find(key), inserted};
    }

    size_t erase(const Key& key) {
        bool removed = false;
        root = remove(root, key, removed);
        return removed ? 1 : 0;
    }

    size_t size() const { return count; }
    bool empty() const { return count == 0; }
    void clear() { clear(root); root = nullptr; count = 0; }
};

#endif // ESET_HPP