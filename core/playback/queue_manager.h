#pragma once
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace tomato::core {
enum class RepeatMode { None, RepeatOne, RepeatAll };

struct QueueItem {
    std::string id;
    std::string uri;
    std::string title;
    int64_t durationMs = 0;
    std::string artist;
};

class IQueueObserver {
public:
    virtual ~IQueueObserver() = default;
    virtual void onQueueChanged() = 0;
    virtual void onCurrentIndexChanged(int newIndex) = 0;
};

class QueueManager {
public:
    QueueManager();
    void enqueue(const QueueItem& item);
    void enqueueBatch(const std::vector<QueueItem>& items);
    bool insert(size_t index, const QueueItem& item);
    bool remove(size_t index);
    bool move(size_t fromIndex, size_t toIndex);
    void clear();

    bool next();
    bool previous();
    bool jumpTo(size_t index);

    size_t size() const;
    bool empty() const;
    int currentIndex() const;
    std::optional<QueueItem> currentItem() const;
    const std::vector<QueueItem>& items() const;

    void setShuffle(bool enabled);
    bool isShuffle() const;
    void setRepeatMode(RepeatMode mode);
    RepeatMode repeatMode() const;

    void addObserver(IQueueObserver* observer);
    void removeObserver(IQueueObserver* observer);

private:
    void notifyQueueChanged();
    void notifyCurrentIndexChanged(int index);
    void rebuildPlayOrder();

    std::vector<QueueItem> m_items;
    std::vector<size_t> m_playOrder;
    int m_playOrderPosition = -1;
    int m_currentIndex = -1;
    bool m_shuffle = false;
    RepeatMode m_repeatMode = RepeatMode::None;
    std::mt19937 m_rng;
    mutable std::mutex m_observerMutex;
    std::vector<IQueueObserver*> m_observers;
};
} // namespace tomato::core
