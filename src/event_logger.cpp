/**
 * @file event_logger.cpp
 * @author Tommy G
 * @brief Implementation for Event Logger
 * @date 2026-04-02
 */

#include "event_logger.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace {
    constexpr const char* ROOT_ID  = "root id";
    constexpr const auto* NODES    = "nodes";
    constexpr const auto* ID       = "id";
    constexpr const auto* KEYS     = "keys";
    constexpr const auto* CHILDREN = "children";
    constexpr const auto* LEAF     = "leaf";
} // namespace

namespace ds {

    // TODO: Collect Event info
    void EventLogger::log(Event event, const TreeSnapshot& tree_snapshot) {
        std::visit([&](auto& ev) { ev.snapshot = std::move(serialize(tree_snapshot)); }, event);
        events.push_back(std::move(event));
    }

    void EventLogger::print_events() const {
        for (const auto& e : events) {
            std::visit(
                [](const auto& ev) {
                    using T = std::decay_t<decltype(ev)>;

                    if constexpr (std::is_same_v<T, InsertEvent>) {
                        std::string path{};
                        for (int i : ev.path)
                            path += std::to_string(i) + " ";
                        std::cout << "INSERT key=" << ev.key << " PATH: " << path << "\n";
                        std::cout << "TREE: " << ev.snapshot << "\n";
                    } else if constexpr (std::is_same_v<T, EventBase>) {
                        std::cout << "EVENT type=" << static_cast<int>(ev.type) << "\n";
                    }
                },
                e);
        }
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
} // namespace ds
