// SimpleAVL.cpp : Defines the entry point for the console application.
// Source code for https://habr.com/post/150732/

#include <algorithm>
#include <cassert>
#include <random>
#include <utility>
#include <vector>

template<typename T>
struct AvlNode
{
    AvlNode* left;
    AvlNode* right;
    T data;
    int height;

    template<typename U>
    AvlNode(U&& value) 
        : data(std::forward<U>(value))
        , left(nullptr)
        , right(nullptr)
        , height(1)
    {}
    ~AvlNode()
    {
        delete left;
        delete right;
    }
};


template<typename T>
AvlNode<T>* find(AvlNode<T>* tree, const T& value)
{
    if (!tree)
        return nullptr;

    if (tree->data < value)
        return find(tree->right, value);
    if (value < tree->data)
        return find(tree->left, value);

    return tree;
}

template<typename T>
int height(AvlNode<T>* p)
{
    return p ? p->height : 0;
}

template<typename T>
int bFactor(AvlNode<T>* p)
{
    return height(p->right) - height(p->left);
}

template<typename T>
void fixHeight(AvlNode<T>* p)
{
    p->height = std::max(height(p->left), height(p->right)) + 1;
}

template<typename T>
int checkDepth(AvlNode<T>* p)
{
    if (!p)
        return 0;
    assert(height(p->left) == checkDepth(p->left));
    assert(height(p->right) == checkDepth(p->right));
    return std::max(height(p->left), height(p->right)) + 1;
}

template<typename T>
AvlNode<T>* rotateRight(AvlNode<T>* p)
{
    AvlNode<T>* result = p->left;
    p->left = result->right;
    result->right = p;
    fixHeight(p);
    fixHeight(result);
    return result;
}

template<typename T>
AvlNode<T>* rotateLeft(AvlNode<T>* p)
{
    AvlNode<T>* result = p->right;
    p->right = result->left;
    result->left = p;
    fixHeight(p);
    fixHeight(result);
    return result;
}

template<typename T>
AvlNode<T>* balance(AvlNode<T>* p)
{
    fixHeight(p);
    if (bFactor(p) == 2)
    {
        if (bFactor(p->right) < 0)
            p->right = rotateRight(p->right);
        return rotateLeft(p);
    }
    if (bFactor(p) == -2)
    {
        if (bFactor(p->left) > 0)
            p->left = rotateLeft(p->left);
        return rotateRight(p);
    }
    return p;
}

template<typename T, typename U>
AvlNode<T>* insert(AvlNode<T>* p, U&& k)
{
    if (!p) 
        return new AvlNode<T>(std::forward<U>(k));
    if (k < p->data)
        p->left = insert(p->left, std::forward<U>(k));
    else if (p->data < k)
        p->right = insert(p->right, std::forward<U>(k));
    else
        return p;

    return balance(p);
}

template<typename T>
AvlNode<T>* findmin(AvlNode<T>* p) // search for a node with a minimum key in the tree p 
{
    return p->left ? findmin(p->left) : p;
}

template<typename T>
AvlNode<T>* removemin(AvlNode<T>* p) // removing a node with a minimal key from the tree p
{
    if (p->left == nullptr)
        return p->right;
    p->left = removemin(p->left);
    return balance(p);
}

template<typename T>
AvlNode<T>* remove(AvlNode<T>* p, const T& k) // removing key k from the tree p
{
    if (!p) return nullptr;
    if (k < p->data)
        p->left = remove(p->left, k);
    else if (p->data < k)
        p->right = remove(p->right, k);
    else //  k == p->key 
    {
        auto q = p->left;
        auto r = p->right;
        p->left = nullptr;
        p->right = nullptr;
        delete p;
        if (!r) return q;
        p = findmin(r);
        p->right = removemin(r);
        p->left = q;
    }
    return balance(p);
}

//////////////////////////////////////////////////////////////////////////////

enum { NNODES = 1000 };

int main(int argc, char* argv[])
{
    std::vector<int> nodes(NNODES);

    std::default_random_engine dre(0);
    for (int i = 0; i < NNODES; ++i)
    {
        std::uniform_int_distribution<int> di(0, i);
        const int j = di(dre);
        if (i != j)
            nodes[i] = nodes[j];
        nodes[j] = i;
    }

    AvlNode<int>* tree(nullptr);

    for (int i = 0; i < NNODES; ++i)
    {
        tree = insert(tree, nodes[i]);
    }

    for (int i = 0; i < NNODES; ++i)
    {
        AvlNode<int>* node = find(tree, nodes[i]);
        assert(node);
        assert(node->data == nodes[i]);
    }

    checkDepth(tree);

    for (int i = 0; i < NNODES; ++i)
    {
        tree = remove(tree, nodes[i]);
    }

    assert(!tree);

	return 0;
}

