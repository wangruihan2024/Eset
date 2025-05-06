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
        size_t _size;

        Node(T data, Color col, Node* parent, Node* left, Node* right) : _data(data), _col(col), _parent(parent), _left(left), _right(right), _size(1){}
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

    void RotateL(Node *&node) { //<-
        Node *rightchild = node->_right;
        node->_right = rightchild->_left;
        if(rightchild->_left)
            rightchild->_left->_parent = node;
        rightchild->_parent = node->_parent;
        if(!node->_parent)
            _root = rightchild;
        else if(node->_parent->_left == node)
            node->_parent->_left = rightchild;
        else
            node->_parent->_right = rightchild;
        rightchild->_left = node;
        node->_parent = rightchild;
        rightchild->_size = node->_size;
        node->_size = 1 + (node->_left ? node->_left->_size : 0) + (node->_right ? node->_right->_size : 0);
    }
    void RotateR(Node *&node) { //->
        #ifdef EXISTENCE
        if(!node)
            std::cerr << "rotateR of invalid node"<< std::endl;
        #endif
        Node *leftchild = node->_left;
        node->_left = leftchild->_right;
        if(leftchild->_right)
            leftchild->_right->_parent = node;
        leftchild->_parent = node->_parent;
        if(!node->_parent)
            _root = leftchild;
        else if(node->_parent->_left == node)
            node->_parent->_left = leftchild;
        else
            node->_parent->_right = leftchild;
        leftchild->_right = node;
        node->_parent = leftchild;
        leftchild->_size = node->_size;
        node->_size = 1 + (node->_left ? node->_left->_size : 0) + (node->_right ? node->_right->_size : 0);
    }
    void fixInsert(Node *&node) {
        while(node != _root && node->_parent->_col == Red) {
            if(node->_parent == node->_parent->_parent->_left) {
                Node *uncle = node->_parent->_parent->_right;
                if(uncle && uncle->_col == Red) { // uncle red
                    node->_parent->_col = Black;
                    uncle->_col = Black;
                    node->_parent->_parent->_col = Red;
                    node = node->_parent->_parent;
                }else { // uncle black
                    if(node == node->_parent->_right) {
                        node = node->_parent;
                        RotateL(node);
                    }
                    node->_parent->_col = Black;
                    node->_parent->_parent->_col = Red;
                    RotateR(node->_parent->_parent);
                }
            } else {
                Node* uncle = node->_parent->_parent->_left;
                if (uncle && uncle->_col == Red) {
                    node->_parent->_col = Black;
                    uncle->_col = Black;
                    node->_parent->_parent->_col = Red;
                    node = node->_parent->_parent;
                } else {
                    if (node == node->_parent->_left) {
                        node = node->_parent;
                        RotateR(node);
                    }
                    node->_parent->_col = Black;
                    node->_parent->_parent->_col = Red;
                    RotateL(node->_parent->_parent);
                }
            }
        }
        _root->_col = Black;
    }
    void fixDelete(Node *node, Node *parent) {
        while (node != _root && (!node || node->_col == Black)) {
            if (node == parent->_left) {
                Node* sibling = parent->_right;
                if (sibling->_col == Red) {
                    // Case 1: sibling is red
                    sibling->_col = Black;
                    parent->_col = Red;
                    RotateL(parent);
                    sibling = parent->_right;
                }
                if ((!sibling->_left || sibling->_left->_col == Black) &&
                    (!sibling->_right || sibling->_right->_col == Black)) {
                    // Case 2: both sibling's children are black
                    sibling->_col = Red;
                    node = parent;
                    parent = node->_parent;
                } else {
                    if (!sibling->_right || sibling->_right->_col == Black) {
                        // Case 3: sibling's right child is black
                        if (sibling->_left) sibling->_left->_col = Black;
                        sibling->_col = Red;
                        RotateR(sibling);
                        sibling = parent->_right;
                    }
                    // Case 4: sibling's right child is red
                    sibling->_col = parent->_col;
                    parent->_col = Black;
                    if (sibling->_right) sibling->_right->_col = Black;
                    RotateL(parent);
                    node = _root;
                }
            } else {
                // Symmetric cases
                Node* sibling = parent->_left;
                if (sibling->_col == Red) {
                    sibling->_col = Black;
                    parent->_col = Red;
                    RotateR(parent);
                    sibling = parent->_left;
                }
                if ((!sibling->_right || sibling->_right->_col == Black) &&
                    (!sibling->_left || sibling->_left->_col == Black)) {
                    sibling->_col = Red;
                    node = parent;
                    parent = node->_parent;
                } else {
                    if (!sibling->_left || sibling->_left->_col == Black) {
                        if (sibling->_right) sibling->_right->_col = Black;
                        sibling->_col = Red;
                        RotateL(sibling);
                        sibling = parent->_left;
                    }
                    sibling->_col = parent->_col;
                    parent->_col = Black;
                    if (sibling->_left) sibling->_left->_col = Black;
                    RotateR(parent);
                    node = _root;
                }
            }
        }
        if (node) 
            node->_col = Black;
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
            if(*this == _tree->begin())
                throw std::out_of_range("invalid minus");
            if(!_node) {
                _node = _tree->_root;
                while(_node && _node->_right)
                    _node = _node->_right;
            }else if(_node->_left) {
                _node = _node->_left;
                while(_node->_right)
                    _node = _node->_right;
            }else {
                Node *newNode = _node->_parent;
                while(newNode && _node == newNode->_left) {
                    _node = newNode;
                    newNode = newNode->_parent;
                }
                _node = newNode;
            }
            return *this;
        }

        const T &operator*() {
            if(!_node)
                throw std::out_of_range("invalid node");
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
        newNode->_size = node->_size;
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

    template< class... Args >
    std::pair<iterator, bool> emplace( Args&&... args ) {
        T value(std::forward<Args>(args)...);
        return insert(value);
    } 
    std::pair<iterator, bool> insert(const T& value) {
        if (!_root) {
            _root = new Node(value, Black, nullptr, nullptr, nullptr);
            _minNode = _root;
            _s = 1;
            return {iterator(this, _root), true};
        }
        Node* current = _root;
        Node* parent = nullptr;
        while (current) {
            parent = current;
            if (compare(value, current->_data)) {
                current = current->_left;
            } else if (compare(current->_data, value)) {
                current = current->_right;
            } else {
                return {iterator(this, current), false}; // already exists
            }
        }

        Node* newNode = new Node(value, Red, parent, nullptr, nullptr);
        if (compare(value, parent->_data)) {
            parent->_left = newNode;
            if (parent == _minNode)
                _minNode = newNode;
        } else {
            parent->_right = newNode;
        }
        _s++;
        for (Node* p = parent; p; p = p->_parent)
            p->_size++;
        fixInsert(newNode);
        return {iterator(this, newNode), true};
    }

    void transplant(Node *u, Node *v) {
        if(!u->_parent) {
            _root = v;
        }else if(u == u->_parent->_left) {
            u->_parent->_left = v;
        }else {
            u->_parent->_right = v;
        }
        if(v)
            v->_parent = u->_parent;
    }
    size_t erase(const T& key) {
        Node *node = _root;
        while(node) {
            if(compare(key, node->_data)) {
                node = node->_left;
            }else if(compare(node->_data, key)) {
                node = node->_right;
            }else {
                break;
            }
        }
        if(!node)
            return 0;
        if(node == _minNode) {
            _minNode = node->_right ? findMin(node->_right) : node->_parent;
        }
        Node *child, *parent;
        Color originalColor = node->_col;
        if(!node->_left) {
            child = node->_right;
            parent = node->_parent;
            transplant(node, node->_right);
        }else if(!node->_right) {
            child = node->_left;
            parent = node->_parent;
            transplant(node, node->_left);
        }else {
            Node *successor = findMin(node->_right);
            originalColor = successor->_col;
            child = successor->_right;
            parent = successor;
            if(successor->_parent == node) {
                if(child)
                    child->_parent = parent;
            }else {
                transplant(successor, successor->_right);
                successor->_right = node->_right;
                successor->_right->_parent = successor;
                parent = successor->_parent;
            }
            transplant(node, successor);
            successor->_left = node->_left;
            successor->_left->_parent = successor;
            successor->_col = node->_col;
            successor->_size = node->_size;
        }
        for (Node *p = parent; p; p = p->_parent)
            p->_size--;
        if(originalColor == Black)
            fixDelete(child, parent);
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
        if(compare(r, l))
            return 0;
        else
            return rank(r) - rank(l) + (find(l) == end() ? 0 : 1);
    }
    size_t rank(const T& key) const { // node下面的子点的个数
        size_t count = 0;
        Node *current = _root;
        while(current) {
            if(compare(key, current->_data)) { // key < current
                current = current->_left;
            }else if(compare(current->_data, key)) { // key > current
                count += 1 + (current->_left ? current->_left->_size : 0);
                current = current->_right;
            }else {
                count += (current->_left ? current->_left->_size : 0);
                break;
            }
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