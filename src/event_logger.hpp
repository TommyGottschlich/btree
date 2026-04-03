/**
 * @file event_logger.hpp
 * @author Tommy G
 * @brief Header of an Event Logger object
 * @date 2026-04-1
 */

#ifndef EVENT_LOGGER_HPP
#define EVENT_LOGGER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "event.hpp"

namespace ds {

    class EventLogger {
    public:
        /// @brief Logs an event by pushing it into the events vector
        /// @param event The event to log
        /// @param tree_snapshot The B tree snapshot from which the event arrived
        void log(Event event, const TreeSnapshot& tree_snapshot);

        void print_events() const;

    private:
        std::vector<Event> events;

        /// @brief Creates a string snapshot of the given B-tree.
        /// @param tree_snapshot The tree to serialize into a human-readable string representation.
        /// @return A textual snapshot of the tree's current structure and contents in json
        static std::string serialize(const TreeSnapshot& tree_snapshot);
    };

} // namespace ds

// #include "event_logger.tpp"

#endif // EVENT_LOGGER_HPP
