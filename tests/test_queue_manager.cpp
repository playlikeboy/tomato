#include "core/playback/queue_manager.h"
#include <cassert>
#include <iostream>

using namespace tomato::core;

void test_basic_enqueue_and_navigation() {
    QueueManager qm;
    assert(qm.empty());
    assert(qm.size() == 0);
    assert(qm.currentIndex() == -1);

    QueueItem item1{"1", "file:///movie1.mp4", "Movie 1", 120000, "Director A"};
    QueueItem item2{"2", "file:///movie2.mp4", "Movie 2", 150000, "Director B"};
    QueueItem item3{"3", "file:///movie3.mp4", "Movie 3", 90000, "Director C"};

    qm.enqueue(item1);
    assert(!qm.empty());
    assert(qm.size() == 1);
    assert(qm.currentIndex() == 0);
    assert(qm.currentItem().has_value());
    assert(qm.currentItem()->title == "Movie 1");

    qm.enqueue(item2);
    qm.enqueue(item3);
    assert(qm.size() == 3);
    assert(qm.currentIndex() == 0);

    // Next
    bool hasNext = qm.next();
    assert(hasNext);
    assert(qm.currentIndex() == 1);
    assert(qm.currentItem()->title == "Movie 2");

    hasNext = qm.next();
    assert(hasNext);
    assert(qm.currentIndex() == 2);
    assert(qm.currentItem()->title == "Movie 3");

    // End reached with RepeatMode::None
    hasNext = qm.next();
    assert(!hasNext);
    assert(qm.currentIndex() == 2);

    // Previous
    bool hasPrev = qm.previous();
    assert(hasPrev);
    assert(qm.currentIndex() == 1);

    hasPrev = qm.previous();
    assert(hasPrev);
    assert(qm.currentIndex() == 0);

    hasPrev = qm.previous();
    assert(!hasPrev);
    assert(qm.currentIndex() == 0);

    std::cout << "  ✓ test_basic_enqueue_and_navigation passed" << std::endl;
}

void test_repeat_modes() {
    QueueManager qm;
    QueueItem item1{"1", "a.mp4", "Item 1", 1000, ""};
    QueueItem item2{"2", "b.mp4", "Item 2", 1000, ""};
    qm.enqueue(item1);
    qm.enqueue(item2);

    // RepeatOne
    qm.setRepeatMode(RepeatMode::RepeatOne);
    qm.jumpTo(0);
    assert(qm.next());
    assert(qm.currentIndex() == 0);

    // RepeatAll
    qm.setRepeatMode(RepeatMode::RepeatAll);
    qm.jumpTo(1);
    assert(qm.next());
    assert(qm.currentIndex() == 0); // Wrapped around to start

    assert(qm.previous());
    assert(qm.currentIndex() == 1); // Wrapped backward to end

    std::cout << "  ✓ test_repeat_modes passed" << std::endl;
}

void test_reorder_and_remove() {
    QueueManager qm;
    qm.enqueue({"1", "1.mp4", "1", 0, ""});
    qm.enqueue({"2", "2.mp4", "2", 0, ""});
    qm.enqueue({"3", "3.mp4", "3", 0, ""});
    qm.jumpTo(1); // Playing "2"

    // Move index 0 to index 2: [1, 2, 3] -> [2, 3, 1]
    qm.move(0, 2);
    assert(qm.items()[0].title == "2");
    assert(qm.items()[1].title == "3");
    assert(qm.items()[2].title == "1");
    assert(qm.currentIndex() == 0); // "2" was shifted to index 0

    // Remove current
    qm.remove(0);
    assert(qm.size() == 2);
    assert(qm.currentIndex() == 0);
    assert(qm.currentItem()->title == "3");

    // Clear
    qm.clear();
    assert(qm.empty());
    assert(qm.currentIndex() == -1);

    std::cout << "  ✓ test_reorder_and_remove passed" << std::endl;
}

int main() {
    std::cout << "[Tomato Tests] Running QueueManager Unit Tests..." << std::endl;
    test_basic_enqueue_and_navigation();
    test_repeat_modes();
    test_reorder_and_remove();
    std::cout << "[Tomato Tests] All QueueManager Tests Passed Successfully!" << std::endl;
    return 0;
}
