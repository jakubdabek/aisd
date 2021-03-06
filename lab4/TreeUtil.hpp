#pragma once

#include <iostream>
#include <queue>


namespace TreeUtil {

    template<class Node>
    inline std::ostream& levelorder(std::ostream& os, const Node *root) noexcept
    {
        std::queue<const Node*> q;
        q.emplace(root);
        int i = 2;
        bool something = false;
        while (!q.empty())
        {
            auto node = q.front();
            q.pop();
            if (node)
            {
                os << *node << " ";
                auto l = q.emplace(node->left.get());
                auto r = q.emplace(node->right.get());
                something = something || l || r;
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

    template<class Node>
    inline std::ostream& inorder(std::ostream& os, const Node *node) noexcept
    {
        if (node)
        {
            inorder(os, node->left.get());
            os << node->value << " ";
            inorder(os, node->right.get());
        }

        return os;
    }

    template<class NodePtr>
    inline NodePtr& max_node(NodePtr& node) noexcept
    {
        if (node->right)
            return max_node(node->right);
        return node;
    }

    template<class Node, class T>
    static bool check(const Node *node, Comparer<T>& cmp)
    {
        if (!node)
            return true;

        if (!check(node->left.get(), cmp))
            return false;
        if (!check(node->right.get(), cmp))
            return false;

        bool correct = true;
        if (node->left && !cmp.compare(node->left->value, node->value))
        {
            std::cerr << "BST violation for left at " << *node << " left=" << (*node->left) << std::endl;
            correct = false;
        }
        if (node->right && !cmp.compare(node->value, node->right->value))
        {
            std::cerr << "BST violation for right at " << *node << " right=" << (*node->right) << std::endl;
            correct = false;
        }

        return correct;
    }
}
