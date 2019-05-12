#pragma once

#include "util.hpp"

#include <optional>
#include <memory>
#include <iostream>


template<class T>
class RedBlackTree
{
private:
    struct Node
    {
        enum class Color { Red, Black };
        enum class Direction { Left, Right };

        T value;
        Color color;
        std::shared_ptr<Node> left, right;
        std::weak_ptr<Node> parent;

        // returns the direction of the comparison with node's value (empty if values are equivalent)
        static std::optional<Direction> direction(const Node& node, const T& value, Comparer<T>& cmp)
        {
            if (cmp.compare(value, node.value))
                return Direction::Left;
            if (cmp.compare(node.value, value))
                return Direction::Right;
            return std::nullopt;
        }

        static std::shared_ptr<Node>& child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static const std::shared_ptr<Node>& child(const Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static std::shared_ptr<Node>& other_child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.right;
            return node.left;
        }

        static const std::shared_ptr<Node>& other_child(const Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.right;
            return node.left;
        }

        static bool search(const Node *node, const T& value, Comparer<T>& cmp) noexcept
        {
            if (!node)
                return false;

            auto dir = direction(*node, value, cmp);

            return !dir.has_value() || search(child(*node, *dir).get(), value, cmp);
        }

        static void rotate_single(std::shared_ptr<Node>& node, Direction dir) noexcept
        {
            auto save = std::move(other_child(node, dir));
            other_child(node, dir) = std::move(child(save, dir));
            child(save, dir) = std::move(save);
        }

        static bool remove(std::shared_ptr<Node>& node, const T& value, Comparer<T>& cmp)
        {
            return false;
        }

        static bool insert(std::shared_ptr<Node>& node, const T& value, Comparer<T>& cmp)
        {
            return false;
        }
    };
    std::shared_ptr<Node> root;
    mutable Comparer<T> cmp;
public:
    RedBlackTree() : cmp() {}
    RedBlackTree(Comparer<T>& cmp) : cmp(cmp) {}
    const Comparer<T>& comparer() const { return cmp; }

    bool insert(const T& value)
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
        return TreeUtil::inorder(os, root.get());
    }

    std::ostream& levelorder(std::ostream& os) const noexcept
    {
        return TreeUtil::levelorder(os, root.get());
    }
};
