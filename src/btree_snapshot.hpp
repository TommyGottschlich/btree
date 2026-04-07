/**
 * @file btree_snapshot.hpp
 * @author Tommy G
 * @brief Header of B tree snapshot objects
 * @date 2026-04-06
 */

#ifndef BTREE_SNAPSHOT_HPP
#define BTREE_SNAPSHOT_HPP

#include <string>
#include <vector>

namespace ds {

    /// @brief Serializable representation of a single B-tree node
    struct NodeData {
        int id{};                        ///< Unique BFS-assigned node identifier
        std::vector<std::string> keys{}; ///< Node keys serialized as strings
        std::vector<int> child_ids{};    ///< Identifiers of child nodes in order
        bool is_leaf{};                  ///< True if the node has no children
    };

    /// @brief Serializable representation of the full B-tree state at a point in time
    struct TreeSnapshot {
        int root_id{0};                ///< Identifier of the root node
        std::vector<NodeData> nodes{}; ///< All nodes in BFS order
    };

} // namespace ds

#endif // BTREE_SNAPSHOT_HPP
