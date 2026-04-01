/**
 * @file node.hpp
 * @author Tommy G
 * @brief Header of a node object
 * @date 2026-03-25
 *
 */

#ifndef NODE_HPP
#define NODE_HPP

#include <array>

namespace ds {

    // Forward declaration of Node class template
    template <typename T, std::size_t ORDER> struct Node {
        static_assert(ORDER >= 3, "B-tree ORDER must be at least 3");

        // Constants for maximum keys and children based on the order of the tree
        static constexpr std::size_t MAX_KEYS     = ORDER - 1;
        static constexpr std::size_t MAX_CHILDREN = ORDER;

        // Data members
        std::array<T, MAX_KEYS + 1> keys{};             // +1 to temporarily hold the new key during splits
        std::array<Node*, MAX_CHILDREN + 1> children{}; // +1 to temporarily hold the new child during splits
        std::size_t key_count{0};

        // Constructors
        /// @brief Construct an empty node
        Node();
        /// @brief Construct a node with one key and two children
        /// @param key The key to store in the node
        /// @param left_child The left child pointer
        /// @param right_child The right child pointer
        Node(T key, Node* left_child, Node* right_child);

        // Raw pointer ownership — copying is undefined, prevent it
        Node(const Node&)            = delete;
        Node& operator=(const Node&) = delete;

        // Move semantics for ownership transfer
        /// @brief Move-construct a node, transferring ownership of its contents
        /// @param other The node to move from
        Node(Node&& other) noexcept;
        /// @brief Move-assign a node, transferring ownership of its contents
        /// @param other The node to move from
        /// @return A reference to this node
        Node& operator=(Node&& other) noexcept;

        // Utility functions

        /// @brief Check whether the node has no children
        /// @return True if the node is a leaf, otherwise false
        bool is_leaf() const;

        /// @brief Check whether the node has the maximum number of keys
        /// @return True if the node is full, otherwise false
        bool is_full() const;

        /// @brief Check whether the node currently has too many keys
        /// @return True if the node is in an overflow state, otherwise false
        bool is_overflow() const;

        /// @brief Check whether the node currently has too few keys
        /// @return True if the node is in an underflow state, otherwise false
        bool is_underflow() const;

        /// @brief Add a key to the node in sorted order
        /// @param key The key to insert
        /// @return True if the key was inserted, false if it already exists
        bool add_key(const T& key);

        /// @brief Remove a key from the node
        /// @param key The key to remove
        /// @return True if the key was removed, false if it was not found
        bool remove_key(const T& key);

        /// @brief Determine which child subtree should contain a given value
        /// @param value The value to locate
        /// @return The index of the child subtree where the value belongs
        /// @note Returns the first key position where value < keys[i], or key_count if
        /// the value is greater than all keys in the node
        std::size_t child_index(const T& value) const;

        /// @brief Find the index of a value within the node's keys
        /// @param value The value to locate
        /// @return The index of the key if found, otherwise key_count
        std::size_t key_index(const T& value) const;

        /// @brief Split the node and promote the middle key
        /// @return A pair containing the promoted key and the newly created right node
        std::pair<T, Node*> split();

        /// @brief Print the node's keys and child addresses
        void print() const;
    };
} // namespace ds

// Include the implementation of the Node template class
#include "node.tpp"

#endif // NODE_HPP