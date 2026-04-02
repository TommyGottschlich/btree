/**
 * @file event_logger.hpp
 * @author Tommy G
 * @brief Header of an Event Logger object
 * @date 2026-04-1
 */

#ifndef EVENT_LOGGER_HPP
#define EVENT_LOGGER_HPP

#include "btree.hpp"

namespace ds {

    enum class BTreeEventType {
        // Insert events
        INSERT_SUCCESS,   ///< Value was successfully inserted
        INSERT_DUPLICATE, ///< Value already exists; insert skipped
        INSERT_SPLIT,     ///< Node was split during insertion (overflow handled)
        INSERT_NEW_ROOT,  ///< A new root was created as a result of a split
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

    class EventLogger {
    public:
        // TODO: Collect Event info
        template <typename T, std::size_t ORDER> void log(const Btree<T, ORDER>& tree) {
            std::cout << snapshot(tree) << std::endl;
        }

    private:
        /// @brief Creates a string snapshot of the given B-tree.
        /// @param tree The tree to serialize into a human-readable string representation.
        /// @return A textual snapshot of the tree's current structure and contents.
        template <typename T, std::size_t ORDER> std::string snapshot(const Btree<T, ORDER>& tree) const;
    };

} // namespace ds

#include "event_logger.tpp"

#endif // EVENT_LOGGER_HPP
