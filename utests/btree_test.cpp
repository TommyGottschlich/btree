/**
 * @file btree_test.cpp
 * @brief Unit tests for Btree class using Boost Test Framework
 */
#include "btree.hpp"

#include <random>
using namespace ds;

#define BOOST_TEST_MODULE utst_two_three_tree
#include <boost/test/unit_test.hpp>

// Test suite for btree functionality
BOOST_AUTO_TEST_SUITE(btree_suite)

namespace {
    constexpr int ORDER = 3;
}

BOOST_AUTO_TEST_CASE(search_tree_2_levels, * boost::unit_test::label("two_three_tree"))
{
    /// Test searching for existing values
    // tree structure:
    //       [2|5]
    //       / | \
    //    [1][3|4][6|8]
    Btree<int,ORDER> tree;
    for (int v : {5, 2, 6, 8, 1, 3, 4})
        tree.insert(v);

    std::string expected_string {
        "[2|5] \n"
        "[1] [3|4] [6|8] \n"
    };
    BOOST_CHECK_EQUAL( tree.to_string(), expected_string);

    BOOST_CHECK(tree.contains(1));
    BOOST_CHECK(tree.contains(4));
    BOOST_CHECK(tree.contains(3));
    BOOST_CHECK(!tree.contains(7)); // 7 is not in the tree
}

BOOST_AUTO_TEST_CASE(search_tree_3_levels, * boost::unit_test::label("two_three_tree")){
    /// Test searching for existing values
    // tree structure:
    //        [5]
    //       /   \
    //    [2]     [7]
    //    / \     / \
    //  [1][3|4] [6][8]
    Btree<int, ORDER> tree;
    for (int v : {2, 7, 5, 1, 3, 4, 6, 8})
        tree.insert(v);
        
    std::string expected_string {
        "[5] \n"
        "[2] [7] \n"
        "[1] [3|4] [6] [8] \n"
    };
    BOOST_CHECK_EQUAL( tree.to_string(), expected_string);
    BOOST_CHECK(tree.contains(1));
    BOOST_CHECK(tree.contains(4));
    BOOST_CHECK(tree.contains(3));
    BOOST_CHECK(tree.contains(8));
    BOOST_CHECK(!tree.contains(10)); // 10 is not in the tree
}


BOOST_AUTO_TEST_CASE(insert_values, * boost::unit_test::label("two_three_tree")){

    Btree<int, ORDER> tree;
    BOOST_CHECK(tree.insert(2) == true);
    BOOST_CHECK(tree.insert(8) == true);
    BOOST_CHECK(tree.insert(4) == true);
    BOOST_CHECK(tree.insert(6) == true);

    BOOST_CHECK(tree.insert(5) == true);
    auto tree_string = tree.to_string();

    std::string expected_string = "[4|6] \n[2] [5] [8] \n";
    BOOST_CHECK(tree_string == expected_string);
}

BOOST_AUTO_TEST_CASE(large_tree_inserts, * boost::unit_test::label("two_three_tree")){
    Btree<int, ORDER> tree;
    std::vector<int> values = {10, 20, 5, 15, 25, 3, 7, 12, 18, 22, 30};
    for (int value : values) {
        BOOST_CHECK(tree.insert(value) == true);
    }
    std::string expected_string {
        "[10|20] \n"
        "[5] [15] [25] \n"
        "[3] [7] [12] [18] [22] [30] \n"
    };
    BOOST_CHECK_EQUAL( tree.to_string(), expected_string);
}



BOOST_AUTO_TEST_CASE(borrow_from_right, * boost::unit_test::label("two_three_tree"))
{
    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    Btree<int, ORDER> tree;
    for (int v : {18, 54, 4, 24, 96, 12, 32, 128})
        tree.insert(v);

    // make middle child [24|32] underfull
    //
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]       [_]     [96|128]
    auto* root = tree.get_root();
    root->children[1]->key_count = 0;

    // borrow from right sibling [96|128]:
    tree.test_borrow_from_right(root, 1);
    std::string expected_tree{
        "[18|96] \n"
        "[4|12] [54] [128] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), expected_tree);
}


BOOST_AUTO_TEST_CASE(borrow_from_right_with_children, * boost::unit_test::label("two_three_tree"))
{   
    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    //  /  |  \     /  |  \      /  |  \
    // [0][8][16] [20][28][36] [90][98][136]

    // borrow from right
    Btree<int, ORDER> tree;
    for (int v : {18, 54, 4, 24, 96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136})
        tree.insert(v);

    auto* root = tree.get_root();
    root->children[0]->key_count = 0;
    auto* child0 = root->children[0];

    // Setup:
    //            [18|54]
    //       ____/   |   \____
    //    [_]     [24|32]     [96|128]
    //     |      /  |  \      /  |  \
    //  [12|16] [20][28][36] [90][98][136]

    child0->children[0]->keys[0] = 12;
    child0->children[0]->keys[1] = 16; 
    child0->children[0]->key_count = 2;
    delete child0->children[1];
    delete child0->children[2];
    child0->children[1] = nullptr;
    child0->children[2] = nullptr;

    std::string expected_underfull_tree {
        "[18|54] \n"
        "[] [24|32] [96|128] \n"
        "[12|16] [20] [28] [36] [90] [98] [136] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), expected_underfull_tree);
    // child index = 0
    // Step 0:                              Step 1: drop 18
    //             [18|54]                            [18|54] 
    //        ____/   |   \____                  ____/   |   \____  
    // C0->[_]     [24|32]     [96|128]   C0->[*18*]     [24|32]     [96|128]         
    //      |      /  |  \      /  |  \        |       /  |  \      /  |  \
    //   [12|16] [20][28][36] [90][98][136] [12|16] [20][28][36] [90][98][136]  
    //
    // Step 2: push up 24                   Step 3: shift right-sibling keys
    //           [*24*|54]                            [24|54] 
    //        ____/   |   \____                  ____/   |   \____  
    // C0->[18]    [24|32]     [96|128]   C0->[18]     [*32*]     [96|128]         
    //      |      /  |  \      /  |  \        |      /  |  \      /  |  \
    //   [12|16] [20][28][36] [90][98][136] [12|16] [20][28][36] [90][98][136]    
    //   
    // Step 4: push up 24
    //             [24|54]
    //        ____/   |   \____ 
    // C0->[18]      [32]     [96|128]
    //    |  \        |  \      /  |  \
    // [12|16][*20*] [28][36] [90][98][136]  

    tree.test_borrow_from_right(root, 0);

    std::string expected_tree {
        "[24|54] \n"
        "[18] [32] [96|128] \n"
        "[12|16] [20] [28] [36] [90] [98] [136] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), expected_tree);

}


BOOST_AUTO_TEST_CASE(borrow_from_left, * boost::unit_test::label("two_three_tree"))
{
    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    Btree<int, ORDER> tree;
    for (int v : {18, 54, 4, 24, 96, 12, 32, 128})
        tree.insert(v);

    // make middle child [24|32] underfull
    //
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]       [_]     [96|128]
    auto* root = tree.get_root();
    root->children[1]->key_count = 0;

    // borrow from left sibling [4|12]:
    tree.test_borrow_from_left(root, 1);
    std::string expected_tree{
        "[12|54] \n"
        "[4] [18] [96|128] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), expected_tree);

}


BOOST_AUTO_TEST_CASE(borrow_from_left_with_children, * boost::unit_test::label("two_three_tree"))
{   
    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    //  /  |  \     /  |  \      /  |  \
    // [0][8][16] [20][28][36] [90][98][136]

    // borrow from left
    Btree<int, ORDER> tree;
    for (int v : {18, 54, 4, 24, 96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136})
        tree.insert(v);

    auto* root = tree.get_root();
    auto* child2 = root->children[root->key_count];
    child2->key_count = 0;

    // Setup:
    //                [18|54]
    //           ____/   |   \____
    //    [4|12]     [24|32]       [_]
    //   /  |  \     /  |  \        | 
    //  [0][8][16] [20][28][36]  [128|136]

    child2->children[0]->keys[0] = 128;
    child2->children[0]->keys[1] = 136; 
    child2->children[0]->key_count = 2;
    delete child2->children[1];
    delete child2->children[2];
    child2->children[1] = nullptr;
    child2->children[2] = nullptr;

    std::string expected_underfull_tree {
        "[18|54] \n"
        "[4|12] [24|32] [] \n"
        "[0] [8] [16] [20] [28] [36] [128|136] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), expected_underfull_tree);
    // child index = 2
    // Step 0:                              Step 1: shift child2 keys right if there are keys
    //             [18|54]                               [18|54] 
    //        ____/   |   \____                     ____/   |   \____  
    //   [4|12]     [24|32]       [_]         [4|12]     [24|32]       [_|*_*]    
    //  /  |  \     /  |  \        |         /  |  \     /  |  \         |       
    // [0][8][16] [20][28][36]  [128|136]  [0][8][16] [20][28][36]   [128|136] 
    //
    // Step 2: drop in 54                   Step 3: push up left-sibling key
    //             [18|54]                               [18|*32*] 
    //        ____/   |   \____                     ____/   |   \____  
    //   [4|12]     [24|32]    [*54*|_]         [4|12]     [24|_]     [54|_]    
    //  /  |  \     /  |  \        |          /  |  \     /  |  \        |    
    // [0][8][16] [20][28][36] [128|136] [0][8][16]    [20][28][36]  [128|136]  
    //   
    // Step 4: shift c2 children right, then move c1 last child to c2[0]
    //             [18|32]
    //        ____/   |   \____ 
    //   [4|12]      [24]     [54] 
    //  /  |  \     /  |      /   \
    // [0][8][16] [20][28] *[36]* *[128|136]*

    tree.test_borrow_from_left(root, root->key_count);

    std::string expected_tree {
        "[18|32] \n"
        "[4|12] [24] [54] \n"
        "[0] [8] [16] [20] [28] [36] [128|136] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), expected_tree);

}

BOOST_AUTO_TEST_CASE(merge_without_chilren, * boost::unit_test::label("two_three_tree"))
{
    Btree<int, ORDER> tree;
    tree.insert(5);
    tree.insert(10);

    auto* root = tree.get_root();

    root->keys[2] = 15;
    root->key_count = 3;

    root->children[0] = new Node<int, 3>();
    root->children[0]->add_key(2);
    root->children[0]->add_key(3);

    root->children[1] = new Node<int, 3>();

    root->children[2] = new Node<int, 3>();
    root->children[2]->add_key(12);
    root->children[2]->add_key(13);

    root->children[3] = new Node<int, 3>();
    root->children[3]->add_key(17);
    root->children[3]->add_key(18);

    //Build:    k0  k1   k2
    //         [ 5 | 10 | 15 ]
    //        /     |    |    \
    //   [2|3]   [ ]  [12|13] [17|18]
    //    c0      c1     c2      c3
    std::string expected_unmerged_tree {
        "[5|10|15] \n"
        "[2|3] [] [12|13] [17|18] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), expected_unmerged_tree);

    tree.test_merge(root, 1);

    //After :    k0  k1   k2
    //         [ 5 | 15 | _ ]
    //        /     |       \
    //   [2|3]   [10|12|13]  [17|18]
    //    c0      c1           c2

    std::string expected_merged_tree {
        "[5|15] \n"
        "[2|3] [10|12|13] [17|18] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), expected_merged_tree);

}

BOOST_AUTO_TEST_CASE(merge_with_chilren, * boost::unit_test::label("two_three_tree"))
{
    // Build:
    //              [5|9]
    //         ____/   |   \____
    //   [2]         [7]       [12]
    //   / \          / \        / \
    // [1][3]       [6][8]    [10][14]
    Btree<int, ORDER> tree;
    for (int v : {5, 9, 2, 7, 12, 1, 3, 6, 8, 10, 14})
        tree.insert(v);

    // Make left child [2] underfull: 0 keys, 1 remaining child [1]
    // ([3] was merged away during a prior deletion)
    auto* root = tree.get_root();
    root->children[0]->key_count = 0;
    root->children[0]->children[1] = nullptr;

    // Setup:
    //              [5|9]
    //         ____/   |   \____
    //    [_]         [7]       [12]
    //     |          / \        / \
    //    [1]       [6] [8]    [10][14]
    std::string expected_unmerged_tree {
        "[5|9] \n"
        "[] [7] [12] \n"
        "[1] [6] [8] [10] [14] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), expected_unmerged_tree);

    // merge(root, 0):
    //   separator(5) pulled down into [_], right sibling [7] absorbed
    //   expected:       [9]
    //              ____/   \____
    //         [5|7]           [12]
    //         / | \            / \
    //       [1][6][8]       [10][14]
    tree.test_merge(root, 0);
    std::string expected_merged_tree {
        "[9] \n"
        "[5|7] [12] \n"
        "[1] [6] [8] [10] [14] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), expected_merged_tree);
}

BOOST_AUTO_TEST_CASE(leaf_3_node_delete, * boost::unit_test::label("two_three_tree"))
{
    //    [2|5]          delete 4        [2|5]
    //    / | \          -------->       / | \
    // [1][3|4][6|8]                  [1][3] [6|8]
    Btree<int, ORDER> tree;
    for (int v : {2, 5, 1, 3, 6, 4, 8})
        tree.insert(v);

    std::string before_delete_tree {
        "[2|5] \n" 
        "[1] [3|4] [6|8] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), before_delete_tree);

    BOOST_CHECK(tree.remove(4));
    std::string after_delete_tree {
        "[2|5] \n" 
        "[1] [3] [6|8] \n"
    };

    BOOST_CHECK_EQUAL(tree.to_string(), after_delete_tree);
}


BOOST_AUTO_TEST_CASE(remove_value, * boost::unit_test::label("two_three_tree"))
{   
    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    //  /  |  \     /  |  \      /  |  \
    // [0][8][16] [20][28][36] [90][98][136]

    // borrow from left
    Btree<int, ORDER> tree;
    std::vector<int> values{18, 54, 4, 24, 96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136};
    for (int v : values)
        tree.insert(v);

    std::string initial_tree {
        "[18|54] \n"
        "[4|12] [24|32] [96|128] \n"
        "[0] [8] [16] [20] [28] [36] [90] [98] [136] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), initial_tree);
    BOOST_CHECK_EQUAL(tree.size(), values.size());

    BOOST_CHECK(tree.remove(18));
    std::string final_tree {
        "[20|54] \n"
        "[4|12] [32] [96|128] \n"
        "[0] [8] [16] [24|28] [36] [90] [98] [136] \n"
    };
    BOOST_CHECK_EQUAL(tree.size(), values.size() -1 );
}

BOOST_AUTO_TEST_CASE(tree_construct_and_destroy, * boost::unit_test::label("two_three_tree"))
{

    // Build:
    //              [18|54]
    //         ____/   |   \____
    //   [4|12]     [24|32]     [96|128]
    //  /  |  \     /  |  \      /  |  \
    // [0][8][16] [20][28][36] [90][98][136]
    Btree<int, ORDER> tree;
    std::vector<int> values{18, 54, 4, 24, 96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136};
    std::size_t expected_size = values.size();
    for (int v : values)
        tree.insert(v);

    std::string initial_tree {
        "[18|54] \n"
        "[4|12] [24|32] [96|128] \n"
        "[0] [8] [16] [20] [28] [36] [90] [98] [136] \n"
    };
    BOOST_CHECK_EQUAL(tree.to_string(), initial_tree);
    BOOST_CHECK_EQUAL(tree.size(), expected_size);

    BOOST_TEST_CONTEXT("delete 18") {
        BOOST_ASSERT(tree.remove(18));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
        std::string delete_18_tree {
            "[20|54] \n"
         "[4|12] [32] [96|128] \n"
         "[0] [8] [16] [24|28] [36] [90] [98] [136] \n"
        };
        BOOST_CHECK_EQUAL(tree.to_string(), delete_18_tree);
    }

    BOOST_TEST_CONTEXT("delete 54") {
        BOOST_ASSERT(tree.remove(54));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 4") {
        BOOST_ASSERT(tree.remove(4));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 24") {
        BOOST_ASSERT(tree.remove(24));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 32") {
        BOOST_ASSERT(tree.remove(32));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 128") {
        BOOST_ASSERT(tree.remove(128));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 0") {
        BOOST_ASSERT(tree.remove(0));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 18 again") {
        BOOST_ASSERT(!tree.remove(18));
        BOOST_CHECK_EQUAL(tree.size(), expected_size);
    }

    BOOST_TEST_CONTEXT("delete 8") {
        BOOST_ASSERT(tree.remove(8));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 28") {
        BOOST_ASSERT(tree.remove(28));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 36") {
        BOOST_ASSERT(tree.remove(36));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 90") {
        BOOST_ASSERT(tree.remove(90));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 98") {
        BOOST_ASSERT(tree.remove(98));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 136") {
        BOOST_ASSERT(tree.remove(136));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 96") {
        BOOST_ASSERT(tree.remove(96));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 16") {
        BOOST_ASSERT(tree.remove(16));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 20") {
        BOOST_ASSERT(tree.remove(20));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_TEST_CONTEXT("delete 12") {
        BOOST_ASSERT(tree.remove(12));
        BOOST_CHECK_EQUAL(tree.size(), --expected_size);
    }

    BOOST_ASSERT(tree.size() == 0);
}

BOOST_AUTO_TEST_CASE(tree_construct_and_destroy_random, * boost::unit_test::label("two_three_tree"))
{
    // used a stress test for delete
    Btree<int, ORDER> tree;
    std::vector<int> values{18, 54, 4, 24, 96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136};
    std::size_t expected_size = values.size();
    for (int v : values)
        tree.insert(v);

    unsigned int seed = std::random_device{}();
    BOOST_TEST_MESSAGE("shuffle seed: " << seed);
    std::shuffle(values.begin(), values.end(), std::mt19937{seed});

    for (int v : values) {
        BOOST_TEST_CONTEXT("deleting " << v) {
            BOOST_ASSERT(tree.remove(v));
            BOOST_CHECK_EQUAL(tree.size(), --expected_size);
            BOOST_TEST_MESSAGE("Tree State: \n" << tree.to_string());
        }
    }

    BOOST_ASSERT(tree.size() == 0);
}

BOOST_AUTO_TEST_CASE(random_insert_delete_invariants, * boost::unit_test::label("two_three_tree"))
{
    // used a stress test for delete
    Btree<int, ORDER> tree;
    std::vector<int> values{18, 54, 4, 24};//96, 12, 32, 128, 0, 20, 90, 8, 28, 98, 16, 36, 136};

    unsigned int seed = std::random_device{}();
    BOOST_TEST_MESSAGE("seed: " << seed);
    std::mt19937 rng{seed};

    std::shuffle(values.begin(), values.end(), rng);
    for (int v : values) {
        BOOST_TEST_CONTEXT("inserting " << v) {
            tree.insert(v);
            BOOST_CHECK(tree.validate()); // all leaves same depth, occupancy invariants
            BOOST_TEST_MESSAGE("Tree State: \n" << tree.to_string());
        }
    }

    std::shuffle(values.begin(), values.end(), rng);
    for (int v : values) {
        BOOST_TEST_CONTEXT("deleting " << v) {
            BOOST_ASSERT(tree.remove(v));
            BOOST_CHECK(tree.validate());
            BOOST_TEST_MESSAGE("Tree State: \n" << tree.to_string());

        }
    }
}




BOOST_AUTO_TEST_SUITE_END()