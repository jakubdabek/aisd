#pragma once

#include <iostream>
#include <queue>


namespace TreeUtil {

    template<class Node>
    std::ostream& levelorder(std::ostream& os, const Node *root) noexcept
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
                os << node->value << " ";
                auto l = q.emplace(node->left.get());
                auto r = q.emplace(node->right.get());
                something = l || r;
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
    std::ostream& inorder(const Node *node, std::ostream& os) noexcept
    {
        if (node)
        {
            inorder(node->left.get(), os);
            os << node->value << " ";
            inorder(node->right.get(), os);
        }

        return os;
    }
}