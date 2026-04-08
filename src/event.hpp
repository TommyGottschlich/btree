/**
 * @file event.hpp
 * @author Tommy G
 * @brief Header of Event objects
 * @date 2026-04-02
 */
#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ds {

    enum class EventType {
        // Insert events
        INSERT_SUCCESS,   ///< Value was successfully inserted
        INSERT_DUPLICATE, ///< Value already exists; insert skipped
        INSERT_SPLIT,     ///< Node was split during insertion (overflow handled)
        // Remove events
        REMOVE_SUCCESS,      ///< Value was successfully removed
        REMOVE_NOT_FOUND,    ///< Value was not present in the tree
        REMOVE_BORROW_LEFT,  ///< Underflow resolved by borrowing from left sibling
        REMOVE_BORROW_RIGHT, ///< Underflow resolved by borrowing from right sibling
        REMOVE_MERGE,        ///< Underflow resolved by merging with a sibling
        REMOVE_SHRINK,       ///< Tree height decreased after root became empty
        // Search events
        SEARCH_FOUND,     ///< Value was found during search
        SEARCH_NOT_FOUND, ///< Value was not found during search
    };

    /// @brief Base for all events; holds the event type and a JSON snapshot of the tree after the event
    struct EventBase {
        EventType type;
        std::string snapshot;
    };

    /// @brief Emitted when a value is successfully inserted into the tree
    struct InsertEvent : EventBase {
        std::string key;       ///< The inserted key
        std::vector<int> path; ///< Child indices traversed from root to insertion point

        InsertEvent(std::string key, std::vector<int> path)
            : EventBase{EventType::INSERT_SUCCESS, {}}, key{std::move(key)}, path{std::move(path)} {
        }
    };

    /// @brief Emitted when a node split occurs during insertion
    struct SplitEvent : EventBase {
        std::string key;       ///< The key being inserted that triggered the split
        std::string promoted;  ///< Key promoted to the parent
        std::string left;      ///< First key of the left node after split
        std::string right;     ///< First key of the right node after split
        std::vector<int> path; ///< Child indices traversed from root to insertion point

        SplitEvent(std::string key, std::string promoted, std::string left, std::string right, std::vector<int> path)
            : EventBase{EventType::INSERT_SPLIT, {}}, key{std::move(key)}, promoted{std::move(promoted)},
              left{std::move(left)}, right{std::move(right)}, path{path} {
        }
    };

    using Event = std::variant<EventBase, InsertEvent, SplitEvent>;

} // namespace ds

#endif // EVENT_HPP
