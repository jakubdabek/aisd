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
        friend std::ostream& operator<<(std::ostream& os, Color c)
        {
            return os << (c == Color::Red ? "R" : "B");
        }

        enum class Direction { Left, Right };

        explicit Node(const T& value) : value(value), color(Color::Red) {}

        T value;
        Color color;
        std::unique_ptr<Node> left, right;

        friend std::ostream& operator<<(std::ostream& os, const Node& node)
        {
            return os << "(" << node.value << ", " << node.color << ")";
        }

        constexpr static bool is_red(const Node *node) noexcept { return node && node->color == Color::Red; }

        constexpr static Direction invert_direction(Direction dir) noexcept
        {
            return dir == Direction::Left ? Direction::Right : Direction::Left;
        }

        // returns the direction of the comparison with node's value (empty if values are equivalent)
        static std::optional<Direction> direction(const Node& node, const T& value, Comparer<T>& cmp) noexcept
        {
            if (cmp.compare(value, node.value))
                return Direction::Left;
            if (cmp.compare(node.value, value))
                return Direction::Right;
            return std::nullopt;
        }

        static std::unique_ptr<Node>& child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static const std::unique_ptr<Node>& child(const Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static std::unique_ptr<Node>& other_child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.right;
            return node.left;
        }

        static const std::unique_ptr<Node>& other_child(const Node& node, Direction direction) noexcept
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

        static void rotate_single(std::unique_ptr<Node>& node, Direction dir) noexcept
        {
            auto save = std::move(other_child(node, dir));
            other_child(node, dir) = std::move(child(save, dir));
            child(save, dir) = std::move(save);
        }

        static bool remove(std::unique_ptr<Node>& node, const T& value, Comparer<T>& cmp)
        {
            return false;
        }

        static bool insert(std::unique_ptr<Node>& root, const T& value, Comparer<T>& cmp)
        {
            return false;
        }
    };
    std::unique_ptr<Node> root;
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
