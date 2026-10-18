#include "core/playback/queue_manager.h"
#include <algorithm>
#include <numeric>
#include <chrono>

namespace tomato::core {

QueueManager::QueueManager()
    : m_rng(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())) {
}

void QueueManager::enqueue(const QueueItem& item) {
    m_items.push_back(item);
    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        notifyCurrentIndexChanged(0);
    }
    rebuildPlayOrder();
    notifyQueueChanged();
}

void QueueManager::enqueueBatch(const std::vector<QueueItem>& items) {
    if (items.empty()) return;
    bool wasEmpty = m_items.empty();
    m_items.insert(m_items.end(), items.begin(), items.end());
    if (wasEmpty && !m_items.empty()) {
        m_currentIndex = 0;
        notifyCurrentIndexChanged(0);
    }
    rebuildPlayOrder();
    notifyQueueChanged();
}

bool QueueManager::insert(size_t index, const QueueItem& item) {
    if (index > m_items.size()) return false;
    m_items.insert(m_items.begin() + index, item);
    if (m_currentIndex == -1) {
        m_currentIndex = static_cast<int>(index);
        notifyCurrentIndexChanged(m_currentIndex);
    } else if (static_cast<int>(index) <= m_currentIndex) {
        m_currentIndex++;
        notifyCurrentIndexChanged(m_currentIndex);
    }
    rebuildPlayOrder();
    notifyQueueChanged();
    return true;
}

bool QueueManager::remove(size_t index) {
    if (index >= m_items.size()) return false;
    m_items.erase(m_items.begin() + index);

    if (m_items.empty()) {
        m_currentIndex = -1;
        notifyCurrentIndexChanged(-1);
    } else if (static_cast<int>(index) < m_currentIndex) {
        m_currentIndex--;
        notifyCurrentIndexChanged(m_currentIndex);
    } else if (static_cast<int>(index) == m_currentIndex) {
        if (m_currentIndex >= static_cast<int>(m_items.size())) {
            m_currentIndex = static_cast<int>(m_items.size()) - 1;
        }
        notifyCurrentIndexChanged(m_currentIndex);
    }

    rebuildPlayOrder();
    notifyQueueChanged();
    return true;
}

bool QueueManager::move(size_t fromIndex, size_t toIndex) {
    if (fromIndex >= m_items.size() || toIndex >= m_items.size() || fromIndex == toIndex) {
        return false;
    }

    QueueItem item = m_items[fromIndex];
    m_items.erase(m_items.begin() + fromIndex);
    m_items.insert(m_items.begin() + toIndex, item);

    if (m_currentIndex == static_cast<int>(fromIndex)) {
        m_currentIndex = static_cast<int>(toIndex);
    } else if (fromIndex < static_cast<size_t>(m_currentIndex) && toIndex >= static_cast<size_t>(m_currentIndex)) {
        m_currentIndex--;
    } else if (fromIndex > static_cast<size_t>(m_currentIndex) && toIndex <= static_cast<size_t>(m_currentIndex)) {
        m_currentIndex++;
    }

    rebuildPlayOrder();
    notifyCurrentIndexChanged(m_currentIndex);
    notifyQueueChanged();
    return true;
}

void QueueManager::clear() {
    m_items.clear();
    m_playOrder.clear();
    m_currentIndex = -1;
    notifyCurrentIndexChanged(-1);
    notifyQueueChanged();
}

bool QueueManager::next() {
    if (m_items.empty()) return false;

    if (m_repeatMode == RepeatMode::RepeatOne) {
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }

    if (m_currentIndex + 1 < static_cast<int>(m_items.size())) {
        m_currentIndex++;
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }

    if (m_repeatMode == RepeatMode::RepeatAll) {
        m_currentIndex = 0;
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }

    return false;
}

bool QueueManager::previous() {
    if (m_items.empty()) return false;

    if (m_currentIndex > 0) {
        m_currentIndex--;
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }

    if (m_repeatMode == RepeatMode::RepeatAll && !m_items.empty()) {
        m_currentIndex = static_cast<int>(m_items.size()) - 1;
        notifyCurrentIndexChanged(m_currentIndex);
        return true;
    }

    return false;
}

bool QueueManager::jumpTo(size_t index) {
    if (index >= m_items.size()) return false;
    m_currentIndex = static_cast<int>(index);
    notifyCurrentIndexChanged(m_currentIndex);
    return true;
}

size_t QueueManager::size() const {
    return m_items.size();
}

bool QueueManager::empty() const {
    return m_items.empty();
}

int QueueManager::currentIndex() const {
    return m_currentIndex;
}

std::optional<QueueItem> QueueManager::currentItem() const {
    if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_items.size())) {
        return m_items[m_currentIndex];
    }
    return std::nullopt;
}

const std::vector<QueueItem>& QueueManager::items() const {
    return m_items;
}

void QueueManager::setShuffle(bool enabled) {
    if (m_shuffle == enabled) return;
    m_shuffle = enabled;
    rebuildPlayOrder();
}

bool QueueManager::isShuffle() const {
    return m_shuffle;
}

void QueueManager::setRepeatMode(RepeatMode mode) {
    m_repeatMode = mode;
}

RepeatMode QueueManager::repeatMode() const {
    return m_repeatMode;
}

void QueueManager::addObserver(IQueueObserver* observer) {
    if (observer && std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
        m_observers.push_back(observer);
    }
}

void QueueManager::removeObserver(IQueueObserver* observer) {
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void QueueManager::notifyQueueChanged() {
    for (auto* obs : m_observers) obs->onQueueChanged();
}

void QueueManager::notifyCurrentIndexChanged(int index) {
    for (auto* obs : m_observers) obs->onCurrentIndexChanged(index);
}

void QueueManager::rebuildPlayOrder() {
    m_playOrder.resize(m_items.size());
    std::iota(m_playOrder.begin(), m_playOrder.end(), 0);
    if (m_shuffle && m_playOrder.size() > 1) {
        std::shuffle(m_playOrder.begin(), m_playOrder.end(), m_rng);
    }
}

} // namespace tomato::core
