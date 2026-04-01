/**
 * @file btree.tpp
 * @author Tommy G
 * @brief Template implementation for Btree
 * @date 2026-03-25
 */
#pragma once

#include <queue>
#include <algorithm>
#include <functional>
#include <deque>

namespace ds {

    /// Public functions implementation ///
    template <typename T, std::size_t ORDER>
    Btree<T, ORDER>::Btree() : root{nullptr}, current_size{0} {
    }

    template <typename T, std::size_t ORDER>
    Btree<T, ORDER>::~Btree() {
        clear();
    }

    template <typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::contains(const T& value) const {
        return search(root, value) != nullptr;
    }

    template <typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::insert(const T& value, bool recursive) {
        auto old_size = current_size;

        if (recursive) (void)recursive_insert(root,value);
        else (void)iterative_insert(root, value);

        return current_size > old_size;
    }

    template <typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::remove(const T& value) {
        if(!remove(root, value)) return false;
       
        // root lost key from merge
        if(root->key_count == 0 && !root->is_leaf()) {
            auto* old_root = root;
            root = root->children[0];
            old_root->children[0] = nullptr;
            delete old_root;
        }
        current_size--;

        return true;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::print() const {
        print(root);
    }

    template <typename T, std::size_t ORDER>
    int Btree<T, ORDER>::height() const {
        return height(root);
    }

    template <typename T, std::size_t ORDER>
    int Btree<T, ORDER>::size() const {
        return current_size;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::clear(const bool recursive) {
        if (recursive) recursive_clear(root);
        else clear(root);
        root = nullptr;
    }

    template<typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::validate() const {
        if (!root || root->key_count == 0) return true;
        int leaf_depth = -1;
        return validate_node(root, 0, leaf_depth, nullptr, nullptr);
    }

    template <typename T, std::size_t ORDER>
    std::string Btree<T, ORDER>::to_string() const {
        std::string result;
        if (!root) return result;

        std::queue<Node<T, ORDER>*> current_level, next_level;
        current_level.push(root);

        while (!current_level.empty()) {
            while (!current_level.empty()) {
                auto* node = current_level.front();
                current_level.pop();

                // print this node's keys
                result += "[";
                for (std::size_t i = 0; i < node->key_count; ++i) {
                    if (i > 0) result += "|";
                    result += std::to_string(node->keys[i]);
                }
                result += "] ";

                // enqueue children for next level
                for (std::size_t i = 0; i <= node->key_count; ++i) {
                    if (node->children[i])
                        next_level.push(node->children[i]);
                }
            }
            result += "\n";
            std::swap(current_level, next_level);
        }
        return result;
    }

    //region Private helper functions
    template <typename T, std::size_t ORDER>
    Node<T, ORDER>* Btree<T, ORDER>::search(Node<T,ORDER>* node, const T& value) const {
        // base case: if node is null, return nullptr
        if (!node) {
            return nullptr;
        }

        // if node is leaf (no children), check if value is in keys
        if (node->is_leaf()) {
            for(std::size_t i = 0; i < node->key_count; ++i) {
                if(node->keys[i] == value) {
                    return node;
                }
            }
            return nullptr;
        }

        // if node is not leaf (has children), check children
        // left to right for the correct child to traverse
        for (std::size_t i = 0; i < node->key_count; ++i) {
            if (value == node->keys[i]) {
                return node;
            } 
            if (value < node->keys[i]) {
                return search(node->children[i], value);
            }
        }

        // if value is greater than all keys in the node, traverse the rightmost child
        return search(node->children[node->key_count], value);
    }

    template <typename T, std::size_t ORDER>
    std::pair<T,Node<T, ORDER>*> Btree<T, ORDER>::recursive_insert(Node<T, ORDER>* node, const T& value) {
        // case 0: if root is null, create a root
         if (!node) {
            root = new Node<T, ORDER>(value, nullptr, nullptr);
            current_size++;
            return {value, root};
        }

        // case 1: if node is a leaf
        if (node->is_leaf()) {
            if (node->add_key(value))
                current_size++; // increment size of tree when a new key is successfully inserted

        } else {// case 2: if node is not a leaf
            // find child node index to insert down into
            auto index = node->child_index(value);
            auto [promoted_key, new_node] = recursive_insert(node->children[index], value);

            if (new_node)
                absorb_promoted(node, promoted_key, new_node);
        }

        // case 3: if overflow
        if(node->is_overflow()) {
            return handle_overflow(node);
        }

        return {value, nullptr}; // base
    }

    template <typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::iterative_insert(Node<T, ORDER>* node, const T& value) {
        // case 0: if root is null create root
        if(!node) {
            root = new Node<T, ORDER>(value, nullptr, nullptr);
            current_size++;
            return true;
        }

        // case 1: if node not a leaf
        std::deque<Node<T,ORDER>*> visited_stack;
        while(!node->is_leaf()) {
            visited_stack.push_front(node);
            auto index = node->child_index(value);
            node = node->children[index];
        }

        // case 2: Node is now a leaf
        if(!node->add_key(value))
            return false;

        current_size++;

        // case 3: if overflow
        while(node->is_overflow()) {
            auto [promoted, new_right] = handle_overflow(node);
            if(visited_stack.empty()) break;

            auto* parent = visited_stack.front();
            visited_stack.pop_front();

            absorb_promoted(parent, promoted, new_right);
            node = parent;
        }
        return true;
    }

    template <typename T, std::size_t ORDER>
    std::pair<T,Node<T, ORDER>*> Btree<T, ORDER>::handle_overflow(Node<T, ORDER>* node) {
        // node is now overfull, split it and promote the middle key
        auto [promoted_key, new_right_node] = node->split();
        
        // if root split, we need to create a new root
        if (node == root) {
            root = new Node<T, ORDER>(promoted_key, node, new_right_node);
            return {promoted_key, root};
        }

        // return the promoted key and new right node to be inserted into the parent
        return {promoted_key, new_right_node}; 
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::absorb_promoted(Node<T, ORDER>* parent, const T& promoted_key, Node<T, ORDER>* new_right_node) {
       
        // find insert position for promoted key
        auto insert_pos = parent->key_count;
        for (std::size_t i = parent->key_count; i > 0; --i) {
            if (promoted_key < parent->keys[i - 1]){
                insert_pos = i - 1;
            }
        }

        // shift keys right of insert_pos & children right of insert_pos + 1 to make room
        // keys:     [ k0, k1, 0  ] -> [ k0, k0, k1 ]
        // children: [ c0, c1, c2, nil ] -> [ c0, c1, c1, c2 ]
        std::copy_backward(parent->keys.begin() + insert_pos, parent->keys.end() - 1, parent->keys.end());
        std::copy_backward(parent->children.begin() + insert_pos + 1, parent->children.end() - 1, parent->children.end());

        // place promoted key and its new right child
        // keys:     [ promoted, k0, k1 ]
        // children: [ c0, new_right, c1, c2 ]
        parent->keys[insert_pos] = promoted_key;
        parent->children[insert_pos + 1] = new_right_node;
        ++parent->key_count;
    }

    template <typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::remove(Node<T, ORDER>* node, const T& value) {
        if(!node) return false;

        // case 1: node has value and is a leaf
        if(node->is_leaf()) {
            return node->remove_key(value);
        }

        auto key_idx = node->key_index(value);
        std::size_t child_idx{};

        // case 2: node has value and is not a leaf
        if (key_idx < node->key_count) {
            // find the very next greatest value in the tree
            child_idx = key_idx + 1;
            auto right_child = node->children[child_idx];
            T successor = inorder_successor(right_child);
            node->keys[key_idx] = successor;

            remove(right_child, successor);

        } else { // case 3: node value not found, search children

            child_idx = node->child_index(value);
            auto* child = node->children[child_idx];

            if (!remove(child, value))
                return false;
        }
        
        auto* child = node->children[child_idx];
        if(child->is_underflow())
            fix_underflow(node, child_idx);
    
        return true;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::merge(Node<T, ORDER>* parent, std::size_t child_index) {
        //region
        // tree structure - child index= 1
        //Before:    k0  k1   k2
        //         [ 5 | 10 | 15 ]
        //        /     |    |    \
        //   [2|3]   [ ]  [12|13] [17|18]
        //    c0      c1     c2      c3
        //
        //After:     k0  k1   k2
        //         [ 5 | 15 | 0 ]
        //        /     |    |   
        //   [2|3] [10|12|13] [17|18] 
        //    c0      c1       c3      
        //endregion
        auto* merge_node = parent->children[child_index];
        auto* delete_node = parent->children[child_index + 1];

        // step 1: set merge_node's next key to the parent's separator key
        merge_node->keys[merge_node->key_count] = parent->keys[child_index];
        ++merge_node->key_count;

        // step 2: shift parent keys left
        for (std::size_t i = child_index; i < parent->key_count - 1; ++i)
            parent->keys[i] = parent->keys[i + 1];
        parent->keys[parent->key_count - 1] = T{};
        --parent->key_count;

        // step 3: shift parent children left
        for (std::size_t i = child_index + 1; i < parent->key_count + 1; ++i)
            parent->children[i] = parent->children[i + 1];
        parent->children[parent->key_count + 1] = nullptr;
     
        // step 4: if delete_node has children
        if(!delete_node->is_leaf()) {
            // copy delete_node's children into merge_node
            std::copy(delete_node->children.begin(), delete_node->children.begin() + delete_node->key_count + 1, merge_node->children.begin() + merge_node->key_count); // assume merge node has orphan, (merge_node->key_count)
        }

        // step 5: copy delete_node's keys into merge_node's keys
        std::copy(delete_node->keys.begin(), delete_node->keys.begin() + delete_node->key_count, merge_node->keys.begin()+ merge_node->key_count);
        // compute new key count
        merge_node->key_count += delete_node->key_count;

        delete delete_node;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::borrow_from_left(Node<T, ORDER>* parent, std::size_t child_index) {
        //region
        // tree structure - child index = 1
        // before:
        //          k0 k1 k2           key indexes
        //          [4 |5| 8]
        //        /   |   |   \
        // [1|2|3] [_|_]  [6]  [9|10]
        //   c0     c1     c2    c3    child indexes
        //
        // after:
        //          k0 k1 k2          key indexes
        //          [3 |5| 8]
        //        /   |   |   \
        // [1|2|_] [4|_]  [6]  [9|10]
        //   c0     c1    c2    c3    child indexes
        //endregion

        //implement key rotation from left child
        auto* underfull_child = parent->children[child_index];
        auto* left_child = parent->children[child_index -1];

        // step 1: shift underfull_child keys to the right
        for (std::size_t i = underfull_child->key_count; i > 0; --i)
            underfull_child->keys[i] = underfull_child->keys[i - 1];

        // step 2: set underfull_child's key to parent key
        underfull_child->keys[0] = parent->keys[child_index -1];
        ++underfull_child->key_count;

        // step 3: set parent's key to left_child's key
        parent->keys[child_index - 1] = left_child->keys[left_child->key_count - 1];
        left_child->keys[left_child->key_count - 1] = T{};

        // step 4: if left child has children, shift children
        if(!left_child->is_leaf()) {
            // shift children right
            for (std::size_t i = underfull_child->key_count; i > 0; --i)
                underfull_child->children[i] = underfull_child->children[i - 1];

            // set underfull_child's first child to left_child's rightmost child
            underfull_child->children[0] = left_child->children[left_child->key_count];
            left_child->children[left_child->key_count] = nullptr;
        }
        // update key count after children moved
        --left_child->key_count;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::borrow_from_right(Node<T, ORDER>* parent, std::size_t child_index) {
        //region
        // tree structure - child index = 1
        // before:
        //         k0 k1           key indexes
        //        [ 2|5 ]
        //       /   |   \
        //   [ 1 ] [ _ ] [ 6|8 ]
        //    c0    c1     c2      child indexes
        //
        // after:
        //         k0 k1           key indexes
        //        [ 2|6 ]
        //       /   |   \
        //  [ 1 ]  [ 5 ] [ 8 ]
        //    c0    c1     c2      child indexes
        //endregion

        //implement key rotation from right child
        auto* underfull_child         = parent->children[child_index];
        auto* right_child = parent->children[child_index + 1];

        // step 1: set underfull_child's key to parent's key
        underfull_child->keys[underfull_child->key_count] = parent->keys[child_index];
        ++underfull_child->key_count;

        // step 2: set parent's key to right_child's key
        parent->keys[child_index] = right_child->keys[0];

        // step 3: shift right_child's keys to the left
        for (std::size_t i = 0; i < right_child->key_count - 1; ++i)
            right_child->keys[i] = right_child->keys[i + 1];
        right_child->keys[right_child->key_count - 1] = T{};

        // step 3: if right_child has children, shift children
        if (!right_child->is_leaf()) {
            // set underfull_child's rightmost child to right_child's leftmost child
            underfull_child->children[underfull_child->key_count] = right_child->children[0];

            // shift right_child's children left
            for (std::size_t i = 0; i < right_child->key_count; ++i)
                right_child->children[i] = right_child->children[i + 1];
            right_child->children[right_child->key_count] = nullptr;
        }
        // update key count after children moved
        --right_child->key_count;
    }

    template <typename T, std::size_t ORDER>
    T Btree<T, ORDER>::inorder_successor(Node<T, ORDER>* node) const {
        // leftmost key in right subtree
        while(!node->is_leaf()) {
            node = node->children[0];
        }
        return node->keys[0];
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::fix_underflow(Node<T, ORDER>* parent, std::size_t child_index) {
        auto* left_sibling  = child_index > 0 ? parent->children[child_index - 1] : nullptr;
        auto* right_sibling = child_index < parent->key_count ? parent->children[child_index + 1] : nullptr;

        auto minimum_keys = (Node<T, ORDER>::MAX_KEYS + 1) / 2;  // matches is_underflow
        if (right_sibling && right_sibling->key_count > minimum_keys) {
            borrow_from_right(parent, child_index);
        } else if (left_sibling && left_sibling->key_count > minimum_keys) {
            borrow_from_left(parent, child_index);
        } else if(right_sibling) {
            merge(parent, child_index); // merge child w/ right
        } else {
            merge(parent, child_index - 1); // merge left w/ child
        }
    }


    template <typename T, std::size_t ORDER>
    int Btree<T, ORDER>::height(Node<T, ORDER>* root) const {
        if (!root) return 0;
        
        // only need to traverse down the leftmost path to get the height of a b tree,
        // since all leaves are at the same level because of the properties of a balanced tree
        int height = 0;
        auto* current_node = root;
        while (!current_node->is_leaf()) {
            current_node = current_node->children[0];
            height++;
        }
        return 1 + height; // add 1 to account for the leaf level
    }

    template <typename T, std::size_t ORDER>
    int Btree<T, ORDER>::size(Node<T, ORDER>* root) const {
        if (!root) return 0;

        int total = 0;
        std::queue<Node<T, ORDER>*> queue;
        queue.push(root);

        while (!queue.empty()) {
            auto* current_node = queue.front();
            total += current_node->key_count; // add keys from current node

            queue.pop();

            for(auto* child : current_node->children) {
                if(child) queue.push(child);
            }
        } 
        return total;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::clear(Node<T, ORDER>* node) {
        if (!node) return;

        std::queue<Node<T, ORDER>*> queue;
        queue.push(node);

        while(!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            for(auto* child : current->children) {
                if(child) queue.push(child);
            }
            delete current;
        }
    }

    template<typename T, std::size_t ORDER>
    void Btree<T, ORDER>::recursive_clear(Node<T, ORDER>* node) {
        if (!node) return;

        for (auto* child : node->children) {
            recursive_clear(child);
        }
        delete node;
    }

    template <typename T, std::size_t ORDER>
    void Btree<T, ORDER>::print(Node<T, ORDER>* node) const {
        if (!node) return;

        auto print_node = [](const Node<T, ORDER>* n) {
            std::cout << "[";
            for (std::size_t i = 0; i < n->key_count; ++i) {
                if (i > 0) std::cout << "|";
                std::cout << n->keys[i];
            }
            std::cout << "]";
        };

        std::function<void(Node<T, ORDER>*, const std::string&, bool)> recurse =
            [&](Node<T, ORDER>* n, const std::string& prefix, bool is_last) {
                std::cout << prefix << (is_last ? "└── " : "├── ");
                print_node(n);
                std::cout << "\n";

                std::string child_prefix = prefix + (is_last ? "    " : "│   ");
                for (std::size_t i = 0; i <= n->key_count; ++i) {
                    if (n->children[i])
                        recurse(n->children[i], child_prefix, i == n->key_count);
                }
            };

        print_node(root);
        std::cout << "\n";
        for (std::size_t i = 0; i <= root->key_count; ++i) {
            if (root->children[i])
                recurse(root->children[i], "", i == root->key_count);
        }
    }

    template<typename T, std::size_t ORDER>
    bool Btree<T, ORDER>::validate_node(Node<T, ORDER>* node, int depth, int& leaf_depth, const T* lower, const T* upper) const {
        // key count bounds
        bool is_root  = (node == root);
        std::size_t min_keys = is_root ? 1 : (ORDER / 2);

        if (node->key_count < min_keys && current_size > 1) return false;
        if (node->key_count > Node<T, ORDER>::MAX_KEYS)     return false;

        // keys are sorted and within the inherited (lower, upper) bounds
        for (std::size_t i = 0; i < node->key_count; ++i) {
            if (lower && node->keys[i] <= *lower) return false;
            if (upper && node->keys[i] >= *upper) return false;
            if (i + 1 < node->key_count && node->keys[i] >= node->keys[i + 1]) return false;
        }

        // leaf depth consistency
        if (node->is_leaf()) {
            if (leaf_depth == -1) leaf_depth = depth;
            else if (depth != leaf_depth) return false;
            return true;
        }

        // recurse into children, tightening the key bounds at each step
        for (std::size_t i = 0; i <= node->key_count; ++i) {
            if (node->children[i] == nullptr) return false;

            const T* child_lower = (i == 0)               ? lower : &node->keys[i - 1];
            const T* child_upper = (i == node->key_count) ? upper : &node->keys[i];

            if (!validate_node(node->children[i], depth + 1, leaf_depth, child_lower, child_upper)) return false;
        }

        return true;
    }
    //endregion
}
