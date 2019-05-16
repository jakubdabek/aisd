#pragma once

#include "util.hpp"
#include "TreeUtil.hpp"

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

        using ptr_t = std::unique_ptr<Node>;

        explicit Node(const T& value) : value(value), color(Color::Red) {}

        T value;
        Color color;
        ptr_t left, right;

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

        static ptr_t& child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static const ptr_t& child(const Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.left;
            return node.right;
        }

        static ptr_t& other_child(Node& node, Direction direction) noexcept
        {
            if (direction == Direction::Left)
                return node.right;
            return node.left;
        }

        static const ptr_t& other_child(const Node& node, Direction direction) noexcept
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

        static void rotate_single(ptr_t& node, Direction dir) noexcept
        {
            if (!node || !other_child(*node, dir))
                return;

            auto save = std::move(node);
            node = std::move(other_child(*save, dir));
            other_child(*save, dir) = std::move(child(*node, dir));
            child(*node, dir) = std::move(save);
        }

        static bool remove_impl_balance(ptr_t& node, Direction dir)
        {
            auto& s = other_child(*node, dir);

            if (is_red(s.get()))
            {
                node->color = Color::Red;
                rotate_single(node, dir);
                node->color = Color::Black;

                return remove_impl_balance(child(*node, dir), dir);
            }

            if (!s)
                return true;

            if (!is_red(s->left.get()) && !is_red(s->right.get()))
            {
                bool fix = true;
                if (is_red(node.get()))
                {
                    fix = false;
                }

                node->color = Color::Black;
                s->color = Color::Red;

                return fix;
            }
            else
            {
                auto save = node->color;
                if (is_red(other_child(*s, dir).get()))
                {
                    // node->color = Color::Red;
                    rotate_single(node, dir);
                    // node->color = Color::Black;
                }
                else
                {
                    s->color = Color::Red;
                    rotate_single(s, invert_direction(dir));
                    // node->color = Color::Red;
                    rotate_single(node, dir);
                    // node->color = Color::Black;
                }

                node->color = save;
                //auto& p = child(*node, dir);
                if (node->left)
                    node->left->color = Color::Black;
                if (node->right)
                    node->right->color = Color::Black;

                return false;
            }
        }

        struct remove_impl_ret { bool removed, fix; };
        static remove_impl_ret remove_impl(ptr_t& node, const T& value, Comparer<T>& cmp)
        {
            if (!node)
                return { false, false };

            auto dir = direction(*node, value, cmp);

            if (!dir.has_value())
            {
                if (!node->left || !node->right)
                {
                    auto& save = [&]()->auto& {
                        if (!node->left)
                            return node->right;
                        else
                            return node->left;
                    }();

                    auto fix = true;

                    if (is_red(node.get()))
                    {
                        fix = false;
                    }
                    else if (is_red(save.get()))
                    {
                        if (save)
                            save->color = Color::Black;
                        fix = false;
                    }

                    node = std::move(save);
                    return { true, fix };
                }
                else
                {
                    auto& swapped = TreeUtil::max_node(node->left);
                    using std::swap;
                    swap(swapped->value, node->value);
                    dir = Direction::Left;
                }
            }

            auto [removed, fix] = remove_impl(child(*node, *dir), value, cmp);

            if (fix)
            {
                fix = remove_impl_balance(node, *dir);
            }

            return { removed, fix };
        }

        static bool remove(ptr_t& root, const T& value, Comparer<T>& cmp)
        {
            auto ret = remove_impl(root, value, cmp).removed;
            if (ret && root)
                root->color = Color::Black;

            return ret;
        }

        struct insert_impl_ret { bool inserted, fix; };
        static insert_impl_ret insert_impl(ptr_t& node, const T& value, Comparer<T>& cmp)
        {
            if (!node)
            {
                node = std::make_unique<Node>(value);
                return { true, true };
            }

            const auto dir = direction(*node, value, cmp);
            if (!dir.has_value())
                return { false, false };

            auto& dir_child = child(*node, *dir);
            const auto ret = insert_impl(dir_child, value, cmp);

            if (!ret.fix)
                return ret;

            if (is_red(dir_child.get()))
            {
                auto& other_dir_child = other_child(*node, *dir);
                if (is_red(other_dir_child.get()))
                {
                    node->color = Color::Red;
                    dir_child->color = Color::Black;
                    other_dir_child->color = Color::Black;

                    return ret;
                }
                else if (!is_red(node.get()))
                {
                    if (!is_red(child(*dir_child, *dir).get()))
                    {
                        rotate_single(dir_child, *dir);
                    }

                    node->color = Color::Red;
                    rotate_single(node, invert_direction(*dir));
                    node->color = Color::Black;

                    return { ret.inserted, false };
                }
                else
                {
                    return ret;
                }
            }

            return { ret.inserted, false };
        }

        static bool insert(ptr_t& root, const T& value, Comparer<T>& cmp)
        {
            auto ret = insert_impl(root, value, cmp).inserted;
            if (ret)
                root->color = Color::Black;

            return ret;
        }
    };

    typename Node::ptr_t root;
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

    void clear() noexcept
    {
        while (root)
            remove(root->value);
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
