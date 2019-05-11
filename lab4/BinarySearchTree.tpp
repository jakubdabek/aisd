#pragma once

#include "util.tpp"
#include "TreeUtil.hpp"

#include <memory>
#include <utility>
#include <queue>


template<class T>
class BinarySearchTree
{
private:
    struct Node
    {
        Node(const T& value) : value(value) {}
        T value;
        std::unique_ptr<Node> left, right;

        static bool search(const Node *node, const T& value, Comparer<T>& cmp) noexcept
        {
            if (!node)
            {
                return false;
            }

            if (cmp.compare(value, node->value))
            {
                return search(node->left.get(), value, cmp);
            }
            else if (cmp.compare(node->value, value))
            {
                return search(node->right.get(), value, cmp);
            }
            else
            {
                return true;
            }
        }

        static std::unique_ptr<Node>& max_node(std::unique_ptr<Node>& node) noexcept
        {
            if (node->right)
                return max_node(node->right);
            return node;
        }

        static bool remove(std::unique_ptr<Node>& node, const T& value, Comparer<T>& cmp)
        {
            if (!node)
            {
                return false;
            }

            if (cmp.compare(value, node->value))
            {
                return remove(node->left, value, cmp);
            }
            else if (cmp.compare(node->value, value))
            {
                return remove(node->right, value, cmp);
            }
            else
            {
                if (!node->left)
                {
                    node = std::move(node->right);
                    return true;
                }
                if (!node->right)
                {
                    node = std::move(node->left);
                    return true;
                }

                // has both left and right nodes
                auto& swapped = max_node(node->left);
                using std::swap;
                swap(swapped->value, node->value);
                remove(swapped, value, cmp);

                return true;
            }
        }

        static bool insert(std::unique_ptr<Node>& node, const T& value, Comparer<T>& cmp)
        {
            if (!node)
            {
                node = std::make_unique<Node>(value);
                return true;
            }

            if (cmp.compare(value, node->value))
            {
                return insert(node->left, value, cmp);
            }
            else if (cmp.compare(node->value, value))
            {
                return insert(node->right, value, cmp);
            }
            else
            {
                return false;
            }
        }
    };

    std::unique_ptr<Node> root;
    mutable Comparer<T> cmp;
public:
    BinarySearchTree() : cmp() {}
    BinarySearchTree(Comparer<T>& cmp) : cmp(cmp) {}
    const Comparer<T>& comparer() const { return cmp; }

    bool insert(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        return Node::insert(root, value, cmp);
    }

    bool remove(const T& value)
    {
        return Node::remove(root, value, cmp);
    }

    bool search(const T& value) const noexcept
    {
        return Node::search(root.get(), value, cmp);
    }

    std::ostream& inorder(std::ostream& os) const noexcept
    {
        return TreeUtil::inorder(root.get(), os);
    }

    std::ostream& levelorder(std::ostream& os) const noexcept
    {
        return TreeUtil::levelorder(os, root.get());
    }
};
