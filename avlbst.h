#ifndef RBBST_H
#define RBBST_H

#include "bst.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>

struct KeyError {};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data
 * member, plus other additional helper functions. You do NOT need to implement
 * any functionality or add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value> {
  public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since
    // they return pointers to AVLNodes - not plain Nodes. See the Node class in
    // bst.h for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

  protected:
    int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class
 * constructor and setting the color to red since every new node will be red
 * when it is first inserted.
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value,
                             AVLNode<Key, Value>* parent)
    : Node<Key, Value>(key, value, parent), balance_(0) {}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value> AVLNode<Key, Value>::~AVLNode() {}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const {
    return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance) {
    balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff) {
    balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is
 * necessary to make sure that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const {
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const {
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const {
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value> {
  public:
    virtual void insert(const std::pair<const Key, Value>& new_item);
    virtual void remove(const Key& key);

  protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);

    // Add helper functions here
  private:
    void insert_helper(const std::pair<const Key, Value>& new_item,
                       AVLNode<Key, Value>* node);
    void insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotate_right(AVLNode<Key, Value>* y);
    void rotate_left(AVLNode<Key, Value>* x);
    void remove_fix(AVLNode<Key, Value>* n, int diff);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    if (this->root_ == nullptr) {
        // Tree is empty
        this->root_ =
            new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    } else {
        insert_helper(new_item, (AVLNode<Key, Value>*)this->root_);
    }
}

// Recursive helper function for insert
template <class Key, class Value>
void AVLTree<Key, Value>::insert_helper(
    const std::pair<const Key, Value>& new_item, AVLNode<Key, Value>* node) {
    if (new_item.first < node->getKey()) {
        if (node->getLeft() == nullptr) {
            AVLNode<Key, Value>* n =
                new AVLNode<Key, Value>(new_item.first, new_item.second, node);
            node->setLeft(n);
            if (node->getBalance() != 0) {
                node->setBalance(0);
            } else {
                node->setBalance(-1);
                insert_fix(node, n);
            }
        } else {
            insert_helper(new_item, node->getLeft());
        }
    } else if (new_item.first == node->getKey()) {
        node->setValue(new_item.second);
    } else {
        // new_item.first > node->getKey()
        if (node->getRight() == nullptr) {
            AVLNode<Key, Value>* n =
                new AVLNode<Key, Value>(new_item.first, new_item.second, node);
            node->setRight(n);
            if (node->getBalance() != 0) {
                node->setBalance(0);
            } else {
                node->setBalance(1);
                insert_fix(node, n);
            }
        } else {
            insert_helper(new_item, node->getRight());
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::insert_fix(AVLNode<Key, Value>* p,
                                     AVLNode<Key, Value>* n) {
    if (p == nullptr || p->getParent() == nullptr) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();
    if (p == g->getLeft()) {
        // p is left child
        g->setBalance(g->getBalance() - 1);
        switch (g->getBalance()) {
        case 0:
            return;
        case -1:
            insert_fix(g, p);
            break;
        case -2:
            if (p->getBalance() == -1) {
                // zig-zig
                rotate_right(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // zig-zag
                rotate_left(p);
                rotate_right(g);
                switch (n->getBalance()) {
                case -1:
                    p->setBalance(0);
                    g->setBalance(1);
                    break;
                case 0:
                    p->setBalance(0);
                    g->setBalance(0);
                    break;
                case 1:
                    p->setBalance(-1);
                    g->setBalance(0);
                    break;
                }
                n->setBalance(0);
            }
        }
    } else {
        // p is right child
        g->setBalance(g->getBalance() + 1);
        switch (g->getBalance()) {
        case 0:
            return;
        case 1:
            insert_fix(g, p);
            break;
        case 2:
            if (p->getBalance() == 1) {
                // zig-zig
                rotate_left(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // zig-zag
                rotate_right(p);
                rotate_left(g);
                switch (n->getBalance()) {
                case 1:
                    p->setBalance(0);
                    g->setBalance(-1);
                    break;
                case 0:
                    p->setBalance(0);
                    g->setBalance(0);
                    break;
                case -1:
                    p->setBalance(1);
                    g->setBalance(0);
                    break;
                }
                n->setBalance(0);
            }
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotate_right(AVLNode<Key, Value>* y) {
    AVLNode<Key, Value>* x = y->getLeft();
    AVLNode<Key, Value>* b = x->getRight();
    AVLNode<Key, Value>* p = y->getParent();
    if (p == nullptr) {
        this->root_ = x;
    } else {
        if (p->getLeft() == y) {
            p->setLeft(x);
        } else {
            p->setRight(x);
        }
    }
    y->setParent(x);
    y->setLeft(b);
    x->setParent(p);
    x->setRight(y);
    if (b != nullptr) {
        b->setParent(y);
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotate_left(AVLNode<Key, Value>* x) {
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* b = y->getLeft();
    AVLNode<Key, Value>* p = x->getParent();
    if (p == nullptr) {
        this->root_ = y;
    } else {
        if (p->getLeft() == x) {
            p->setLeft(y);
        } else {
            p->setRight(y);
        }
    }
    x->setParent(y);
    x->setRight(b);
    y->setParent(p);
    y->setLeft(x);
    if (b != nullptr) {
        b->setParent(x);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    AVLNode<Key, Value>* n = (AVLNode<Key, Value>*)this->internalFind(key);
    if (n == nullptr) {
        return;
    }
    if (n->getLeft() != nullptr && n->getRight() != nullptr) {
        // n has two children
        nodeSwap(n, (AVLNode<Key, Value>*)this->predecessor(n));
    }
    AVLNode<Key, Value>* p = n->getParent();
    int diff = 0;
    if (p != nullptr) {
        if (n == p->getLeft()) {
            // n is a left child
            diff = 1;
        } else {
            // n is a right child
            diff = -1;
        }
    }
    AVLNode<Key, Value>* c = n->getLeft();
    if (c == nullptr) {
        c = n->getRight();
    }

    delete n;
    // Fix pointers
    if (p == nullptr) {
        // n was root node
        this->root_ = c;
    } else {
        // n was not root node
        if (diff == 1) {
            // n was a left child
            p->setLeft(c);
        } else {
            // n was a right child
            p->setRight(c);
        }
    }
    if (c != nullptr) {
        c->setParent(p);
    }
    remove_fix(p, diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::remove_fix(AVLNode<Key, Value>* n, int diff) {
    if (n == nullptr) {
        return;
    }
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if (p != nullptr) {
        if (n == p->getLeft()) {
            // n is a left child
            ndiff = 1;
        } else {
            // n is a right child
            ndiff = -1;
        }
    }
    switch (n->getBalance() + diff) {
    case -2: {
        AVLNode<Key, Value>* c = n->getLeft();
        switch (c->getBalance()) {
        case -1: // zig-zig case
            rotate_right(n);
            n->setBalance(0);
            c->setBalance(0);
            remove_fix(p, ndiff);
            break;
        case 0: // zig-zig case
            rotate_right(n);
            n->setBalance(-1);
            c->setBalance(1);
            break;
        case 1: { // zig-zag case
            AVLNode<Key, Value>* g = c->getRight();
            rotate_left(c);
            rotate_right(n);
            switch (g->getBalance()) {
            case 1:
                n->setBalance(0);
                c->setBalance(-1);
                break;
            case 0:
                n->setBalance(0);
                c->setBalance(0);
                break;
            case -1:
                n->setBalance(1);
                c->setBalance(0);
                break;
            }
            g->setBalance(0);
            remove_fix(p, ndiff);
            break;
        }
        }
        break;
    }
    case 2: {
        AVLNode<Key, Value>* c = n->getRight();
        switch (c->getBalance()) {
        case 1: // zig-zig case
            rotate_left(n);
            n->setBalance(0);
            c->setBalance(0);
            remove_fix(p, ndiff);
            break;
        case 0: // zig-zig case
            rotate_left(n);
            n->setBalance(1);
            c->setBalance(-1);
            break;
        case -1: { // zig-zag case
            AVLNode<Key, Value>* g = c->getLeft();
            rotate_right(c);
            rotate_left(n);
            switch (g->getBalance()) {
            case -1:
                n->setBalance(0);
                c->setBalance(1);
                break;
            case 0:
                n->setBalance(0);
                c->setBalance(0);
                break;
            case 1:
                n->setBalance(-1);
                c->setBalance(0);
                break;
            }
            g->setBalance(0);
            remove_fix(p, ndiff);
            break;
        }
        }
        break;
    }
    case -1:
        n->setBalance(-1);
        break;
    case 1:
        n->setBalance(1);
        break;
    case 0:
        n->setBalance(0);
        remove_fix(p, ndiff);
        break;
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1,
                                   AVLNode<Key, Value>* n2) {
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
