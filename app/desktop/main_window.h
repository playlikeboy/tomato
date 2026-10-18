#pragma once
#include "core/playback/queue_manager.h"
#include "engine/libvlc-adapter/libvlc_adapter.h"
#include <QMainWindow>
#include <QStringList>
class QCloseEvent;
class QComboBox;
class QDockWidget;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QShowEvent;
class QSlider;
class QTimer;
class QWidget;

namespace tomato::app {
class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(engine::LibVLCAdapter* player, QWidget* parent = nullptr);
    void openPaths(const QStringList& paths);
protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
private:
    void buildUi();
    void openMediaDialog();
    void openSubtitleDialog();
    void togglePlayPause();
    void playCurrentItem();
    void stopPlayback();
    void nextItem();
    void previousItem();
    void toggleFullscreen();
    void refreshTracks();
    void refreshQueue();
    void updateUi();
    void showContextMenu(const QPoint& position);
    static QString formatTime(int64_t milliseconds);

    engine::LibVLCAdapter* m_player = nullptr;
    core::QueueManager m_queue;
    QWidget* m_videoSurface = nullptr;
    QLabel* m_videoHint = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_elapsedLabel = nullptr;
    QLabel* m_durationLabel = nullptr;
    QPushButton* m_playButton = nullptr;
    QPushButton* m_muteButton = nullptr;
    QPushButton* m_queueButton = nullptr;
    QSlider* m_seekSlider = nullptr;
    QSlider* m_volumeSlider = nullptr;
    QComboBox* m_audioTracks = nullptr;
    QComboBox* m_subtitleTracks = nullptr;
    QDockWidget* m_queueDock = nullptr;
    QListWidget* m_queueList = nullptr;
    QTimer* m_pollTimer = nullptr;
    bool m_hasMedia = false;
    bool m_muted = false;
    engine::PlaybackState m_lastState = engine::PlaybackState::Stopped;
};
} // namespace tomato::app
