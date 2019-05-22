#pragma once

#include "util.hpp"
#include "TreeUtil.hpp"

#include <optional>
#include <memory>


extern int modification_count;

template<class T>
class SplayTree
{
private:
    struct Node
    {
        enum class Direction { Left, Right };

        using ptr_t = std::unique_ptr<Node>;

        explicit Node(const T& value) : value(value) {}

        T value;
        ptr_t left, right;

        friend std::ostream& operator<<(std::ostream& os, const Node& node)
        {
            return os << node.value;
        }

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

        static bool splay(ptr_t& root, const T& value, Comparer<T>& cmp) noexcept
        {
            if (!root)
                return false;

            ptr_t left, right;
            ptr_t *left_max = &left, *right_min = &right;
            const auto select_tree = [&](Direction dir)->ptr_t*&
            {
                if (dir == Direction::Left)
                    return right_min;
                else
                    return left_max;
            };

            bool found = false;

            while (true)
            {
                const auto dir = direction(*root, value, cmp);
                if (!dir.has_value())
                {
                    found = true;
                    break;
                }

                auto& ch = child(*root, *dir);
                if (!ch)
                    break;

                const auto child_dir = direction(*ch, value, cmp);
                if (!child_dir.has_value())
                {
                    found = true;
                }
                // else if (!child(*ch, *child_dir))
                // {
                //     break;
                // }
                else if (*child_dir == *dir && child(*ch, *child_dir))
                {
                    /* rotate
                      A      L      B
                       \    -->    /
                        B         A
                       /    <--    \
                      C      R      C
                    */
                    auto tmp = std::move(ch);
                    auto& ch_other_ch = other_child(*tmp, *child_dir);
                    ch = std::move(ch_other_ch);
                    modification_count++;
                    ch_other_ch = std::move(root);
                    modification_count++;
                    root = std::move(tmp);
                    modification_count++;
                }

                ptr_t *&tmp_tree = select_tree(*dir);
                *tmp_tree = std::move(root);
                modification_count++;
                tmp_tree = &child(**tmp_tree, *dir);
                root = std::move(*tmp_tree);
                modification_count++;
            }

            *left_max = std::move(root->left);
            *right_min = std::move(root->right);

            root->left = std::move(left);
            modification_count++;
            root->right = std::move(right);
            modification_count++;

            return found;
        }

        static bool search(ptr_t& root, const T& value, Comparer<T>& cmp) noexcept
        {
            return splay(root, value, cmp);
        }

        static bool remove(ptr_t& root, const T& value, Comparer<T>& cmp)
        {
            if (!root || !splay(root, value, cmp))
                return false;

            if (root->left)
            {
                splay(root->left, value, cmp);
                root->left->right = std::move(root->right);
                modification_count++;
                root = std::move(root->left);
                modification_count++;
            }
            else
            {
                root = std::move(root->right);
                modification_count++;
            }

            return true;
        }

        static bool insert(ptr_t& root, const T& value, Comparer<T>& cmp)
        {
            if (!root)
            {
                root = std::make_unique<Node>(value);
                modification_count++;
                return true;
            }

            if (splay(root, value, cmp))
                return false;

            const auto dir = direction(*root, value, cmp);
            // if (!dir.has_value())
            //     return false;

            auto new_node = std::make_unique<Node>(value);
            child(*new_node, *dir) = std::move(child(*root, *dir));
            modification_count++;
            other_child(*new_node, *dir) = std::move(root);
            modification_count++;
            root = std::move(new_node);
            modification_count++;

            return true;
        }
    };

    typename Node::ptr_t root;
    mutable Comparer<T> cmp;
public:
    SplayTree() : cmp() {}
    SplayTree(Comparer<T>& cmp) : cmp(cmp) {}
    const Comparer<T>& comparer() const { return cmp; }

    bool insert(const T& value)
    {
        return Node::insert(root, value, cmp);
    }

    bool remove(const T& value)
    {
        return Node::remove(root, value, cmp);
    }

    bool search(const T& value) noexcept
    {
        return Node::search(root, value, cmp);
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

    bool check()
    {
        auto tmp = cmp;
        return TreeUtil::check(root.get(), tmp);
    }
};
