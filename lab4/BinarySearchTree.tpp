#pragma once

#include "util.tpp"

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

        static bool search(const Node *node, const T& value, Comparer<T>& cmp)
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

        static std::unique_ptr<Node>& max_node(std::unique_ptr<Node>& node)
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

        static std::ostream& inorder(const Node *node, std::ostream& os)
        {
            if (node)
            {
                inorder(node->left.get(), os);
                os << node->value << " ";
                inorder(node->right.get(), os);
            }

            return os;
        }
    };

    std::unique_ptr<Node> root;
    Comparer<T> cmp;
public:
    BinarySearchTree() : cmp() {}
    BinarySearchTree(Comparer<T>& cmp) : cmp(cmp) {}
    const Comparer<T>& comparer() const { return cmp; }

    bool insert(const T& value)
    {
        return Node::insert(root, value, cmp);
    }

    bool remove(const T& value)
    {
        return Node::remove(root, value, cmp);
    }

    bool search(const T& value)
    {
        return Node::search(root.get(), value, cmp);
    }

    std::ostream& inorder(std::ostream& os)
    {
        return Node::inorder(root.get(), os);
    }

    std::ostream& levelorder(std::ostream& os)
    {
        std::queue<std::reference_wrapper<const std::unique_ptr<Node>>> q;
        q.emplace(root);
        int i = 2;
        bool something = false;
        while (!q.empty())
        {
            auto node = q.front();
            q.pop();
            if (node.get())
            {
                os << node.get()->value << " ";
                auto l = q.emplace(node.get()->left);
                auto r = q.emplace(node.get()->right);
                something = l.get() || r.get();
            }
            else
            {
                os << "_ ";
                q.emplace(node);
                q.emplace(node);
            }
            if (is_power_of_2(i++))
            {
                std::cout << "\n";
                if (something)
                    something = false;
                else
                    break;
            }
        }

        return os;
    }
};
