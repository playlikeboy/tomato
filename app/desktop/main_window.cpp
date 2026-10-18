#include "app/desktop/main_window.h"
#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QShowEvent>
#include <QShortcut>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cstdint>
#include <vector>

namespace tomato::app {
namespace {
void* nativeWindowHandle(QWidget* widget) {
    const WId id = widget->winId();
#if defined(_WIN32) || defined(__APPLE__)
    return reinterpret_cast<void*>(id);
#else
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(id));
#endif
}
QPushButton* button(const QString& text, const QString& tip, QWidget* parent, const char* name = nullptr) {
    auto* result = new QPushButton(text, parent);
    result->setToolTip(tip);
    result->setCursor(Qt::PointingHandCursor);
    result->setMinimumSize(34, 32);
    if (name) result->setObjectName(QString::fromLatin1(name));
    return result;
}
} // namespace

MainWindow::MainWindow(engine::LibVLCAdapter* player, QWidget* parent)
    : QMainWindow(parent), m_player(player) {
    setWindowTitle("Tomato");
    resize(1280, 760);
    setMinimumSize(760, 480);
    buildUi();
    m_player->setVolume(100);
    m_volumeSlider->setValue(m_player->getVolume());
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(200);
    connect(m_pollTimer, &QTimer::timeout, this, [this] { updateUi(); });
    m_pollTimer->start();

    auto* playShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(playShortcut, &QShortcut::activated, this, [this] { togglePlayPause(); });
    auto* openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, [this] { openMediaDialog(); });
    auto* subShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O), this);
    connect(subShortcut, &QShortcut::activated, this, [this] { openSubtitleDialog(); });
    auto* fullShortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(fullShortcut, &QShortcut::activated, this, [this] { toggleFullscreen(); });
    auto* quitShortcut = new QShortcut(QKeySequence::Quit, this);
    connect(quitShortcut, &QShortcut::activated, this, [this] { close(); });
    auto* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, [this] { if (isFullScreen()) showNormal(); });
}

void MainWindow::buildUi() {
    setStyleSheet(R"(
      QMainWindow, QWidget { background:#080909; color:#F5F3EF; font-family:"Segoe UI","SF Pro Text",sans-serif; font-size:13px; }
      QWidget#topBar { background:#111212; border-bottom:1px solid #222222; }
      QWidget#controlPanel { background:#101111; border-top:1px solid #242424; }
      QWidget#videoSurface { background:#000000; }
      QLabel#brand { color:#F5F3EF; font-size:15px; font-weight:700; }
      QLabel#appMark { color:#E96914; font-size:19px; }
      QLabel#mediaTitle, QLabel#timeLabel { color:#A3A3A3; font-size:11px; }
      QLabel#timeLabel { min-width:52px; }
      QLabel#videoHint { color:#777777; background:transparent; font-size:14px; }
      QPushButton { color:#D7D7D7; background:transparent; border:1px solid transparent; border-radius:7px; padding:5px 9px; }
      QPushButton:hover { background:#262626; color:#FFFFFF; }
      QPushButton:focus { border:1px solid #E96914; }
      QPushButton#playButton { color:#111111; background:#E96914; font-size:16px; font-weight:700; }
      QPushButton#playButton:hover { background:#F27A2B; }
      QSlider::groove:horizontal { background:#303030; height:6px; border-radius:3px; }
      QSlider::sub-page:horizontal { background:#E96914; border-radius:3px; }
      QSlider::add-page:horizontal { background:#303030; border-radius:3px; }
      QSlider::handle:horizontal { background:#E96914; width:11px; margin:-4px 0; border-radius:5px; }
      QComboBox { color:#D7D7D7; background:#191919; border:1px solid #303030; border-radius:6px; padding:5px 7px; min-height:20px; }
      QComboBox QAbstractItemView { background:#191919; color:#F5F3EF; selection-background-color:#353535; }
      QDockWidget::title { padding:8px; background:#191919; }
      QListWidget { background:#101111; border:none; outline:none; }
      QListWidget::item { padding:8px; border-radius:5px; }
      QListWidget::item:selected { background:#29231F; border-left:2px solid #E96914; }
      QMenu { background:#191919; color:#F5F3EF; border:1px solid #303030; padding:5px; }
      QMenu::item { padding:7px 28px 7px 12px; border-radius:5px; }
      QMenu::item:selected { background:#303030; }
      QMenu::separator { height:1px; background:#303030; margin:5px 8px; }
    )");
    auto* root = new QWidget(this);
    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* header = new QWidget(root);
    header->setObjectName("topBar");
    header->setFixedHeight(42);
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(14, 0, 12, 0);
    headerLayout->setSpacing(8);
    auto* mark = new QLabel("🍅", header); mark->setObjectName("appMark");
    auto* brand = new QLabel("Tomato", header); brand->setObjectName("brand");
    m_titleLabel = new QLabel("Open a media file", header);
    m_titleLabel->setObjectName("mediaTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    auto* openButton = button("Open", "Open media files (Ctrl+O)", header);
    auto* menuButton = button("⋯", "More actions", header);
    headerLayout->addWidget(mark);
    headerLayout->addWidget(brand);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel, 2);
    headerLayout->addStretch();
    headerLayout->addWidget(openButton);
    headerLayout->addWidget(menuButton);
    layout->addWidget(header);

    m_videoSurface = new QWidget(root);
    m_videoSurface->setObjectName("videoSurface");
    m_videoSurface->setAttribute(Qt::WA_NativeWindow, true);
    m_videoSurface->setMinimumSize(600, 300);
    m_videoSurface->setContextMenuPolicy(Qt::CustomContextMenu);
    auto* videoLayout = new QVBoxLayout(m_videoSurface);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->addStretch();
    m_videoHint = new QLabel("Open a video to get started\nYour media stays on your device", m_videoSurface);
    m_videoHint->setObjectName("videoHint");
    m_videoHint->setAlignment(Qt::AlignCenter);
    videoLayout->addWidget(m_videoHint, 0, Qt::AlignHCenter);
    videoLayout->addStretch();
    layout->addWidget(m_videoSurface, 1);

    auto* panel = new QWidget(root);
    panel->setObjectName("controlPanel");
    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(12, 8, 12, 10);
    panelLayout->setSpacing(8);
    auto* timeline = new QHBoxLayout();
    m_elapsedLabel = new QLabel("00:00", panel); m_elapsedLabel->setObjectName("timeLabel");
    m_durationLabel = new QLabel("00:00", panel); m_durationLabel->setObjectName("timeLabel");
    m_seekSlider = new QSlider(Qt::Horizontal, panel);
    m_seekSlider->setRange(0, 10000);
    m_seekSlider->setEnabled(false);
    timeline->addWidget(m_elapsedLabel);
    timeline->addWidget(m_seekSlider, 1);
    timeline->addWidget(m_durationLabel);
    panelLayout->addLayout(timeline);

    auto* row = new QHBoxLayout();
    row->setSpacing(5);
    auto* prev = button("◀|", "Previous item (P)", panel);
    m_playButton = button("▶", "Play or pause (Space)", panel, "playButton");
    m_playButton->setFixedSize(38, 32);
    auto* stop = button("■", "Stop (S)", panel);
    auto* next = button("|▶", "Next item (N)", panel);
    m_audioTracks = new QComboBox(panel);
    m_audioTracks->setMinimumWidth(90); m_audioTracks->setMaximumWidth(155);
    m_audioTracks->setToolTip("Audio track");
    auto* subOpen = button("CC+", "Open a subtitle file (Ctrl+Shift+O)", panel);
    m_subtitleTracks = new QComboBox(panel);
    m_subtitleTracks->setMinimumWidth(80); m_subtitleTracks->setMaximumWidth(150);
    m_subtitleTracks->addItem("Off", -1);
    m_muteButton = button("Volume", "Toggle mute", panel);
    m_volumeSlider = new QSlider(Qt::Horizontal, panel);
    m_volumeSlider->setRange(0, 200); m_volumeSlider->setFixedWidth(88);
    m_queueButton = button("Queue (0)", "Show playback queue", panel);
    auto* full = button("⛶", "Toggle fullscreen (F11)", panel);
    row->addWidget(prev); row->addWidget(m_playButton); row->addWidget(stop); row->addWidget(next);
    row->addStretch();
    row->addWidget(m_audioTracks); row->addWidget(subOpen); row->addWidget(m_subtitleTracks);
    row->addSpacing(6);
    row->addWidget(m_muteButton); row->addWidget(m_volumeSlider); row->addWidget(m_queueButton); row->addWidget(full);
    panelLayout->addLayout(row);
    layout->addWidget(panel);
    setCentralWidget(root);

    m_queueDock = new QDockWidget("Playback Queue", this);
    m_queueDock->setMinimumWidth(230);
    m_queueList = new QListWidget(m_queueDock);
    m_queueDock->setWidget(m_queueList);
    addDockWidget(Qt::RightDockWidgetArea, m_queueDock);
    m_queueDock->hide();

    connect(openButton, &QPushButton::clicked, this, [this] { openMediaDialog(); });
    connect(menuButton, &QPushButton::clicked, this, [this, menuButton] {
        QMenu menu(this);
        auto* open = menu.addAction("Open Media…");
        auto* subtitle = menu.addAction("Open Subtitle…");
        menu.addSeparator();
        auto* queue = menu.addAction("Show Queue");
        auto* fullscreen = menu.addAction("Toggle Fullscreen");
        menu.addSeparator();
        auto* quit = menu.addAction("Quit Tomato");
        auto* selected = menu.exec(menuButton->mapToGlobal(QPoint(0, menuButton->height())));
        if (selected == open) openMediaDialog();
        else if (selected == subtitle) openSubtitleDialog();
        else if (selected == queue) m_queueDock->show();
        else if (selected == fullscreen) toggleFullscreen();
        else if (selected == quit) close();
    });
    connect(m_videoSurface, &QWidget::customContextMenuRequested, this,
            [this](const QPoint& p) { showContextMenu(p); });
    connect(prev, &QPushButton::clicked, this, [this] { previousItem(); });
    connect(m_playButton, &QPushButton::clicked, this, [this] { togglePlayPause(); });
    connect(stop, &QPushButton::clicked, this, [this] { stopPlayback(); });
    connect(next, &QPushButton::clicked, this, [this] { nextItem(); });
    connect(subOpen, &QPushButton::clicked, this, [this] { openSubtitleDialog(); });
    connect(full, &QPushButton::clicked, this, [this] { toggleFullscreen(); });
    connect(m_queueButton, &QPushButton::clicked, this, [this] { m_queueDock->setVisible(!m_queueDock->isVisible()); });
    connect(m_muteButton, &QPushButton::clicked, this, [this] {
        m_muted = !m_muted; m_player->setMuted(m_muted);
        m_muteButton->setText(m_muted ? "Muted" : "Volume");
    });
    connect(m_volumeSlider, &QSlider::valueChanged, this, [this](int value) { m_player->setVolume(value); });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this] {
        if (!m_hasMedia) return;
        const auto pos = m_player->getPosition();
        if (pos.durationMs > 0) {
            const auto t = static_cast<int64_t>(static_cast<double>(m_seekSlider->value()) / m_seekSlider->maximum() * pos.durationMs);
            m_player->seekMs(t);
        }
    });
    connect(m_seekSlider, &QSlider::sliderMoved, this, [this](int value) {
        if (!m_hasMedia) return;
        const auto pos = m_player->getPosition();
        if (pos.durationMs > 0) {
            const auto t = static_cast<int64_t>(static_cast<double>(value) / m_seekSlider->maximum() * pos.durationMs);
            m_elapsedLabel->setText(formatTime(t));
        }
    });
    connect(m_audioTracks, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int i) {
        if (i >= 0) { const int id = m_audioTracks->itemData(i).toInt(); if (id >= -1) m_player->selectAudioTrack(id); }
    });
    connect(m_subtitleTracks, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int i) {
        if (i >= 0) { const int id = m_subtitleTracks->itemData(i).toInt(); if (id >= -1) m_player->selectSubtitleTrack(id); }
    });
    connect(m_queueList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        const int rowIndex = item->data(Qt::UserRole).toInt();
        if (rowIndex >= 0 && m_queue.jumpTo(static_cast<size_t>(rowIndex))) playCurrentItem();
    });
}

void MainWindow::openMediaDialog() {
    const auto paths = QFileDialog::getOpenFileNames(this, "Open Media", QString(),
        "Media files (*.mp4 *.mkv *.avi *.mov *.webm *.mp3 *.flac *.wav *.ogg *.m4a *.ts *.mpeg *.mpg);;All files (*)");
    if (!paths.isEmpty()) openPaths(paths);
}
void MainWindow::openPaths(const QStringList& paths) {
    if (paths.isEmpty()) return;
    std::vector<core::QueueItem> items;
    items.reserve(static_cast<size_t>(paths.size()));
    for (const auto& path : paths) {
        const QFileInfo info(path);
        items.push_back({path.toStdString(), path.toStdString(), info.fileName().toStdString(), 0, ""});
    }
    m_queue.clear();
    m_queue.enqueueBatch(items);
    refreshQueue();
    playCurrentItem();
}
void MainWindow::openSubtitleDialog() {
    const auto path = QFileDialog::getOpenFileName(this, "Open Subtitle", QString(),
        "Subtitle files (*.srt *.ass *.ssa *.vtt *.sub);;All files (*)");
    if (path.isEmpty()) return;
    if (!m_player->addSubtitleFile(path.toStdString())) {
        QMessageBox::warning(this, "Tomato", "The subtitle could not be attached to the current media.");
        return;
    }
    QTimer::singleShot(500, this, [this] { refreshTracks(); });
}
void MainWindow::togglePlayPause() {
    if (!m_hasMedia) { openMediaDialog(); return; }
    const auto state = m_player->getState();
    if (state == engine::PlaybackState::Playing || state == engine::PlaybackState::Buffering) m_player->pause();
    else if (state == engine::PlaybackState::Ended || state == engine::PlaybackState::Stopped) playCurrentItem();
    else m_player->play();
}
void MainWindow::playCurrentItem() {
    const auto item = m_queue.currentItem();
    if (!item) return;
    m_player->attachWindow(nativeWindowHandle(m_videoSurface));
    if (!m_player->loadMedia(item->uri)) {
        m_hasMedia = false; m_videoHint->setText("Unable to open this media file."); m_videoHint->show();
        QMessageBox::warning(this, "Tomato", "LibVLC could not open the selected media file.");
        return;
    }
    m_hasMedia = true;
    m_lastState = engine::PlaybackState::Opening;
    m_titleLabel->setText(QString::fromStdString(item->title));
    setWindowTitle(QString("Tomato — %1").arg(QString::fromStdString(item->title)));
    m_videoHint->hide();
    m_seekSlider->setEnabled(true);
    m_player->play();
    refreshQueue();
    QTimer::singleShot(700, this, [this] { refreshTracks(); });
}
void MainWindow::stopPlayback() {
    m_player->stop(); m_lastState = engine::PlaybackState::Stopped;
    if (m_hasMedia) { m_videoHint->setText("Playback stopped"); m_videoHint->show(); }
}
void MainWindow::nextItem() { if (m_queue.next()) playCurrentItem(); }
void MainWindow::previousItem() {
    if (m_hasMedia && m_player->getPosition().timeMs > 3000) { m_player->seekMs(0); return; }
    if (m_queue.previous()) playCurrentItem();
}
void MainWindow::toggleFullscreen() { if (isFullScreen()) showNormal(); else showFullScreen(); }

void MainWindow::refreshTracks() {
    if (!m_hasMedia) return;
    const auto audio = m_player->getAudioTracks();
    const auto subtitles = m_player->getSubtitleTracks();
    {
        const QSignalBlocker blocker(m_audioTracks);
        m_audioTracks->clear();
        int selected = -1;
        for (const auto& track : audio) {
            m_audioTracks->addItem(QString::fromStdString(track.name), track.id);
            if (track.isSelected) selected = m_audioTracks->count() - 1;
        }
        if (audio.empty()) { m_audioTracks->addItem("No audio track", -2); m_audioTracks->setEnabled(false); }
        else { m_audioTracks->setEnabled(true); m_audioTracks->setCurrentIndex(selected >= 0 ? selected : 0); }
    }
    {
        const QSignalBlocker blocker(m_subtitleTracks);
        m_subtitleTracks->clear();
        m_subtitleTracks->addItem("Off", -1);
        int selected = 0;
        for (const auto& track : subtitles) {
            if (track.id == -1) { if (track.isSelected) selected = 0; continue; }
            m_subtitleTracks->addItem(QString::fromStdString(track.name), track.id);
            if (track.isSelected) selected = m_subtitleTracks->count() - 1;
        }
        m_subtitleTracks->setCurrentIndex(selected);
        m_subtitleTracks->setEnabled(true);
    }
}
void MainWindow::refreshQueue() {
    m_queueList->clear();
    const auto& items = m_queue.items();
    for (size_t i = 0; i < items.size(); ++i) {
        auto* row = new QListWidgetItem(QString::fromStdString(items[i].title));
        row->setData(Qt::UserRole, static_cast<int>(i));
        if (static_cast<int>(i) == m_queue.currentIndex()) row->setText("▶  " + row->text());
        m_queueList->addItem(row);
        if (static_cast<int>(i) == m_queue.currentIndex()) m_queueList->setCurrentItem(row);
    }
    m_queueButton->setText(QString("Queue (%1)").arg(items.size()));
}
void MainWindow::updateUi() {
    if (!m_player) return;
    const auto state = m_player->getState();
    if (state == engine::PlaybackState::Ended && m_lastState != engine::PlaybackState::Ended) {
        m_lastState = state;
        if (m_queue.next()) { playCurrentItem(); return; }
    }
    m_lastState = state;
    const bool playing = state == engine::PlaybackState::Playing || state == engine::PlaybackState::Buffering;
    m_playButton->setText(playing ? "Ⅱ" : "▶");
    m_playButton->setToolTip(playing ? "Pause (Space)" : "Play (Space)");
    if (!m_hasMedia || state == engine::PlaybackState::Error) {
        m_videoHint->show();
        if (state == engine::PlaybackState::Error) m_videoHint->setText("Playback failed. Try another file.");
        m_seekSlider->setEnabled(false);
        return;
    }
    if (state != engine::PlaybackState::Stopped) m_videoHint->hide();
    const auto position = m_player->getPosition();
    m_elapsedLabel->setText(formatTime(position.timeMs));
    m_durationLabel->setText(formatTime(position.durationMs));
    if (position.durationMs > 0) {
        m_seekSlider->setEnabled(true);
        if (!m_seekSlider->isSliderDown()) {
            const int value = static_cast<int>(static_cast<double>(position.timeMs) / position.durationMs * m_seekSlider->maximum());
            m_seekSlider->setValue(std::clamp(value, 0, m_seekSlider->maximum()));
        }
    } else if (!m_seekSlider->isSliderDown()) m_seekSlider->setValue(0);
}
void MainWindow::showContextMenu(const QPoint& position) {
    QMenu menu(this);
    auto* play = menu.addAction(m_player->getState() == engine::PlaybackState::Playing ? "Pause" : "Play");
    auto* stop = menu.addAction("Stop");
    menu.addSeparator();
    auto* sub = menu.addAction("Open Subtitle…");
    auto* open = menu.addAction("Open Media…");
    menu.addSeparator();
    auto* queue = menu.addAction("Show Playback Queue");
    auto* fullscreen = menu.addAction("Toggle Fullscreen");
    auto* selected = menu.exec(m_videoSurface->mapToGlobal(position));
    if (selected == play) togglePlayPause();
    else if (selected == stop) stopPlayback();
    else if (selected == sub) openSubtitleDialog();
    else if (selected == open) openMediaDialog();
    else if (selected == queue) m_queueDock->show();
    else if (selected == fullscreen) toggleFullscreen();
}
QString MainWindow::formatTime(int64_t milliseconds) {
    const int64_t seconds = std::max<int64_t>(0, milliseconds) / 1000;
    const int64_t hours = seconds / 3600;
    const int64_t minutes = (seconds % 3600) / 60;
    const int64_t remainder = seconds % 60;
    if (hours > 0) return QString("%1:%2:%3").arg(hours).arg(minutes, 2, 10, QChar('0')).arg(remainder, 2, 10, QChar('0'));
    return QString("%1:%2").arg(minutes).arg(remainder, 2, 10, QChar('0'));
}
void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    QTimer::singleShot(0, this, [this] { if (m_player && m_videoSurface) m_player->attachWindow(nativeWindowHandle(m_videoSurface)); });
}
void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_pollTimer) m_pollTimer->stop();
    if (m_player) m_player->stop();
    QMainWindow::closeEvent(event);
}
} // namespace tomato::app
