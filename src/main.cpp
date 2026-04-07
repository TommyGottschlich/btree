#include "btree.hpp"
#include "event_logger.hpp"

using namespace ds;

int main() {
    using Btree = Btree<int, 3>;

    Btree tree;
    EventLogger logger;
    tree.set_logger(&logger);

    std::vector<int> values = {10, 20, 5,  15, 25,
                               3,  7,  12, 18, 22}; // 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
    //};
    for (int value : values) {
        tree.insert(value);
        logger.print_events();
        std::cout << std::endl;
    }
    std::cout << "Tree " << (tree.validate() ? "valid" : "not valid") << std::endl;
    tree.print();

    logger.print_events();

    return 0;
}