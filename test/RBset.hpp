#include <iostream>
//#include <functional>
#include <stdexcept>
#include <cstdio>
#include <utility>
#define EXISTENCE 
enum Color
{
    Black,
    Red
};
template <class T>
class Node{
    public:
        T _data;
        Color _col;
        Node<T>* _parent;
        Node<T>* _left;
        Node<T>* _right;

        Node(T data, Color col, Node* parent, Node* left, Node* right) : _data(data), _col(col), _parent(parent), _left(left), _right(right){}
        Node(T data) : Node(data, Red, nullptr, nullptr, nullptr) {}
        ~Node() {}
};
template <class T, class Compare = std::less<T>>class ESet{
private:
    Compare compare{};
public:
    typedef Node<T> Node;
    Node *_root = nullptr;
    Node *_minNode = nullptr;
    size_t _s = 0;

    Node *findMin(Node *node) const {
        if(!node)
            return nullptr;
        while(node->_left)
            node = node->_left;
        return node;
    }

    void RotateL(Node *node) {
        Node *rightChild = node->_right;
        node->_right = rightChild->_left;
        if (node->_right) 
            node->_right->_parent = node;
        rightChild->_parent = node->_parent;
        if (!node->_parent) 
            _root = rightChild;
        else if (node == node->_parent->_left) 
            node->_parent->_left = rightChild;
        else 
            node->_parent->_right = rightChild; 
        rightChild->_left = node;
        node->_parent = rightChild;
    }
    void RotateR(Node *node) {
        Node *leftChild = node->_left;
        node->_left = leftChild->_right;
        if (node->_left) 
            node->_left->_parent = node;
        leftChild->_parent = node->_parent;
        if (!node->_parent) 
            _root = leftChild;
        else if (node == node->_parent->_left) 
            node->_parent->_left = leftChild;
        else 
            node->_parent->_right = leftChild;    
        leftChild->_right = node;
        node->_parent = leftChild;
    }
    void fixInsert(Node *node) {
        while (node != _root && node->_parent->_col == Red) {
            Node *parent = node->_parent;
            Node *grandparent = parent->_parent;
            
            if (parent == grandparent->_left) {
                Node *uncle = grandparent->_right;
                
                if (uncle && uncle->_col == Red) {  // Case 1: Uncle is red
                    parent->_col = Black;
                    uncle->_col = Black;
                    grandparent->_col = Red;
                    node = grandparent;
                } 
                else {
                    if (node == parent->_right) {  // Case 2: LR (Left-Right)
                        RotateL(parent);
                        node = parent;
                        parent = node->_parent;
                    }
                    // Case 3: LL (Left-Left)
                    parent->_col = Black;
                    grandparent->_col = Red;
                    RotateR(grandparent);
                }
            } 
            else {  // Symmetric case (parent is right child)
                Node *uncle = grandparent->_left;
                
                if (uncle && uncle->_col == Red) {  // Case 1: Uncle is red
                    parent->_col = Black;
                    uncle->_col = Black;
                    grandparent->_col = Red;
                    node = grandparent;
                } 
                else {
                    if (node == parent->_left) {  // Case 2: RL (Right-Left)
                        RotateR(parent);
                        node = parent;
                        parent = node->_parent;
                    }
                    // Case 3: RR (Right-Right)
                    parent->_col = Black;
                    grandparent->_col = Red;
                    RotateL(grandparent);
                }
            }
        }
        _root->_col = Black;  // Ensure root is black
    }
    void fixDelete(Node *node, Node *parent) {
        while (node != _root && (!node || node->_col == Black)) {
            if (node == parent->_left) {
                Node *bro = parent->_right;
                
                if (bro->_col == Red) {  // Case 1: Brother is red
                    parent->_col = Red;
                    bro->_col = Black;
                    RotateL(parent);
                    bro = parent->_right;
                }
                
                if ((!bro->_left || bro->_left->_col == Black) && 
                    (!bro->_right || bro->_right->_col == Black)) {  // Case 2: Both nephews black
                    bro->_col = Red;
                    node = parent;
                    parent = node->_parent;
                } 
                else {
                    if (!bro->_right || bro->_right->_col == Black) {  // Case 3: Right nephew black
                        bro->_left->_col = Black;
                        bro->_col = Red;
                        RotateR(bro);
                        bro = parent->_right;
                    }
                    // Case 4: Right nephew red
                    bro->_col = parent->_col;
                    parent->_col = Black;
                    bro->_right->_col = Black;
                    RotateL(parent);
                    node = _root;
                }
            } 
            else {  // Symmetric case (node is right child)
                Node *bro = parent->_left;
                
                if (bro->_col == Red) {  // Case 1: Brother is red
                    parent->_col = Red;
                    bro->_col = Black;
                    RotateR(parent);
                    bro = parent->_left;
                }
                
                if ((!bro->_left || bro->_left->_col == Black) && 
                    (!bro->_right || bro->_right->_col == Black)) {  // Case 2: Both nephews black
                    bro->_col = Red;
                    node = parent;
                    parent = node->_parent;
                } 
                else {
                    if (!bro->_left || bro->_left->_col == Black) {  // Case 3: Left nephew black
                        bro->_right->_col = Black;
                        bro->_col = Red;
                        RotateL(bro);
                        bro = parent->_left;
                    }
                    // Case 4: Left nephew red
                    bro->_col = parent->_col;
                    parent->_col = Black;
                    bro->_left->_col = Black;
                    RotateR(parent);
                    node = _root;
                }
            }
        }
        if (node) node->_col = Black;  // Ensure node is black
    }

    class iterator {
    public:
        Node *_node;
        const ESet<T, Compare> *_tree;

        iterator(const ESet<T, Compare> *tree, Node *node = nullptr) : _tree(tree), _node(node) {}
        iterator() : iterator(nullptr) {}
        iterator(const iterator& other) : _tree(other._tree), _node(other._node) {}
        
        iterator& operator=(const iterator &other) {
            if(this == &other)
                return *this;
            _tree = other._tree;
            _node = other._node;
            return *this;
        }
        iterator operator++(int) {
            iterator old = *this;
            ++*this;
            return old;
        }
        iterator &operator++() {
            if (!_node)
                return *this;
            if (_node->_right) {
                _node = _node->_right;
                while (_node->_left)
                    _node = _node->_left;
            } else {
                Node* parent = _node->_parent;
                while (parent && _node == parent->_right) {
                    _node = parent;
                    parent = parent->_parent;
                }
                _node = parent;
            }
            return *this;
        }
        iterator operator--(int) {
            iterator old = *this;
            --(*this);
            return old;
        }
        iterator &operator--() {
            if (!_node) {
                _node = _tree->_root;
                while (_node && _node->_right)
                    _node = _node->_right;
                return *this;
            }
            if (_node == _tree->_minNode)
                return *this;
            if (_node->_left) {
                _node = _node->_left;
                while (_node->_right)
                    _node = _node->_right;
            } else {
                Node *parent = _node->_parent;
                while (parent && _node == parent->_left) {
                    _node = parent;
                    parent = parent->_parent;
                }
                _node = parent;
            }  
            return *this;
        }

        const T &operator*() {
            if(!_node) {
                #ifdef EXISTENCE
                std::cout << "* of invalid node" << std::endl;
                #endif
                throw std::out_of_range("invalid node");
            }
                
            return _node->_data;
        }
        bool operator==(const iterator &other) const {
            return _tree == other._tree && _node == other._node;
        }
        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }
    };
    iterator begin() const noexcept{ return iterator(this, _minNode); }
    iterator end() const noexcept{ return iterator(this, nullptr); }

    ESet() {}
    ~ESet() {
        clear(_root);
        _root = nullptr;
        _minNode = nullptr;
        _s = 0;
    }
    void clear(Node *node) {
        if(node) {
            clear(node->_left);
            clear(node->_right);
            delete node;
        }
    }

    Node* copyTree(Node* node, Node *parent) {
        if(!node)
            return nullptr;
        Node *newNode = new Node(node->_data, node->_col, parent, nullptr, nullptr);
        newNode->_left = copyTree(node->_left, newNode);
        newNode->_right = copyTree(node->_right, newNode);
        return newNode;
    }
    ESet(const ESet& other) :compare(other.compare){
        _root = copyTree(other._root, nullptr);
        _minNode = findMin(_root);
        _s = other._s;
    }
    ESet& operator=(const ESet& other) {
        if(this == &other)
            return *this;
        clear(_root);
        compare = other.compare;
        _root = copyTree(other._root, nullptr);
        _minNode = findMin(_root);
        _s = other._s;
        return *this;
    }

    ESet(ESet&& other) noexcept: _root(other._root), _minNode(other._minNode), _s(other._s), compare(std::move(other.compare)) {
        other._root = other._minNode = nullptr;
        other._s = 0;
    }
    ESet& operator=(ESet&& other) noexcept {
        if(this == &other)
            return *this;
        clear(_root);
        _root = other._root;
        _minNode = other._minNode;
        _s = other._s;
        compare = std::move(other.compare);
        other._root = other._minNode = nullptr;
        other._s = 0;
        return *this;
    }

private:
    template <typename U>
    std::pair<iterator, bool> insert_impl(U&& value) {
        if (!_root) {
            _root = new Node(std::forward<U>(value), Black, nullptr, nullptr, nullptr);
            _minNode = _root;
            _s = 1;
            return {iterator(this, _root), true};
        }
        Node* current = _root;
        Node* parent = nullptr;
        bool is_left_child = false;
        while (current) {
            parent = current;
            if (compare(value, current->_data)) {
                current = current->_left;
                is_left_child = true;
            } else if (compare(current->_data, value)) {
                current = current->_right;
                is_left_child = false;
            } else {
                return {iterator(this, current), false};
            }
        }
        Node* newNode = new Node(std::forward<U>(value), Red, parent, nullptr, nullptr);
        if (is_left_child) {
            parent->_left = newNode;
            if (parent == _minNode) {
                _minNode = newNode;
            }
        } else {
            parent->_right = newNode;
        }
        _s++;
        fixInsert(newNode);
        return {iterator(this, newNode), true};
    }
    
public:
    template< class... Args >
    std::pair<iterator, bool> emplace( Args&&... args ) {
        return insert_impl(T(std::forward<Args>(args)...));
    }
    std::pair<iterator, bool> insert(const T& value) { return insert_impl(value);}
    std::pair<iterator, bool> insert(T&& value) { return insert_impl(std::move(value));}

    void transplant(Node *u, Node *v) {
        if (!u->_parent) {
            _root = v;
        } else if (u == u->_parent->_left) {
            u->_parent->_left = v;
        } else {
            u->_parent->_right = v;
        }
        if (v) {
            v->_parent = u->_parent;
        }
    }
    size_t erase(const T& key) {
        Node *node = _root;
        while (node) {
            if (compare(key, node->_data)) {
                node = node->_left;
            } else if (compare(node->_data, key)) {
                node = node->_right;
            } else {
                break;
            }
        }
        if (!node) return 0; 
        if (node == _minNode) {
            _minNode = (node->_right ? findMin(node->_right) : node->_parent);
        }
        Node *child = nullptr;
        Node *parent = nullptr;
        Color original_col = node->_col;
        if (!node->_left) {
            child = node->_right;
            parent = node->_parent;
            transplant(node, node->_right);
        } else if (!node->_right) {
            child = node->_left;
            parent = node->_parent;
            transplant(node, node->_left);
        } else {
            Node *successor = findMin(node->_right);
            original_col = successor->_col;
            child = successor->_right;
            parent = successor;
            if (successor->_parent != node) {
                transplant(successor, successor->_right);
                successor->_right = node->_right;
                successor->_right->_parent = successor;
                parent = successor->_parent;
            }
            transplant(node, successor);
            successor->_left = node->_left;
            successor->_left->_parent = successor;
            successor->_col = node->_col;
        }
        if (original_col == Black) {
            fixDelete(child, parent);
        }
        if (_root && !_minNode) {
            _minNode = findMin(_root);
        } else if (_minNode && _minNode->_parent && _minNode == _minNode->_parent->_right) {
            _minNode = _minNode->_parent;
        }
        delete node;
        _s--;
        return 1;
    }

    iterator find(const T& key) const {
        Node *newNode = _root;
        while(newNode) {
            if(compare(key, newNode->_data))
                newNode = newNode->_left;
            else if(compare(newNode->_data, key))
                newNode = newNode->_right;
            else
                return iterator(this, newNode);
        }
        return end();
    }

    size_t range(const T& l, const T& r) const {
        if (compare(r, l)) // r < l
            return 0;
        iterator first = lower_bound(l); 
        iterator last = upper_bound(r);  
        if (first == end())
            return 0;
        size_t count = 0;
        for (auto it = first; it != last; ++it) {
            ++count;
        }
        return count;
    }
    
    size_t size() const noexcept{
        return _s;
    }

    iterator lower_bound(const T& key) const { //>=
        Node *current = _root, *result = nullptr;
        while(current) {
            if(!compare(current->_data, key)) {
                result = current;
                current = current->_left;
            }else {
                current = current->_right;
            }
        }
        return iterator(this, result);
    }
    iterator upper_bound(const T& key) const {// >
        Node *current = _root, *result = nullptr;
        while(current) {
            if(compare(key, current->_data)) {
                result = current;
                current = current->_left;
            }else {
                current = current->_right;
            }
        }
        return iterator(this, result);
    } 
};