#include "core/playback/queue_manager.h"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <utility>

namespace tomato::core {
QueueManager::QueueManager()
    : m_rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())) {}

void QueueManager::enqueue(const QueueItem& item) {
    const bool hadNoCurrent = m_currentIndex < 0;
    m_items.push_back(item);
    if (hadNoCurrent) m_currentIndex = 0;
    rebuildPlayOrder();
    if (hadNoCurrent) notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
}

void QueueManager::enqueueBatch(const std::vector<QueueItem>& items) {
    if (items.empty()) return;
    const bool hadNoCurrent = m_currentIndex < 0;
    m_items.insert(m_items.end(), items.begin(), items.end());
    if (hadNoCurrent && !m_items.empty()) m_currentIndex = 0;
    rebuildPlayOrder();
    if (hadNoCurrent && m_currentIndex >= 0) notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
}

bool QueueManager::insert(size_t index, const QueueItem& item) {
    if (index > m_items.size()) return false;
    m_items.insert(m_items.begin() + static_cast<std::ptrdiff_t>(index), item);
    bool currentChanged = false;
    if (m_currentIndex < 0) {
        m_currentIndex = static_cast<int>(index);
        currentChanged = true;
    } else if (index <= static_cast<size_t>(m_currentIndex)) {
        ++m_currentIndex;
        currentChanged = true;
    }
    rebuildPlayOrder();
    if (currentChanged) notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
    return true;
}

bool QueueManager::remove(size_t index) {
    if (index >= m_items.size()) return false;
    const int oldIndex = m_currentIndex;
    m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(index));
    if (m_items.empty()) m_currentIndex = -1;
    else if (static_cast<int>(index) < m_currentIndex) --m_currentIndex;
    else if (static_cast<int>(index) == m_currentIndex)
        m_currentIndex = std::min(m_currentIndex, static_cast<int>(m_items.size()) - 1);
    rebuildPlayOrder();
    if (m_currentIndex != oldIndex) notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
    return true;
}

bool QueueManager::move(size_t from, size_t to) {
    if (from >= m_items.size() || to >= m_items.size() || from == to) return false;
    const int oldCurrent = m_currentIndex;
    QueueItem item = std::move(m_items[from]);
    m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(from));
    m_items.insert(m_items.begin() + static_cast<std::ptrdiff_t>(to), std::move(item));
    if (m_currentIndex == static_cast<int>(from)) m_currentIndex = static_cast<int>(to);
    else if (m_currentIndex >= 0 && from < static_cast<size_t>(m_currentIndex) && to >= static_cast<size_t>(m_currentIndex)) --m_currentIndex;
    else if (m_currentIndex >= 0 && from > static_cast<size_t>(m_currentIndex) && to <= static_cast<size_t>(m_currentIndex)) ++m_currentIndex;
    rebuildPlayOrder();
    if (m_currentIndex != oldCurrent) notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
    return true;
}

void QueueManager::clear() {
    const bool hadCurrent = m_currentIndex >= 0;
    m_items.clear();
    m_playOrder.clear();
    m_playOrderPosition = -1;
    m_currentIndex = -1;
    if (hadCurrent) notifyCurrentIndexChanged(-1);
    notifyQueueChanged();
}

bool QueueManager::next() {
    if (m_items.empty()) return false;
    if (m_currentIndex >= 0 && m_repeatMode == RepeatMode::RepeatOne) {
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    if (m_playOrderPosition < 0) {
        m_playOrderPosition = 0;
        m_currentIndex = static_cast<int>(m_playOrder.front());
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    const int nextPosition = m_playOrderPosition + 1;
    if (nextPosition < static_cast<int>(m_playOrder.size())) {
        m_playOrderPosition = nextPosition;
        m_currentIndex = static_cast<int>(m_playOrder[static_cast<size_t>(m_playOrderPosition)]);
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    if (m_repeatMode == RepeatMode::RepeatAll) {
        m_playOrderPosition = 0;
        m_currentIndex = static_cast<int>(m_playOrder.front());
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    return false;
}

bool QueueManager::previous() {
    if (m_items.empty()) return false;
    if (m_playOrderPosition < 0) {
        m_playOrderPosition = static_cast<int>(m_playOrder.size()) - 1;
        m_currentIndex = static_cast<int>(m_playOrder[static_cast<size_t>(m_playOrderPosition)]);
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    if (m_playOrderPosition > 0) {
        --m_playOrderPosition;
        m_currentIndex = static_cast<int>(m_playOrder[static_cast<size_t>(m_playOrderPosition)]);
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    if (m_repeatMode == RepeatMode::RepeatAll) {
        m_playOrderPosition = static_cast<int>(m_playOrder.size()) - 1;
        m_currentIndex = static_cast<int>(m_playOrder.back());
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }
    return false;
}

bool QueueManager::jumpTo(size_t index) {
    if (index >= m_items.size()) return false;
    auto it = std::find(m_playOrder.begin(), m_playOrder.end(), index);
    if (it == m_playOrder.end()) {
        rebuildPlayOrder();
        it = std::find(m_playOrder.begin(), m_playOrder.end(), index);
    }
    if (it == m_playOrder.end()) return false;
    m_playOrderPosition = static_cast<int>(std::distance(m_playOrder.begin(), it));
    m_currentIndex = static_cast<int>(index);
    notifyCurrentIndexChanged(m_currentIndex);
    return true;
}

size_t QueueManager::size() const { return m_items.size(); }
bool QueueManager::empty() const { return m_items.empty(); }
int QueueManager::currentIndex() const { return m_currentIndex; }

std::optional<QueueItem> QueueManager::currentItem() const {
    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_items.size()))
        return m_items[static_cast<size_t>(m_currentIndex)];
    return std::nullopt;
}
const std::vector<QueueItem>& QueueManager::items() const { return m_items; }

void QueueManager::setShuffle(bool enabled) {
    if (m_shuffle == enabled) return;
    m_shuffle = enabled;
    rebuildPlayOrder();
}
bool QueueManager::isShuffle() const { return m_shuffle; }
void QueueManager::setRepeatMode(RepeatMode mode) { m_repeatMode = mode; }
RepeatMode QueueManager::repeatMode() const { return m_repeatMode; }

void QueueManager::addObserver(IQueueObserver* observer) {
    if (!observer) return;
    std::lock_guard<std::mutex> lock(m_observerMutex);
    if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end())
        m_observers.push_back(observer);
}
void QueueManager::removeObserver(IQueueObserver* observer) {
    std::lock_guard<std::mutex> lock(m_observerMutex);
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}
void QueueManager::notifyQueueChanged() {
    std::vector<IQueueObserver*> observers;
    { std::lock_guard<std::mutex> lock(m_observerMutex); observers = m_observers; }
    for (auto* observer : observers) if (observer) observer->onQueueChanged();
}
void QueueManager::notifyCurrentIndexChanged(int index) {
    std::vector<IQueueObserver*> observers;
    { std::lock_guard<std::mutex> lock(m_observerMutex); observers = m_observers; }
    for (auto* observer : observers) if (observer) observer->onCurrentIndexChanged(index);
}

void QueueManager::rebuildPlayOrder() {
    m_playOrder.clear();
    m_playOrder.reserve(m_items.size());
    for (size_t i = 0; i < m_items.size(); ++i) m_playOrder.push_back(i);

    const bool hasCurrent = m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_items.size());
    if (m_shuffle && m_playOrder.size() > 1) {
        std::vector<size_t> remaining;
        remaining.reserve(m_items.size());
        for (size_t index : m_playOrder)
            if (!hasCurrent || index != static_cast<size_t>(m_currentIndex)) remaining.push_back(index);
        std::shuffle(remaining.begin(), remaining.end(), m_rng);
        m_playOrder = std::move(remaining);
        if (hasCurrent) {
            m_playOrder.insert(m_playOrder.begin(), static_cast<size_t>(m_currentIndex));
            m_playOrderPosition = 0;
        } else {
            m_playOrderPosition = -1;
        }
        return;
    }
    m_playOrderPosition = hasCurrent ? m_currentIndex : -1;
}
} // namespace tomato::core
