#ifndef __JLIB_QT_DARKMODE_H__
#define __JLIB_QT_DARKMODE_H__

namespace jlib {
namespace qt {

/**
 * @brief Dark mode stylesheet for Qt applications.
 *
 * This stylesheet is designed to provide a dark theme for Qt applications,
 * enhancing the user interface with a modern and visually appealing look.
 * It includes styles for various widgets such as buttons, text fields, combo boxes,
 * and more, ensuring a consistent dark theme throughout the application.
 */
static const char* dark_mode_stylesheet = R"(
QWidget {
    background-color: #333333;
}
QDialog {
    background-color: #333333;
    font-family: 'SimHei', 'Microsoft YaHei', 'Arial', sans-serif;
    font-size: 24pt;
}
QPushButton {
    background-color: #555555;
    color: #CCCCCC;
    border: 1px solid #666666;
    border-radius: 4px;
    padding: 6px 12px;
    min-width: 60px;
}
QPushButton:hover {
    background-color: #666666;
}
QPushButton:pressed {
    background-color: #777777;
}
QPushButton:disabled {
    background-color: #444444;
    color: #888888;
}
QLineEdit, QTextEdit {
    background-color: #444444;
    color: #CCCCCC;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 4px;
}
/* 新增的下拉菜单专项样式 */
QComboBox QAbstractItemView {
    background-color: #444444;    /* 下拉菜单背景色 */
    border: 1px solid #555555;    /* 边框与主界面统一 */
    border-radius: 3px;
    color: white;                 /* 文字颜色 */
    selection-background-color: #0078D4;  /* 选中项背景色 */
    selection-color: white;       /* 选中项文字颜色 */
    outline: 0;                   /* 去除虚线焦点框 */
    max-height: 200px;            /* 防止过长 */
}

/* 下拉菜单项的样式 */
QComboBox QAbstractItemView::item {
    height: 28px;                 /* 统一行高 */
    padding: 2 8px;               /* 文字缩进 */
}

/* 原有基础样式增强 */
QComboBox {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
    min-width: 30px;
}

/* 滚动条样式（保持整体风格统一） */
QComboBox QScrollBar:vertical {
    background: #333333;
    width: 10px;
}
QComboBox QScrollBar::handle:vertical {
    background: #666666;
}
QGroupBox {
    color: #AAEEAA;
    border: 1px solid #555555;
    margin-top: 10px;
    padding-top: 15px;
}
QLabel {
    color: #CCCCCC;
}
QCheckBox, QRadioButton {
    color: #CCCCCC;
}
QListWidget {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
}
QListWidgetItem {
    background-color: #444444;
    color: white;
    padding: 4px 8px;
}
QPlainTextEdit {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 4px;
}
QProgressBar {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
}
QHexView {
    background-color: #333333;
    color: #CCCCCC;
}
QHexView::header {
    background-color: #444444;
    color: #CCCCCC;
}
QHexView::address {
    background-color: #444444;
    color: #CCCCCC;
}
QHexView::hex {
    background-color: #444444;
    color: #CCCCCC;
}
QHexView::ascii {
    background-color: #444444;
    color: #CCCCCC;
}
QScrollArea {
    background-color: #333333;
    border: none;
}
QScrollBar:vertical {
    background: #333333;
    width: 12px;
    border: none;
    margin: 0px;
}
QScrollBar::handle:vertical {
    background: #666666;
    min-height: 20px;
    border-radius: 6px;
}
QScrollBar::handle:vertical:hover {
    background: #777777;
}
QScrollBar::handle:vertical:pressed {
    background: #888888;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    border: none;
    background: none;
    height: 0px;
}
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
    background: none;
}
QScrollBar:horizontal {
    background: #333333;
    height: 12px;
    border: none;
    margin: 0px;
}
QScrollBar::handle:horizontal {
    background: #666666;
    min-width: 20px;
    border-radius: 6px;
}
QScrollBar::handle:horizontal:hover {
    background: #777777;
}
QScrollBar::handle:horizontal:pressed {
    background: #888888;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    border: none;
    background: none;
    width: 0px;
}
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
    background: none;
}

/* QTabWidget 整体 */
QTabWidget {
    background-color: #2b2b2b;
    border: none;
}

QTabWidget::pane {
    border: none;
    background-color: #2b2b2b;
}

/* 标签栏（选项卡区域） */
QTabBar {
    background-color: #3c3c3c;
    padding: 0px;
    border-bottom: 1px solid #555555;
}

/* 单个标签页（未选中） */
QTabBar::tab {
    background-color: #3c3c3c;
    color: #cccccc;
    padding: 8px 20px;
    margin-right: 2px;
    border-top-left-radius: 4px;
    border-top-right-radius: 4px;
    border: 1px solid #555555;
    border-bottom: none;
    min-width: 60px;
}

/* 标签页悬停 */
QTabBar::tab:hover {
    background-color: #505050;
    color: #ffffff;
}

/* 标签页选中 */
QTabBar::tab:selected {
    background-color: #2b2b2b;
    color: #ffffff;
    border-bottom: 1px solid #2b2b2b; /* 与内容区无缝连接 */
}

/* 标签页禁用 */
QTabBar::tab:disabled {
    color: #666666;
}

/* 内容页面区域（QWidget 或 QScrollArea 等） */
QWidget#qt_tabwidget_stackedwidget {
    background-color: #2b2b2b;
    border: 1px solid #555555;
    border-top: none;
}

/* 表格视图 */
QTableWidget, QTableView {
    background-color: #444444;
    color: #CCCCCC;
    border: 1px solid #555555;
    border-radius: 3px;
    gridline-color: #555555;
    alternate-background-color: #3a3a3a;
    selection-background-color: #0078D4;
    selection-color: white;
    outline: 0;                   /* 去除虚线焦点框 */
}
QTableWidget::item, QTableView::item {
    padding: 2px 6px;
}
QTableWidget::item:hover, QTableView::item:hover {
    background-color: #505050;
}
QTableWidget::item:selected, QTableView::item:selected {
    background-color: #0078D4;
    color: white;
}

/* 列表/树视图 (覆盖 QListView 与 QTreeView; QFontDialog/QFileDialog 等系统对话框内部用) */
QListView, QTreeView {
    background-color: #444444;
    color: #CCCCCC;
    border: 1px solid #555555;
    border-radius: 3px;
    alternate-background-color: #3a3a3a;
    selection-background-color: #0078D4;
    selection-color: white;
    outline: 0;
}
QListView::item, QTreeView::item {
    padding: 2px 6px;
}
QListView::item:hover, QTreeView::item:hover {
    background-color: #505050;
}
QListView::item:selected, QTreeView::item:selected {
    background-color: #0078D4;
    color: white;
}
QTreeView::branch {
    background-color: #444444;
}

/* 数值输入 */
QSpinBox, QDoubleSpinBox, QAbstractSpinBox {
    background-color: #444444;
    color: #CCCCCC;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 2px 4px;
    selection-background-color: #0078D4;
    selection-color: white;
}
QSpinBox::up-button, QDoubleSpinBox::up-button {
    background-color: #555555;
    border: 1px solid #666666;
    border-bottom: none;
    width: 14px;
}
QSpinBox::down-button, QDoubleSpinBox::down-button {
    background-color: #555555;
    border: 1px solid #666666;
    border-top: none;
    width: 14px;
}
QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
    background-color: #666666;
}

/* 表格表头 */
QHeaderView {
    background-color: #444444;
    border: none;
}
QHeaderView::section {
    background-color: #555555;
    color: #FFFFFF;
    border: none;
    border-right: 1px solid #666666;
    border-bottom: 1px solid #666666;
    padding: 4px 8px;
    min-height: 24px;
}
QHeaderView::section:hover {
    background-color: #666666;
}
QHeaderView::section:pressed {
    background-color: #777777;
}

/* 表格左上角按钮 */
QTableCornerButton::section {
    background-color: #555555;
    border: none;
    border-right: 1px solid #666666;
    border-bottom: 1px solid #666666;
}
QTableCornerButton::section:hover {
    background-color: #666666;
}

/* 表格内滚动条视口透明，露出交替行色 */
QTableWidget QScrollBar:vertical, QTableView QScrollBar:vertical {
    background: #333333;
    width: 12px;
}
)";

// same as dark_mode_stylesheet but with white text for better contrast
static const char* dark_mode2_stylesheet = R"(
QWidget {
    background-color: #333333;
}
QDialog QFrame {
    background-color: #333333;
    font-family: 'SimHei', 'Microsoft YaHei', 'Arial', sans-serif;
    font-size: 24pt;
}
QPushButton {
    background-color: #555555;
    color: #FFFFFF;
    border: 1px solid #666666;
    border-radius: 4px;
    padding: 6px 12px;
}
QPushButton:hover {
    background-color: #666666;
}
QPushButton:pressed {
    background-color: #777777;
}
QPushButton:disabled {
    background-color: #444444;
    color: #888888;
}
QLineEdit, QTextEdit {
    background-color: #444444;
    color: #FFFFFF;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 4px;
}
/* 新增的下拉菜单专项样式 */
QComboBox QAbstractItemView {
    background-color: #444444;    /* 下拉菜单背景色 */
    border: 1px solid #555555;    /* 边框与主界面统一 */
    border-radius: 3px;
    color: white;                 /* 文字颜色 */
    selection-background-color: #0078D4;  /* 选中项背景色 */
    selection-color: white;       /* 选中项文字颜色 */
    outline: 0;                   /* 去除虚线焦点框 */
    max-height: 200px;            /* 防止过长 */
}

/* 下拉菜单项的样式 */
QComboBox QAbstractItemView::item {
    height: 28px;                 /* 统一行高 */
    padding: 2 8px;               /* 文字缩进 */
}

/* 原有基础样式增强 */
QComboBox {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
    min-width: 30px;
}

/* 滚动条样式（保持整体风格统一） */
QComboBox QScrollBar:vertical {
    background: #333333;
    width: 10px;
}
QComboBox QScrollBar::handle:vertical {
    background: #666666;
}
QGroupBox {
    color: #FFFFFF;
    border: 2px solid #555555;
    margin-top: 1ex;
    padding-top: 15px;  /* 增加顶部内边距 */
}
QGroupBox::title {
    subcontrol-origin: border;
    subcontrol-position: top left;
    left: 8px;
    top: -5px;  /* 先设为-9px，再根据实际效果微调 */
    padding: 0 5px;
}
QLabel {
    color: #FFFFFF;
}
QCheckBox, QRadioButton {
    color: #FFFFFF;
}
QListWidget {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
}
QListWidgetItem {
    background-color: #444444;
    color: white;
    padding: 4px 8px;
}
QPlainTextEdit {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 4px;
}
QProgressBar {
    background-color: #444444;
    color: white;
    border: 1px solid #555555;
    border-radius: 3px;
}
QHexView {
    background-color: #333333;
    color: #FFFFFF;
    min-width: 1030px;
    margin-top: 1ex;
    padding-top: 15px;  /* 增加顶部内边距 */
}
QHexView::header {
    background-color: #444444;
    color: #FFFFFF;
}
QHexView::address {
    background-color: #444444;
    color: #FFFFFF;
}
QHexView::hex {
    background-color: #444444;
    color: #FFFFFF;
}
QHexView::ascii {
    background-color: #444444;
    color: #FFFFFF;
}
QScrollArea {
    background-color: #333333;
    border: none;
}
QScrollBar:vertical {
    background: #333333;
    width: 12px;
    border: none;
    margin: 0px;
}
QScrollBar::handle:vertical {
    background: #666666;
    min-height: 20px;
    border-radius: 6px;
}
QScrollBar::handle:vertical:hover {
    background: #777777;
}
QScrollBar::handle:vertical:pressed {
    background: #888888;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    border: none;
    background: none;
    height: 0px;
}
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
    background: none;
}
QScrollBar:horizontal {
    background: #333333;
    height: 12px;
    border: none;
    margin: 0px;
}
QScrollBar::handle:horizontal {
    background: #666666;
    min-width: 20px;
    border-radius: 6px;
}
QScrollBar::handle:horizontal:hover {
    background: #777777;
}
QScrollBar::handle:horizontal:pressed {
    background: #888888;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    border: none;
    background: none;
    width: 0px;
}
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
    background: none;
}

/* 表格视图 */
QTableWidget, QTableView {
    background-color: #444444;
    color: #FFFFFF;
    border: 1px solid #555555;
    border-radius: 3px;
    gridline-color: #555555;
    alternate-background-color: #3a3a3a;
    selection-background-color: #0078D4;
    selection-color: white;
    outline: 0;                   /* 去除虚线焦点框 */
}
QTableWidget::item, QTableView::item {
    padding: 2px 6px;
}
QTableWidget::item:hover, QTableView::item:hover {
    background-color: #505050;
}
QTableWidget::item:selected, QTableView::item:selected {
    background-color: #0078D4;
    color: white;
}

/* 列表/树视图 */
QListView, QTreeView {
    background-color: #444444;
    color: #FFFFFF;
    border: 1px solid #555555;
    border-radius: 3px;
    alternate-background-color: #3a3a3a;
    selection-background-color: #0078D4;
    selection-color: white;
    outline: 0;
}
QListView::item, QTreeView::item {
    padding: 2px 6px;
}
QListView::item:hover, QTreeView::item:hover {
    background-color: #505050;
}
QListView::item:selected, QTreeView::item:selected {
    background-color: #0078D4;
    color: white;
}
QTreeView::branch {
    background-color: #444444;
}

/* 数值输入 */
QSpinBox, QDoubleSpinBox, QAbstractSpinBox {
    background-color: #444444;
    color: #FFFFFF;
    border: 1px solid #555555;
    border-radius: 3px;
    padding: 2px 4px;
    selection-background-color: #0078D4;
    selection-color: white;
}
QSpinBox::up-button, QDoubleSpinBox::up-button {
    background-color: #555555;
    border: 1px solid #666666;
    border-bottom: none;
    width: 14px;
}
QSpinBox::down-button, QDoubleSpinBox::down-button {
    background-color: #555555;
    border: 1px solid #666666;
    border-top: none;
    width: 14px;
}
QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
    background-color: #666666;
}

/* 表格表头 */
QHeaderView {
    background-color: #444444;
    border: none;
}
QHeaderView::section {
    background-color: #555555;
    color: #FFFFFF;
    border: none;
    border-right: 1px solid #666666;
    border-bottom: 1px solid #666666;
    padding: 4px 8px;
    min-height: 24px;
}
QHeaderView::section:hover {
    background-color: #666666;
}
QHeaderView::section:pressed {
    background-color: #777777;
}

/* 表格左上角按钮 */
QTableCornerButton::section {
    background-color: #555555;
    border: none;
    border-right: 1px solid #666666;
    border-bottom: 1px solid #666666;
}
QTableCornerButton::section:hover {
    background-color: #666666;
}

/* 表格内滚动条视口透明，露出交替行色 */
QTableWidget QScrollBar:vertical, QTableView QScrollBar:vertical {
    background: #333333;
    width: 12px;
}
)";

}  // namespace qt
}  // namespace jlib

#endif  // __JLIB_QT_DARKMODE_H__
