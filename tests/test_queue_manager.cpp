#include "core/playback/queue_manager.h"
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace tomato::core;

namespace {
void test_navigation() {
    QueueManager q;
    q.enqueue({"1", "a.mp4", "A", 1000, ""});
    q.enqueue({"2", "b.mp4", "B", 1000, ""});
    q.enqueue({"3", "c.mp4", "C", 1000, ""});
    assert(q.currentIndex() == 0 && q.currentItem()->title == "A");
    assert(q.next() && q.currentItem()->title == "B");
    assert(q.next() && q.currentItem()->title == "C");
    assert(!q.next());
    assert(q.previous() && q.currentItem()->title == "B");
    assert(q.previous() && q.currentItem()->title == "A");
    assert(!q.previous());
}
void test_repeat_modes() {
    QueueManager q;
    q.enqueue({"1", "a.mp4", "A", 1000, ""});
    q.enqueue({"2", "b.mp4", "B", 1000, ""});
    q.setRepeatMode(RepeatMode::RepeatOne);
    assert(q.jumpTo(0) && q.next() && q.currentItem()->title == "A");
    q.setRepeatMode(RepeatMode::RepeatAll);
    assert(q.jumpTo(1) && q.next() && q.currentItem()->title == "A");
    assert(q.previous() && q.currentItem()->title == "B");
}
void test_shuffle_visits_every_item() {
    QueueManager q;
    std::vector<QueueItem> items;
    for (int i = 0; i < 12; ++i) {
        const auto id = std::to_string(i);
        items.push_back({id, id + ".mp4", "Item " + id, 1000, ""});
    }
    q.enqueueBatch(items);
    assert(q.jumpTo(5));
    const auto selected = q.currentItem()->id;
    q.setShuffle(true);
    assert(q.currentItem()->id == selected);
    std::set<std::string> seen{selected};
    for (size_t i = 1; i < items.size(); ++i) {
        assert(q.next());
        seen.insert(q.currentItem()->id);
    }
    assert(seen.size() == items.size());
    assert(!q.next());
    const auto before = q.currentItem()->id;
    q.setShuffle(false);
    assert(q.currentItem()->id == before);
}
void test_mutations_and_bounds() {
    QueueManager q;
    q.enqueue({"1", "1.mp4", "1", 0, ""});
    q.enqueue({"2", "2.mp4", "2", 0, ""});
    q.enqueue({"3", "3.mp4", "3", 0, ""});
    assert(q.jumpTo(1));
    assert(q.move(0, 2));
    assert(q.items()[0].title == "2" && q.currentIndex() == 0);
    assert(q.remove(0) && q.currentItem()->title == "3");
    assert(!q.remove(99) && !q.move(0, 0) && !q.insert(99, {"x", "x", "x", 0, ""}));
    q.clear();
    assert(q.empty() && q.currentIndex() == -1);
    assert(!q.next() && !q.previous());
}
class SelfRemovingObserver final : public IQueueObserver {
public:
    explicit SelfRemovingObserver(QueueManager& q) : q_(q) {}
    void onQueueChanged() override { ++queueChanges; q_.removeObserver(this); }
    void onCurrentIndexChanged(int) override { ++indexChanges; }
    int queueChanges = 0;
    int indexChanges = 0;
private:
    QueueManager& q_;
};
void test_observer_reentrancy() {
    QueueManager q;
    SelfRemovingObserver observer(q);
    q.addObserver(&observer);
    q.enqueue({"1", "1", "1", 0, ""});
    q.enqueue({"2", "2", "2", 0, ""});
    assert(observer.queueChanges == 1 && observer.indexChanges == 1);
}
}
int main() {
    test_navigation();
    test_repeat_modes();
    test_shuffle_visits_every_item();
    test_mutations_and_bounds();
    test_observer_reentrancy();
    std::cout << "Tomato QueueManager tests passed.\n";
    return 0;
}
