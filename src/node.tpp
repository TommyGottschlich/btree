/**
 * @file node.tpp
 * @author Tommy G
 * @brief Template implementation for Node
 * @date 2026-03-25
 */
#pragma once

#include <algorithm>
#include <iostream>

namespace ds {

    // Node implementation
    template <typename T, std::size_t ORDER>
    Node<T, ORDER>::Node() {
        keys.fill(T{}); // Initialize keys to default value
        children.fill(nullptr);
    }

    template <typename T, std::size_t ORDER>
    Node<T, ORDER>::Node(T key, Node<T, ORDER>* left_child, Node<T, ORDER>* right_child) :
        Node() {
        keys[0] = key;
        children[0] = left_child;
        children[1] = right_child;
        key_count = 1;
    }


    template <typename T, std::size_t ORDER>
    Node<T, ORDER>::Node(Node&& other) noexcept
        : keys{std::move(other.keys)}
        , children{std::move(other.children)}
        , key_count{other.key_count} {
        
        other.keys.fill(T{}); // reset moved-from keys to default
        other.children.fill(nullptr); // prevent double-free on move
        other.key_count = 0;
    }

    template <typename T, std::size_t ORDER>
    Node<T, ORDER>& Node<T, ORDER>::operator=(Node&& other) noexcept {
        if (this != &other) {
            for (auto* child : children)
                delete child;

            keys      = std::move(other.keys);
            children  = other.children;
            key_count = other.key_count;
            other.keys.fill(T{});
            other.children.fill(nullptr);
            other.key_count = 0;
        }
        return *this;
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::is_leaf() const {
         return children[0] == nullptr;
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::is_full() const {
         return key_count == MAX_KEYS;
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::is_overflow() const {
         return key_count > MAX_KEYS;
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::is_underflow() const {
        return key_count < ((MAX_KEYS + 1) / 2);
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::add_key(const T& key) {
        // protect against adding to a full node or non-leaf node
        if (is_overflow() || !is_leaf()) {
            return false;
        }

        auto end = keys.begin() + key_count;
        if(std::find(keys.begin(), end, key) != end) {
            return false; // no duplicates
        }

        std::size_t insert_pos = key_count;
        for (std::size_t i = key_count; i > 0; --i) {

            if (key < keys[i - 1]) {
                keys[i] = keys[i - 1];
                insert_pos = i - 1;
            } else {
                break; // found the correct position for insertion
            }
        }
        keys[insert_pos] = key;
        key_count++;
        return true;
    }

    template <typename T, std::size_t ORDER>
    bool Node<T, ORDER>::remove_key(const T& key) {

        std::size_t i = 0;

        // find the key position
        while (i < key_count && keys[i] != key) ++i;

        if (i == key_count) return false;

        // shift everything left
        std::copy(keys.begin() + i + 1, keys.begin() + key_count, keys.begin() + i);
        keys[key_count - 1] = T{};
        --key_count;

        return true;
    }


    template <typename T, std::size_t ORDER>
    std::pair<T, Node<T, ORDER>*> Node<T, ORDER>::split() {
        auto* right_node = new Node<T, ORDER>();
        std::size_t middle = ORDER / 2;

        // save off promoted
        auto promoted_key = keys[middle];

        // Reassign keys
        // When splitting the key count is MAX_KEYS+1 or ORDER
        // copy left node keys[mid+1..MAX_KEYS+1] to right node keys[0..]
        // e.g       ORDER 5
        //           idx  0   1    2    3  (4)   "(4)" is overflow key state
        //              [ 1 , 2 , {3} , 4 , 5 ] 
        //                         ^-- promoted key index (middle=2)
        // L node       [ 1 , 2 ,  0  , 0 , 0 ]   key count = 2,  keeps keys[0..mid]
        // R node       [ 4 , 5 ,  0  , 0 , 0 ]   key count = 2,  gets  keys[mid+1..MAX_KEYS+1]
        std::copy(keys.begin() + middle + 1, keys.end(), right_node->keys.begin());
        std::fill(keys.begin() + middle, keys.end(), T{});

        // Reassign key count
        right_node->key_count = ORDER - (middle + 1);
        key_count = middle;

        // Reassign children
        // When splitting, a node with ORDER keys has MAX_CHILDREN+1 children
        // copy left node children[mid+1..MAX_CHILDREN+1] to right node children[0..]
        // e.g       ORDER 5
        //           idx  0    1    2    3    4   (5)   "(5)" is overflow child state
        //              [ c0 , c1 , c2 , c3 , c4 , c5 ]
        //                          ^-- (mid=2)
        // L node       [ c0 , c1 , c2 , 0  , 0  , 0  ]  keeps children[0..mid]
        // R node       [ c3 , c4 , c5 , 0  , 0  , 0  ]  gets  children[mid+1..MAX_CHILDREN+1]
        if (!is_leaf()) {
            std::copy(children.begin() + middle + 1, children.end(), right_node->children.begin());
            std::fill(children.begin() + middle + 1, children.end(), nullptr);
        }

        return {promoted_key, right_node};
    }

    template <typename T, std::size_t ORDER>
    std::size_t Node<T, ORDER>::child_index(const T& value) const {
        std::size_t index = key_count;
        for(std::size_t i = 0; i < key_count; ++i){
            if (value < keys[i]) {
                index = i;
                break;
            }
        }
        return index;
    }
    
    template <typename T, std::size_t ORDER>
    std::size_t Node<T, ORDER>::key_index(const T& value) const {
        auto key_idx = key_count;
        for(std::size_t i = 0; i < key_count; ++i){
            if (value == keys[i] ){
                key_idx = i;
                break;
            }
        }
        return key_idx;
    }

    template <typename T, std::size_t ORDER>
    void Node<T, ORDER>::print() const {
        std::cout << "keys: [";
        for (std::size_t i = 0; i < key_count; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << "]  children: [";
        for (std::size_t i = 0; i <= key_count; ++i) {
            if (i > 0) std::cout << ", ";
            if (children[i]) {
                std::cout << "[" << i << "]=";
                for (std::size_t j = 0; j < children[i]->key_count; ++j) {
                    if (j > 0) std::cout << ", ";
                    std::cout << children[i]->keys[j];
                }
            } else {
                std::cout << "null";
            }
        }
        std::cout << "]\n";
    }

} // namespace ds