/**
 * @file node_test.cpp
 * @brief Unit tests for Node class using Boost Test Framework
 */
#include "node.hpp"
using namespace ds;

#define BOOST_TEST_MODULE utst_node
#include <boost/test/unit_test.hpp>

// Test suite for Node functionality
BOOST_AUTO_TEST_SUITE(node_suite)

// Example test case - replace with your actual tests
BOOST_AUTO_TEST_CASE(default_node_construction, *boost::unit_test::label("node")) {
    BOOST_TEST_CONTEXT("order 3") {
        constexpr std::size_t ORDER = 3;
        const Node<int, ORDER> node;

        BOOST_CHECK(node.is_leaf());
        BOOST_CHECK(!node.is_full());

        BOOST_CHECK_EQUAL(node.key_count, 0);
        BOOST_CHECK_EQUAL(node.MAX_KEYS, ORDER - 1);
        BOOST_CHECK_EQUAL(node.MAX_CHILDREN, ORDER);
        // +1 for overflow states
        BOOST_CHECK_EQUAL(node.keys.size(), node.MAX_KEYS + 1);
        BOOST_CHECK_EQUAL(node.children.size(), node.MAX_CHILDREN + 1);
    }

    BOOST_TEST_CONTEXT("order 4") {
        constexpr std::size_t ORDER = 4;
        const Node<int, ORDER> node;

        BOOST_CHECK(node.is_leaf());
        BOOST_CHECK(!node.is_full());

        BOOST_CHECK_EQUAL(node.key_count, 0);
        BOOST_CHECK_EQUAL(node.MAX_KEYS, ORDER - 1);
        BOOST_CHECK_EQUAL(node.MAX_CHILDREN, ORDER);
        // +1 for overflow states
        BOOST_CHECK_EQUAL(node.keys.size(), node.MAX_KEYS + 1);
        BOOST_CHECK_EQUAL(node.children.size(), node.MAX_CHILDREN + 1);
    }

    BOOST_TEST_CONTEXT("order 5") {
        constexpr std::size_t ORDER = 5;
        const Node<int, ORDER> node;

        BOOST_CHECK(node.is_leaf());
        BOOST_CHECK(!node.is_full());

        BOOST_CHECK_EQUAL(node.key_count, 0);
        BOOST_CHECK_EQUAL(node.MAX_KEYS, ORDER - 1);
        BOOST_CHECK_EQUAL(node.MAX_CHILDREN, ORDER);
        // +1 for overflow states
        BOOST_CHECK_EQUAL(node.keys.size(), node.MAX_KEYS + 1);
        BOOST_CHECK_EQUAL(node.children.size(), node.MAX_CHILDREN + 1);
    }
}

BOOST_AUTO_TEST_CASE(node_construction, *boost::unit_test::label("node")) {

    BOOST_TEST_CONTEXT("order 3") {
        constexpr std::size_t ORDER = 3;
        Node<int, ORDER> left;
        left.add_key(1);
        Node<int, ORDER> right;
        right.add_key(10);

        const Node root(5, &left, &right);

        BOOST_CHECK(!root.is_leaf());
        BOOST_CHECK(!root.is_full());

        BOOST_CHECK_EQUAL(root.key_count, 1);
        BOOST_CHECK_EQUAL(root.MAX_KEYS, ORDER - 1);
        BOOST_CHECK_EQUAL(root.MAX_CHILDREN, ORDER);
        // +1 for overflow states
        BOOST_CHECK_EQUAL(root.keys.size(), root.MAX_KEYS + 1);
        BOOST_CHECK_EQUAL(root.children.size(), root.MAX_CHILDREN + 1);
    }

    BOOST_TEST_CONTEXT("order 5") {
        constexpr std::size_t ORDER = 5;

        Node<int, ORDER> left;
        left.add_key(1);
        Node<int, ORDER> right;
        right.add_key(10);

        const Node root(5, &left, &right);

        BOOST_CHECK(!root.is_leaf());
        BOOST_CHECK(!root.is_full());

        BOOST_CHECK_EQUAL(root.key_count, 1);
        BOOST_CHECK_EQUAL(root.MAX_KEYS, ORDER - 1);
        BOOST_CHECK_EQUAL(root.MAX_CHILDREN, ORDER);
        // +1 for overflow states
        BOOST_CHECK_EQUAL(root.keys.size(), root.MAX_KEYS + 1);
        BOOST_CHECK_EQUAL(root.children.size(), root.MAX_CHILDREN + 1);
    }
}

BOOST_AUTO_TEST_CASE(move_node, *boost::unit_test::label("node")) {
    constexpr std::size_t ORDER = 3;

    auto* left_child  = new Node<int, ORDER>();
    auto* right_child = new Node<int, ORDER>();

    Node<int, ORDER> source;
    source.keys[0]     = 5;
    source.keys[1]     = 15;
    source.key_count   = 2;
    source.children[0] = left_child;
    source.children[1] = right_child;

    Node<int, ORDER> moved(std::move(source));

    BOOST_CHECK_EQUAL(moved.key_count, 2u);
    BOOST_CHECK_EQUAL(moved.keys[0], 5);
    BOOST_CHECK_EQUAL(moved.keys[1], 15);
    BOOST_CHECK(moved.children[0] == left_child);
    BOOST_CHECK(moved.children[1] == right_child);

    BOOST_CHECK_EQUAL(source.key_count, 0u);
    BOOST_CHECK_EQUAL(source.keys[0], 0);
    BOOST_CHECK_EQUAL(source.keys[1], 0);
    BOOST_CHECK(source.children[0] == nullptr);
    BOOST_CHECK(source.children[1] == nullptr);

    auto* new_left_child  = new Node<int, ORDER>();
    auto* new_right_child = new Node<int, ORDER>();

    Node<int, ORDER> target;
    target.keys[0]     = 99;
    target.key_count   = 1;
    target.children[0] = new_left_child;
    target.children[1] = new_right_child;

    target = std::move(moved);

    BOOST_CHECK_EQUAL(target.key_count, 2u);
    BOOST_CHECK_EQUAL(target.keys[0], 5);
    BOOST_CHECK_EQUAL(target.keys[1], 15);
    BOOST_CHECK(target.children[0] == left_child);
    BOOST_CHECK(target.children[1] == right_child);

    BOOST_CHECK_EQUAL(moved.key_count, 0u);
    BOOST_CHECK_EQUAL(moved.keys[0], 0);
    BOOST_CHECK_EQUAL(moved.keys[1], 0);
    BOOST_CHECK(moved.children[0] == nullptr);
    BOOST_CHECK(moved.children[1] == nullptr);

    delete target.children[0];
    delete target.children[1];
}

BOOST_AUTO_TEST_CASE(add_key, *boost::unit_test::label("node")) {
    BOOST_TEST_CONTEXT("order 3") {
        constexpr std::size_t ORDER = 3;
        Node<int, ORDER> node;

        BOOST_CHECK(node.add_key(10));
        BOOST_CHECK_EQUAL(node.key_count, 1u);
        BOOST_CHECK_EQUAL(node.keys[0], 10);

        BOOST_CHECK(node.add_key(5));
        BOOST_CHECK_EQUAL(node.key_count, 2u);
        BOOST_CHECK_EQUAL(node.keys[0], 5);
        BOOST_CHECK_EQUAL(node.keys[1], 10);

        BOOST_CHECK(!node.add_key(10));
        BOOST_CHECK_EQUAL(node.key_count, 2u);
        BOOST_CHECK_EQUAL(node.keys[0], 5);
        BOOST_CHECK_EQUAL(node.keys[1], 10);
    }

    BOOST_TEST_CONTEXT("order 5") {
        constexpr std::size_t ORDER = 5;
        Node<int, ORDER> node;

        BOOST_CHECK(node.add_key(40));
        BOOST_CHECK(node.add_key(10));
        BOOST_CHECK(node.add_key(30));
        BOOST_CHECK(node.add_key(20));
        BOOST_CHECK(node.add_key(50));

        BOOST_CHECK_EQUAL(node.key_count, 5u);
        BOOST_CHECK_EQUAL(node.keys[0], 10);
        BOOST_CHECK_EQUAL(node.keys[1], 20);
        BOOST_CHECK_EQUAL(node.keys[2], 30);
        BOOST_CHECK_EQUAL(node.keys[3], 40);
        BOOST_CHECK_EQUAL(node.keys[4], 50);

        BOOST_CHECK(!node.add_key(30));
        BOOST_CHECK_EQUAL(node.key_count, 5u);
        BOOST_CHECK_EQUAL(node.keys[0], 10);
        BOOST_CHECK_EQUAL(node.keys[1], 20);
        BOOST_CHECK_EQUAL(node.keys[2], 30);
        BOOST_CHECK_EQUAL(node.keys[3], 40);
        BOOST_CHECK_EQUAL(node.keys[4], 50);
    }
}

BOOST_AUTO_TEST_CASE(node_fullness, *boost::unit_test::label("node")) {
    Node<int, 5> node;
    node.key_count = Node<int, 5>::MAX_KEYS; // Simulate a full node
    BOOST_CHECK(node.is_full());
}

BOOST_AUTO_TEST_CASE(is_leaf, *boost::unit_test::label("node")) {
    Node<int, 3> node;
    BOOST_CHECK(node.is_leaf());

    node.children[0] = new Node<int, 3>(); // Add a child to make it non-leaf
    BOOST_CHECK(!node.is_leaf());

    // Clean up allocated child node
    delete node.children[0];
    node.children[0] = nullptr; // Reset pointer after deletion
}

BOOST_AUTO_TEST_CASE(node_size_constants, *boost::unit_test::label("node")) {
    Node<int, 4> node;
    BOOST_CHECK(node.MAX_KEYS == 3);
    BOOST_CHECK(node.MAX_CHILDREN == 4);

    Node<int, 5> another_node;
    BOOST_CHECK(another_node.MAX_KEYS == 4);
    BOOST_CHECK(another_node.MAX_CHILDREN == 5);

    Node<int, 10> large_node;
    BOOST_CHECK(large_node.MAX_KEYS == 9);
    BOOST_CHECK(large_node.MAX_CHILDREN == 10);
}

BOOST_AUTO_TEST_CASE(node_empty_state, *boost::unit_test::label("node")) {
    Node<int, 3> node;
    BOOST_CHECK(node.key_count == 0);
    for (std::size_t i = 0; i < ds::Node<int, 3>::MAX_KEYS; ++i) {
        BOOST_CHECK(node.keys[i] == 0); // Default value for int
    }
    for (std::size_t i = 0; i < ds::Node<int, 3>::MAX_CHILDREN; ++i) {
        BOOST_CHECK(node.children[i] == nullptr);
    }
}

BOOST_AUTO_TEST_CASE(node_string_type, *boost::unit_test::label("node")) {
    Node<std::string, 3> node;
    node.keys[0]   = "Hello";
    node.keys[1]   = "World";
    node.key_count = 2;

    BOOST_CHECK(node.keys[0] == "Hello");
    BOOST_CHECK(node.keys[1] == "World");
    BOOST_CHECK(node.is_full());
}

BOOST_AUTO_TEST_CASE(node_overflow_state, *boost::unit_test::label("node")) {
    Node<int, 3> node;
    node.keys[0]   = 10;
    node.keys[1]   = 20;
    node.keys[2]   = 30; // This key exceeds the MAX_KEYS for a node of order 3
    node.key_count = 3;  // Simulate an overflow state

    BOOST_CHECK(node.is_overflow());
}

BOOST_AUTO_TEST_CASE(node_underflow_state, *boost::unit_test::label("node")) {
    BOOST_TEST_CONTEXT("order 3") {
        constexpr std::size_t ORDER = 3;
        Node<int, ORDER> node;
        BOOST_CHECK(node.is_underflow());

        node.add_key(10);
        BOOST_CHECK(!node.is_underflow());
        node.add_key(20);
        BOOST_CHECK(!node.is_underflow());
    }

    BOOST_TEST_CONTEXT("order 4") {
        constexpr std::size_t ORDER = 4;
        Node<int, ORDER> node;
        BOOST_CHECK(node.is_underflow());

        node.add_key(10);
        BOOST_CHECK(node.is_underflow());
        node.add_key(20);
        BOOST_CHECK(!node.is_underflow());
        node.add_key(30);
        BOOST_CHECK(!node.is_underflow());
    }

    BOOST_TEST_CONTEXT("order 5") {
        constexpr std::size_t ORDER = 5;
        Node<int, ORDER> node;
        BOOST_CHECK(node.is_underflow());

        node.add_key(10);
        BOOST_CHECK(node.is_underflow());
        node.add_key(20);
        BOOST_CHECK(!node.is_underflow());
    }
}

BOOST_AUTO_TEST_CASE(node_add_key_spurious_overflow_slot, *boost::unit_test::label("node")) {
    // keys=[3,5], add_key(3) fails but shifts 5 into the overflow slot keys[2]
    // before detecting the duplicate at keys[0]
    Node<int, 3> node;
    node.add_key(3);
    node.add_key(5); // keys=[3,5], key_count=2

    BOOST_CHECK(!node.add_key(3)); // duplicate — should fail
    BOOST_CHECK_EQUAL(node.key_count, 2u);
    BOOST_CHECK(!node.is_overflow()); // overflow slot must not be counted

    // keys[2] may have been written during the failed shift
    BOOST_CHECK_EQUAL(node.keys[2], 0); // spurious value lives here
}

BOOST_AUTO_TEST_CASE(node_split_no_children, *boost::unit_test::label("node")) {
    // ORDER=5: mid=2, keys [1,2,3,4,5] -> promoted=3, left=[1,2], right=[4,5]
    auto* left = new Node<int, 5>();
    left->add_key(1);
    left->add_key(2);
    left->add_key(3);
    left->add_key(4);
    left->add_key(5);

    auto [promoted, right] = left->split();

    BOOST_CHECK_EQUAL(promoted, 3);

    // left keeps keys[0..mid-1], clears the rest
    BOOST_CHECK_EQUAL(left->key_count, 2u);
    BOOST_CHECK_EQUAL(left->keys[0], 1);
    BOOST_CHECK_EQUAL(left->keys[1], 2);
    BOOST_CHECK_EQUAL(left->keys[2], 0);
    BOOST_CHECK_EQUAL(left->keys[3], 0);
    BOOST_CHECK_EQUAL(left->keys[4], 0);
    BOOST_CHECK(left->is_leaf());

    // right gets keys[mid+1..end]
    BOOST_CHECK_EQUAL(right->key_count, 2u);
    BOOST_CHECK_EQUAL(right->keys[0], 4);
    BOOST_CHECK_EQUAL(right->keys[1], 5);
    BOOST_CHECK_EQUAL(right->keys[2], 0);
    BOOST_CHECK_EQUAL(right->keys[3], 0);
    BOOST_CHECK_EQUAL(right->keys[4], 0);
    BOOST_CHECK(right->is_leaf());

    delete left;
    delete right;
}

BOOST_AUTO_TEST_CASE(node_split_children, *boost::unit_test::label("node")) {
    // Build an ORDER=5 internal node in overflow state (5 keys, 6 children)
    // keys:     [ 1, 2, 3, 4, 5 ]
    // children: [ c0, c1, c2, c3, c4, c5 ]
    // mid = ORDER/2 = 2  =>  promoted = 3
    // left  keeps: keys[0..1], children[0..2]
    // right gets:  keys[3..4], children[3..5]

    auto* left = new Node<int, 5>();
    left->add_key(1);
    left->add_key(2);
    left->add_key(3);
    left->add_key(4);
    left->add_key(5);

    auto* c0          = new Node<int, 5>();
    auto* c1          = new Node<int, 5>();
    auto* c2          = new Node<int, 5>();
    auto* c3          = new Node<int, 5>();
    auto* c4          = new Node<int, 5>();
    auto* c5          = new Node<int, 5>();
    left->children[0] = c0;
    left->children[1] = c1;
    left->children[2] = c2;
    left->children[3] = c3;
    left->children[4] = c4;
    left->children[5] = c5;

    auto [promoted, right] = left->split();

    BOOST_CHECK(promoted == 3);

    // left keeps keys[0..mid-1], clears the rest
    BOOST_CHECK_EQUAL(left->key_count, 2u);
    BOOST_CHECK_EQUAL(left->keys[0], 1);
    BOOST_CHECK_EQUAL(left->keys[1], 2);
    BOOST_CHECK_EQUAL(left->keys[2], 0);
    BOOST_CHECK_EQUAL(left->keys[3], 0);
    BOOST_CHECK_EQUAL(left->keys[4], 0);
    BOOST_CHECK(!left->is_leaf());

    // right gets keys[mid+1..end]
    BOOST_CHECK_EQUAL(right->key_count, 2u);
    BOOST_CHECK_EQUAL(right->keys[0], 4);
    BOOST_CHECK_EQUAL(right->keys[1], 5);
    BOOST_CHECK_EQUAL(right->keys[2], 0);
    BOOST_CHECK_EQUAL(right->keys[3], 0);
    BOOST_CHECK_EQUAL(right->keys[4], 0);
    BOOST_CHECK(!right->is_leaf());

    // left node keeps children[0..2], clears [3..5]
    BOOST_CHECK(left->children[0] == c0);
    BOOST_CHECK(left->children[1] == c1);
    BOOST_CHECK(left->children[2] == c2);
    BOOST_CHECK(left->children[3] == nullptr);
    BOOST_CHECK(left->children[4] == nullptr);
    BOOST_CHECK(left->children[5] == nullptr);

    // right node gets children[3..5]
    BOOST_CHECK(right->children[0] == c3);
    BOOST_CHECK(right->children[1] == c4);
    BOOST_CHECK(right->children[2] == c5);
    BOOST_CHECK(right->children[3] == nullptr);
    BOOST_CHECK(right->children[4] == nullptr);
    BOOST_CHECK(right->children[5] == nullptr);

    delete left;
    delete right;
}

BOOST_AUTO_TEST_CASE(remove_key, *boost::unit_test::label("node")) {

    auto* node = new Node<int, 6>();
    node->add_key(1);
    node->add_key(2);
    node->add_key(3);
    node->add_key(4);
    node->add_key(5);
    node->add_key(7);

    // remove value that ddoes not exist
    BOOST_CHECK(!node->remove_key(6));

    // remove middle
    BOOST_CHECK(node->remove_key(4));

    BOOST_CHECK_EQUAL(node->key_count, 5);
    BOOST_CHECK_EQUAL(node->keys[0], 1);
    BOOST_CHECK_EQUAL(node->keys[1], 2);
    BOOST_CHECK_EQUAL(node->keys[2], 3);
    BOOST_CHECK_EQUAL(node->keys[3], 5);
    BOOST_CHECK_EQUAL(node->keys[4], 7);

    // remove first
    BOOST_CHECK(node->remove_key(1));

    BOOST_CHECK_EQUAL(node->key_count, 4);
    BOOST_CHECK_EQUAL(node->keys[0], 2);
    BOOST_CHECK_EQUAL(node->keys[1], 3);
    BOOST_CHECK_EQUAL(node->keys[2], 5);
    BOOST_CHECK_EQUAL(node->keys[3], 7);

    // remove last
    BOOST_CHECK(node->remove_key(7));

    BOOST_CHECK_EQUAL(node->key_count, 3);
    BOOST_CHECK_EQUAL(node->keys[0], 2);
    BOOST_CHECK_EQUAL(node->keys[1], 3);
    BOOST_CHECK_EQUAL(node->keys[2], 5);
}

BOOST_AUTO_TEST_CASE(child_index, *boost::unit_test::label("node")) {
    constexpr std::size_t ORDER = 5;
    Node<int, ORDER> node;

    node.add_key(10);
    node.add_key(20);
    node.add_key(30);

    BOOST_CHECK_EQUAL(node.child_index(5), 0u);
    BOOST_CHECK_EQUAL(node.child_index(10), 1u);
    BOOST_CHECK_EQUAL(node.child_index(15), 1u);
    BOOST_CHECK_EQUAL(node.child_index(20), 2u);
    BOOST_CHECK_EQUAL(node.child_index(35), 3u);
}

BOOST_AUTO_TEST_CASE(key_index, *boost::unit_test::label("node")) {
    constexpr std::size_t ORDER = 5;
    Node<int, ORDER> node;

    node.add_key(10);
    node.add_key(20);
    node.add_key(30);

    BOOST_CHECK_EQUAL(node.key_index(10), 0u);
    BOOST_CHECK_EQUAL(node.key_index(20), 1u);
    BOOST_CHECK_EQUAL(node.key_index(30), 2u);
    BOOST_CHECK_EQUAL(node.key_index(25), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
