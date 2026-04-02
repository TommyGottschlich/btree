#include "btree.hpp"
#include "event_logger.hpp"

using namespace ds;

int main() {
    using Btree = Btree<int, 3>;

    Btree tree;
    std::vector<int> values = {10, 20, 5,  15, 25,
                               3,  7,  12, 18, 22}; // 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    //};
    for (int value : values) {
        tree.insert(value);
    }
    std::cout << "Tree " << (tree.validate() ? "valid" : "not valid") << std::endl;
    tree.print();
    EventLogger logger;

    logger.log(tree);

    return 0;
}