/**
 * @file btree.tpp
 * @author Tommy G
 * @brief Header of a B tree object
 * @date 2026-03-25
 */

#ifndef B_TREE_HPP
#define B_TREE_HPP

#include <iostream>

#include "btree_snapshot.hpp"
#include "event_logger.hpp"
#include "node.hpp"

namespace ds {

    // B-tree class template
    template <typename T, std::size_t ORDER> class Btree {
        static_assert(ORDER >= 3, "B-tree ORDER must be at least 3");

    public:
        /// @brief Construct an empty tree
        Btree();
        /// @brief Destroy the tree and release all allocated nodes
        ~Btree();

        /// No copying — raw pointer ownership
        Btree(const Btree&)            = delete;
        Btree& operator=(const Btree&) = delete;

        /// Move semantics for ownership transfer
        Btree(Btree&&)            = default;
        Btree& operator=(Btree&&) = default;

        void set_logger(EventLogger* event_logger) {
            logger = event_logger;
        }

        /// @brief Check whether a value exists in the tree
        /// @param value The value to search for
        /// @return True if the value is found, otherwise false
        bool contains(const T& value) const;

        /// @brief Insert a value into the tree, maintaining the properties of a B tree
        /// @param value The value to be inserted
        /// @param recursive If true, use the recursive insertion path; otherwise use the iterative path
        /// @return True if the value was successfully inserted, false if the value already exists
        bool insert(const T& value, bool recursive = true);

        /// @brief Remove a value from the tree, maintaining the properties of a B tree
        /// @param value The value to be removed
        /// @return True if the value was successfully removed, false if the value was not found
        bool remove(const T& value);

        /// Utility functions

        /// @brief Print the tree structure in a readable format
        void print() const;

        /// @brief Get the height of the tree
        /// @return The height of the tree
        [[nodiscard]] int height() const;

        /// @brief Get the total number of keys in the tree
        /// @return The total number of keys in the tree
        [[nodiscard]] int size() const;

        /// @brief Clear the tree, deallocating all nodes and resetting the tree to an empty state
        void clear(bool recursive = false);

        /// @brief validate the tree state
        [[nodiscard]] bool validate() const;

        /// @brief Provide a string representation of the tree
        /// @return The string representation of the tree
        [[nodiscard]] std::string to_string() const;

        /// @brief Captures the current tree state as a breadth-first traversal
        /// @return TreeSnapshot containing all nodes, their id, keys, children, and is leaf status
        [[nodiscard]] TreeSnapshot snapshot() const;

    private:
        /// Root node of the tree
        Node<T, ORDER>* root;
        /// Cached number of keys currently stored in the tree
        std::size_t current_size;
        std::vector<int> insert_path{};

        EventLogger* logger;

        void emit(Event e) {
            if (!logger)
                return;
            logger->log(std::move(e), snapshot());
        }

        /// @brief Recursive search function that traverses the tree to find a value
        /// @param node The current node being processed
        /// @param value The value to search for
        /// @return A pointer to the node containing the value, or nullptr if the value is not found in the subtree
        /// rooted at the given node
        Node<T, ORDER>* search(Node<T, ORDER>* node, const T& value) const;

        /// Insert Functions
        /// @brief Recursive insert function that handles the logic of inserting a value into the tree
        /// @param node The current node being processed
        /// @param value The value to be inserted
        /// @return A pair containing the value and a pointer to the node where the value was inserted, or nullptr if
        /// the value already exists
        std::pair<T, Node<T, ORDER>*> recursive_insert(Node<T, ORDER>* node, const T& value);

        /// @brief Iterative insert function that handles the logic of inserting a value into the tree
        /// @param node The current node being processed
        /// @param value The value to be inserted
        /// @return True if successfully inserted into the tree, otherwise false
        bool iterative_insert(Node<T, ORDER>* node, const T& value);

        // region Insert Function Helpers
        /// @brief Splits an overfull node and promotes the middle key, creating a new root if needed
        /// @param overflow_node The overfull node to split
        /// @param path Sequence of child indices traversed from the root to the insertion point
        /// @return A pair of the promoted key and the new right node created by the split
        std::pair<T, Node<T, ORDER>*> handle_overflow(Node<T, ORDER>* overflow_node, std::vector<int>& path);

        /// @brief Inserts a promoted key and its new right child into a parent node after a split
        /// @param parent The parent node receiving the promoted key
        /// @param promoted_key The key promoted up from the child split
        /// @param new_right_node The new right node created by the child split
        void absorb_promoted(Node<T, ORDER>* parent, const T& promoted_key, Node<T, ORDER>* new_right_node);
        // endregion

        /// @brief Recursive remove function that handles the logic of removing a value from the tree
        /// @param node The current node being processed
        /// @param value The value to be removed
        /// @return True if the value was successfully removed, false if the value was not found
        bool remove(Node<T, ORDER>* node, const T& value);

        // region Remove Function helpers
        /// @brief Merge a child node with its sibling when the child node has too few keys after a removal
        /// @param parent The parent node of the child being merged
        /// @param child_index The index of the child node to be merged
        void merge(Node<T, ORDER>* parent, std::size_t child_index);

        /// @brief Borrow a key from the left sibling of a child node when the child node has too few keys after a
        /// removal
        /// @param parent The parent node of the child being borrowed from
        /// @param child_index The index of the child node that needs to borrow a key
        void borrow_from_left(Node<T, ORDER>* parent, std::size_t child_index);

        /// @brief Borrow a key from the right sibling of a child node when the child node has too few keys after a
        /// removal
        /// @param parent The parent node of the child being borrowed from
        /// @param child_index The index of the child node that needs to borrow a key
        void borrow_from_right(Node<T, ORDER>* parent, std::size_t child_index);

        /// @brief Get the in order successor of a node, which is the smallest key in the right subtree
        /// @param node The node for which to find the in order successor
        /// @return The in order successor of the given node
        T inorder_successor(Node<T, ORDER>* node) const;

        /// @brief Fix underflow at a child node by borrowing or merging
        /// @param parent The parent node of the underfull child
        /// @param child_index The index of the underfull child
        void fix_underflow(Node<T, ORDER>* parent, std::size_t child_index);
        // endregion

        /// @brief Get the height of the tree starting from a given node
        /// @param root The node from which to calculate the height
        /// @return The height of the tree
        int height(Node<T, ORDER>* root) const;

        /// @brief Get the total number of keys in the tree starting from a given node
        /// @param root The node from which to calculate the size
        /// @return The total number of keys in the tree
        int size(Node<T, ORDER>* root) const;

        /// @brief Clear the tree starting from a given node, deallocating all nodes in the process
        /// @param node The node from which to start clearing the tree
        void clear(Node<T, ORDER>* node);

        /// @brief Clear the tree recursively starting from a given node, deallocating all nodes in the process
        /// @param node The node from which to start clearing the tree
        void recursive_clear(Node<T, ORDER>* node);

        /// @brief Print the tree structure starting from a given node
        /// @param node The node from which to start printing the tree
        void print(Node<T, ORDER>* node) const;

        /// @brief Validate that the tree satisfies its structural invariants
        /// @param node The node to validate
        /// @param depth The current depth of the node
        /// @param leaf_depth The depth of the first leaf found, used to check consistency
        /// @param lower The lower bound (exclusive) for all keys in this subtree, nullptr if unbounded
        /// @param upper The upper bound (exclusive) for all keys in this subtree, nullptr if unbounded
        /// @return True if the subtree rooted at node is valid, otherwise false
        bool validate_node(Node<T, ORDER>* node, int depth, int& leaf_depth, const T* lower, const T* upper) const;

        /// @brief Converts the T value to a string
        /// @param value The value to convert to a string
        /// @return string representation of the value
        std::string value_to_string(const T& value) const;

        // region Test Function helpers
#ifdef TESTING
    public:
        /// @brief Expose the root for testing purposes
        Node<T, ORDER>* get_root() {
            return root;
        }
        /// @brief Test helper that forwards to borrow_from_left
        void test_borrow_from_left(Node<T, ORDER>* parent, std::size_t child_index) {
            borrow_from_left(parent, child_index);
        }
        /// @brief Test helper that forwards to borrow_from_right
        void test_borrow_from_right(Node<T, ORDER>* parent, std::size_t child_index) {
            borrow_from_right(parent, child_index);
        }
        /// @brief Test helper that forwards to merge
        void test_merge(Node<T, ORDER>* parent, std::size_t child_index) {
            merge(parent, child_index);
        }
#endif
        // endregion
    };
} // namespace ds

#include "btree.tpp"
#endif // B_TREE_HPP