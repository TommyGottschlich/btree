/**
 * @file event_logger.cpp
 * @author Tommy G
 * @brief Implementation for Event Logger
 * @date 2026-04-02
 */

#include "event_logger.hpp"

// Suppress deprecation warnings from RapidJSON 1.1.0
// (uses std::iterator, deprecated in C++17)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wnontrivial-memcall"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#pragma GCC diagnostic pop

namespace {
    constexpr const char* ROOT_ID  = "root id";
    constexpr const auto* NODES    = "nodes";
    constexpr const auto* ID       = "id";
    constexpr const auto* KEYS     = "keys";
    constexpr const auto* CHILDREN = "children";
    constexpr const auto* LEAF     = "leaf";

    constexpr const char* EVENT_TYPE        = "event";
    constexpr const char* INSERT_EVENT_TYPE = "insert";
    constexpr const char* SPLIT_EVENT_TYPE  = "split";
    constexpr const char* SNAPSHOT          = "snapshot";

    constexpr const char* KEY      = "key";
    constexpr const char* PROMOTED = "promoted";
    constexpr const char* LEFT     = "left";
    constexpr const char* RIGHT    = "right";
    constexpr const char* PATH     = "path";
} // namespace

namespace ds {

    // TODO: Collect Event info
    void EventLogger::log(Event event, const TreeSnapshot& tree_snapshot) {
        std::visit([&](auto& ev) { ev.snapshot = std::move(serialize(tree_snapshot)); }, event);
        events.push_back(std::move(event));
    }

    void EventLogger::print_events() {
        for (const auto& e : events) {
            std::visit(
                [](const auto& ev) {
                    using T = std::decay_t<decltype(ev)>;

                    if constexpr (std::is_same_v<T, InsertEvent>) {
                        std::cout << serialize(ev) << std::endl;
                        // std::cout << "TREE: " << ev.snapshot << "\n";
                    } else if constexpr (std::is_same_v<T, SplitEvent>) {
                        std::cout << serialize(ev) << std::endl;
                    } else if constexpr (std::is_same_v<T, EventBase>) {
                        std::cout << "EVENT type=" << static_cast<int>(ev.type) << "\n";
                    }
                },
                e);
        }
        events.clear();
    }

    std::string EventLogger::serialize(const TreeSnapshot& tree_snapshot) {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();
        rapidjson::Value nodes(rapidjson::kArrayType);
        doc.AddMember(rapidjson::StringRef(ROOT_ID), 0, alloc);

        for (const auto& tree_node : tree_snapshot.nodes) {
            rapidjson::Value node(rapidjson::kObjectType);
            node.AddMember(rapidjson::StringRef(ID), tree_node.id, alloc);

            rapidjson::Value keys(rapidjson::kArrayType);
            for (const auto& key : tree_node.keys)
                keys.PushBack(rapidjson::Value(key.c_str(), alloc), alloc);
            node.AddMember(rapidjson::StringRef(KEYS), keys.Move(), alloc);

            rapidjson::Value children(rapidjson::kArrayType);
            for (int cid : tree_node.child_ids)
                children.PushBack(cid, alloc);
            node.AddMember(rapidjson::StringRef(CHILDREN), children.Move(), alloc);

            node.AddMember(rapidjson::StringRef(LEAF), tree_node.is_leaf, alloc);
            nodes.PushBack(node.Move(), alloc);
        }

        doc.AddMember(rapidjson::StringRef(NODES), nodes.Move(), alloc);

        rapidjson::StringBuffer buf;
        rapidjson::Writer writer(buf);
        doc.Accept(writer);
        return buf.GetString();
    }

    std::string EventLogger::serialize(const InsertEvent& insert_event) {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();

        doc.AddMember(rapidjson::StringRef(EVENT_TYPE), rapidjson::StringRef(INSERT_EVENT_TYPE), alloc);
        doc.AddMember(rapidjson::StringRef(KEY), rapidjson::Value(insert_event.key.c_str(), alloc), alloc);

        rapidjson::Value path(rapidjson::kArrayType);
        for (int index : insert_event.path)
            path.PushBack(index, alloc);
        doc.AddMember(rapidjson::StringRef(PATH), path.Move(), alloc);

        rapidjson::Document snapshot;
        snapshot.Parse(insert_event.snapshot.c_str());
        doc.AddMember(rapidjson::StringRef(SNAPSHOT), snapshot, alloc);

        rapidjson::StringBuffer buf;
        rapidjson::Writer writer(buf);
        doc.Accept(writer);
        return buf.GetString();
    }

    std::string EventLogger::serialize(const SplitEvent& split_event) {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();

        doc.AddMember(rapidjson::StringRef(EVENT_TYPE), rapidjson::StringRef(SPLIT_EVENT_TYPE), alloc);
        doc.AddMember(rapidjson::StringRef(KEY), rapidjson::Value(split_event.key.c_str(), alloc), alloc);
        doc.AddMember(rapidjson::StringRef(PROMOTED), rapidjson::Value(split_event.promoted.c_str(), alloc), alloc);
        doc.AddMember(rapidjson::StringRef(LEFT), rapidjson::Value(split_event.left.c_str(), alloc), alloc);
        doc.AddMember(rapidjson::StringRef(RIGHT), rapidjson::Value(split_event.right.c_str(), alloc), alloc);

        rapidjson::Value path(rapidjson::kArrayType);
        for (int index : split_event.path)
            path.PushBack(index, alloc);
        doc.AddMember(rapidjson::StringRef(PATH), path.Move(), alloc);

        rapidjson::Document snapshot;
        snapshot.Parse(split_event.snapshot.c_str());
        doc.AddMember(rapidjson::StringRef(SNAPSHOT), snapshot, alloc);

        rapidjson::StringBuffer buf;
        rapidjson::Writer writer(buf);
        doc.Accept(writer);
        return buf.GetString();
    }
} // namespace ds
