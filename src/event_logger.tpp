/**
 * @file event_logger.tpp
 * @author Tommy G
 * @brief Implementation for Event Logger
 * @date 2026-04-02
 */
#pragma once

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace {
    constexpr const char* ROOT_ID  = "root id";
    constexpr const auto* NODES    = "nodes";
    constexpr const auto* ID       = "id";
    constexpr const auto* KEYS     = "keys";
    constexpr const auto* CHILDREN = "children";
    constexpr const auto* LEAF     = "leaf";
} // namespace

namespace ds {

    template <typename T, std::size_t ORDER> std::string EventLogger::snapshot(const Btree<T, ORDER>& tree) const {
        rapidjson::Document doc;
        doc.SetObject();
        auto& alloc = doc.GetAllocator();
        rapidjson::Value nodes(rapidjson::kArrayType);
        doc.AddMember("root id", 0, alloc);

        tree.bfs([&](int id, const auto& key_values, const auto& child_ids, bool is_leaf) {
            rapidjson::Value node(rapidjson::kObjectType);
            node.AddMember("id", id, alloc);

            rapidjson::Value keys(rapidjson::kArrayType);
            for (const auto& key : key_values)
                keys.PushBack(key, alloc);
            node.AddMember("keys", keys.Move(), alloc);

            rapidjson::Value children(rapidjson::kArrayType);
            for (int cid : child_ids)
                children.PushBack(cid, alloc);
            node.AddMember("children", children.Move(), alloc);

            node.AddMember("leaf", is_leaf, alloc);
            nodes.PushBack(node.Move(), alloc);
        });

        doc.AddMember("nodes", nodes.Move(), alloc);

        rapidjson::StringBuffer buf;
        rapidjson::Writer writer(buf);
        doc.Accept(writer);
        return buf.GetString();
    }
} // namespace ds
