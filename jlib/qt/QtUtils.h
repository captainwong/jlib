#pragma once

#include <QApplication>
#include <QColor>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <cassert>

#ifdef Q_OS_WIN
#include <QDesktopWidget>
#endif
#include <QDir>
#include <QIcon>
#include <QProcess>
#include <QSet>
#include <QTimer>
#include <chrono>
#include <thread>

#include "qt_global.h"
#include "QtDebug.h"
#include "QtPathHelper.h"

JLIBQT_NAMESPACE_BEGIN

/**
 * @brief 阻塞等待一段时间
 * @param ms 等待的毫秒数
 */
inline void blocking_wait(int ms, QString log = "") {
    if (!log.isEmpty()) {
        MYQDEBUG << log << "in";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    if (!log.isEmpty()) {
        MYQDEBUG << log << "out";
    }
}

/**
 * @brief 不阻塞UI响应的情况下，等待一段时间
 * @param ms 等待的毫秒数
 */
inline void non_blocking_wait_in_ui_thread(int ms, QString log = "") {
    if (!log.isEmpty()) {
        MYQDEBUG << log << "in";
    }
    QEventLoop q;
    QTimer t;
    t.setSingleShot(true);
    QObject::connect(&t, &QTimer::timeout, &q, &QEventLoop::quit);
    t.start(ms);
    q.exec();
    if (!log.isEmpty()) {
        MYQDEBUG << log << "out";
    }
}

#ifdef Q_OS_WIN
/**
 * @brief 在文件夹中显示
 * @param pathIn 若为文件路径则在文件夹中显示，若为文件夹路径则打开改文件夹
 */
inline void showInGraphicalShell(const QString& pathIn) {
    QString param;
    if (!QFileInfo(pathIn).isDir())
        param = QLatin1String("/select,");
    param += QDir::toNativeSeparators(pathIn);
    QString command = "explorer.exe " + param;
    QProcess::startDetached(command);
}
#endif

inline bool warn_if_load_pixmap_failed(QPixmap& pixmap, QString icon_path, QString file_line, bool forceUseQss = true) {
    /*if (!QDir().isAbsolutePath(icon_path)) {
            icon_path = PathHelper::program() + "/" + icon_path;
    }*/

    if (forceUseQss && !icon_path.startsWith(":/")) {
        icon_path = ":/" + icon_path;
    }

    if (!pixmap.load(icon_path) && !pixmap.load(icon_path, "png")) {
        qCritical() << file_line << "load pixmap failed: " << icon_path;
        assert(0);
        return false;
    }
    return true;
}

#define LOAD_PIXMAP_EX(icon_path) JLIBQT_NAMESPACE warn_if_load_pixmap_failed(pixmap, icon_path, JLIBQT_QDEBUG_FILE_LINE_VALUE)
#define LOAD_PIXMAP_EX2(icon_path) JLIBQT_NAMESPACE warn_if_load_pixmap_failed(pixmap, icon_path, JLIBQT_QDEBUG_FILE_LINE_VALUE, false)

inline QIcon icon_from_path(QString path, QSize icon_sz) {
    QPixmap pixmap;
    LOAD_PIXMAP_EX(path);
    return QIcon(pixmap.scaled(icon_sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

inline void fill_bg_with_color(QWidget* widget, QColor color) {
    if (!widget) {
        return;
    }

    widget->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    widget->setPalette(palette);
}

inline void set_pos(QWidget* widget, QRect pos) {
    if (!widget) {
        return;
    }
    widget->setFixedSize(pos.width(), pos.height());
    widget->move(pos.left(), pos.top());
}

inline void set_image_bg(QWidget* widget, const QPixmap& pixmap) {
    if (!widget || pixmap.isNull()) return;
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    widget->setPalette(palette);
}

inline void set_image_bg(QWidget* widget, const QString& icon_path) {
    QPixmap pixmap;
    if (!LOAD_PIXMAP_EX(icon_path)) {
        return;
    }
    set_image_bg(widget, pixmap);
}

inline void center_to_parent(QWidget* widget, QWidget* parent) {
    if (!widget || !parent) {
        return;
    }
    auto center = parent->rect().center();
    widget->move(center.x() - widget->width() / 2, center.y() - widget->height() / 2);
}

inline void center_to_desktop(QWidget* widget) {
    if (!widget) {
        return;
    }
    auto center = QApplication::desktop()->availableGeometry(widget).center();
    widget->move(center.x() - widget->width() / 2, center.y() - widget->height() / 2);
}

inline void center_to_desktop(QWidget* widget, int new_width, int new_height) {
    if (!widget) {
        return;
    }
    auto center = QApplication::desktop()->availableGeometry(widget).center();
    widget->setFixedSize(new_width, new_height);
    widget->move(center.x() - new_width / 2, center.y() - new_height / 2);
    widget->adjustSize();
}

inline QString remove_duplicate_chars(const QString& input) {
    QString result;
    QSet<QChar> seenChars;

    for (const QChar& ch : input) {
        if (!seenChars.contains(ch)) {
            seenChars.insert(ch);
            result.append(ch);
        }
    }
    return result;
}

//////////////////////////// string utils ////////////////////////////

inline QString byte2bits(uint8_t byte, bool with_prefix = true) {
    QString bits = with_prefix ? "b" : "";
    uint8_t mask = 0x80;
    while (mask) {
        bits += (byte & mask) ? '1' : '0';
        mask >>= 1;
    }
    return bits;
}

inline QString word2bits(uint16_t word, bool with_prefix = true) {
    QString str = with_prefix ? "b" : "";
    return str + byte2bits((uint8_t)(word >> 8), false) + " " + byte2bits((uint8_t)(word & 0xFF), false);
}

inline QString dword2bits(uint32_t dword, bool with_prefix = true) {
    QString str = with_prefix ? "b" : "";
    return str + word2bits((uint16_t)(dword >> 16), false) + " " + word2bits((uint16_t)(dword & 0xFFFF), false);
}

#define qdebug_qbytes(origin)                               \
    do {                                                    \
        auto arr = origin;                                  \
        char tmp[8];                                        \
        QStringList sl;                                     \
        for (int i = 0; i < arr.length(); i++) {            \
            auto c = arr[i];                                \
            snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c); \
            sl.append(tmp);                                 \
        }                                                   \
        qDebug() << sl.join(" ");                           \
    } while (0);

inline QString bytes2string(const uint8_t* buf, size_t len) {
    char tmp[8];
    QStringList sl;
    for (size_t i = 0; i < len; i++) {
        uint8_t c = buf[i];
        snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c);
        sl.append(tmp);
    }
    return sl.join(" ");
}

inline QString bytes2string(const QByteArray& arr) {
    return bytes2string((const uint8_t*)arr.constData(), arr.size());
}

inline QString hex2string(uint8_t hex, bool with0x = false) {
    char tmp[8];
    snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)hex);
    if (with0x) {
        return QString("0x%1").arg(tmp);
    }
    return QString(tmp);
}

inline QString hex2string(uint16_t hex, bool with0x = false) {
    char tmp[8];
    snprintf(tmp, sizeof(tmp), "%04X", (uint16_t)hex);
    if (with0x) {
        return QString("0x%1").arg(tmp);
    }
    return QString(tmp);
}

inline QString hex2string(uint32_t hex, bool with0x = false) {
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%08X", (uint32_t)hex);
    if (with0x) {
        return QString("0x%1").arg(tmp);
    }
    return QString(tmp);
}

inline QString hex2string(uint8_t* buf, size_t len, const char* sep = "") {
    char tmp[8];
    QStringList sl;
    for (size_t i = 0; i < len; i++) {
        uint8_t c = buf[i];
        snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c);
        sl.append(tmp);
    }
    return sl.join(QString(sep));
}

JLIBQT_NAMESPACE_END
