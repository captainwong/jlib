#pragma once

#include "qt_global.h"
#include <QString>
#include <QColor>

JLIBQT_NAMESPACE_BEGIN

namespace def_sizes {

static const int window_width = 646;
static const int window_height = 424;

static const int title_height = 54;

static const int content_height = window_height - title_height;
static const int control_height = 24;

static const int big_window_width = 1022;
static const int big_window_height = 670;
static const int big_content_height = big_window_height - title_height;

static const int device_view_width = 306;
static const int device_view_height = 236;

} // namespace def_sizes


namespace def_colors {

// 0x202224
static const auto window_bk = QColor(0x202224);
// 0x2c2d30
static const auto control_bk = QColor(0x2c2d30);
// 0x26272a
static const auto control_bk2 = QColor(0x26272a);
// 0x313235
static const auto control_bk_suspend = QColor(0x313235);
// 0x424446
static const auto control_bk_unknown = QColor(0x424446);
// 0xd7d5d5
static const auto unknown_trans = QColor(0xd7d5d5);
// 0xfd8b30
static const auto control_text_font = QColor(0xfd8b30);

// 0xb0afaf
static const auto icon_text_color = QColor(0xb0afaf);

// 0xfe3e3e
static const auto hint_color = QColor(0xfe3e3e);
// 0xa8a9a9
static const auto edit_color = QColor(0xa8a9a9);

// 0x626466
static const auto bg_normal = QColor(0x626466);
// 0x727476
static const auto bg_suspend = QColor(0x727476);
// 0xbbbbbb
static const auto font_normal = QColor(0xbbbbbb);
// 0xf1f1f1
static const auto font_suspend = QColor(0xf1f1f1);
// 0x838383
static const auto font_normal2 = QColor(0x838383);
// 0xaaaaaa
static const auto font_normal3 = QColor(0xaaaaaa);
// 0x929292
static const auto font_normal4 = QColor(0x929292);

// 0x1a1a1a
static const auto ptz_border = QColor(0x1a1a1a);
// 0x2d2e31
static const auto ptz_back = QColor(0x2d2e31);

} // namespace def_colors


namespace def_font_families {

static const char* const yahei = "Microsoft YaHei";

} // namespace def_font_families


inline QString color_value_to_string(int value) {
	auto s = QString::number(value, 16);
	if (s.length() == 1) { s.insert(0, '0'); }
	return s;
}

inline QString color_to_string_16(QColor color) {
	return color_value_to_string(color.red()) + color_value_to_string(color.green()) + color_value_to_string(color.blue());
}

// background:[style];
inline QString build_bg_style(QString style = "transparent") {
	return QString("background:") + style + QString(";");
}

// color:#[color];
inline QString build_color_style(QColor color) {
	return QString("color:#") + color_to_string_16(color) + QString(";");
}

// background-color:#[color];
inline QString build_bgcolor_style(QColor bgcolor) {
	return QString("background-color:#") + color_to_string_16(bgcolor) + QString(";");
}

// background-color:rgba([r],[g],[b],[a]%);
inline QString build_bgcolor_style(int r, int g, int b, int a) {
	return QString("background-color:rgba(%1,%2,%3,%4%);").arg(r).arg(g).arg(b).arg(a);
}

// border:[border]px;
inline QString build_border_style(size_t border) {
	return QString("border:") + QString::number(border) + QString("px;");
}

// font:[font_size]pt;font-family:[font_family];
inline QString build_font_style(size_t font_size, QString font_family = def_font_families::yahei) {
	return QString("font:") + QString::number(font_size) + QString("pt;")
		+ QString("font-family:") + font_family + QString(";");
}

// color:#[color];font:[font_size]pt;font-family:[font_family];
inline QString build_style(QColor color, size_t font_size, QString font_family = def_font_families::yahei) {
	return build_color_style(color)	+ build_font_style(font_size, font_family);
}

// background-color:#[color];color:#[color];font:[font_size]pt;font-family:[font_family];[border:[border]px;]
inline QString build_style(QColor bgcolor, QColor color, size_t font_size, 
						   QString font_family = def_font_families::yahei, 
						   bool with_border = false, size_t border = 0) {
	auto style = build_bgcolor_style(bgcolor) + build_style(color, font_size, font_family);
	if (with_border) { style += build_border_style(border); }
	return style;
}

// background-color:rgba([r],[g],[b],[a]%);color:#[color];font:[font_size]pt;font-family:[font_family];
inline QString build_style(int r, int g, int b, int a, QColor color, size_t font_size,
						   QString font_family = def_font_families::yahei) {
	return build_bgcolor_style(r, g, b, a) + build_style(color, font_size, font_family);
}


namespace def_style_sheets {

// color:#ffffff;font:11pt;font-family:Microsoft YaHei;
inline QString label() {
	return build_style(Qt::white, 11);
}

// color:#fe3e3e;font:9pt;font-family:Microsoft YaHei;
inline QString hint() {
	return build_style(def_colors::hint_color, 9);
}

// color:#202224;font:12pt;font-family:Microsoft YaHei;border:0px;
inline QString edit() {
	return build_style(def_colors::window_bk, def_colors::edit_color, 12, def_font_families::yahei, true);
}

// color:#424446;font:11pt;font-family:Microsoft YaHei;border:0px;
inline QString unknown() {
	return build_style(def_colors::control_bk_unknown, def_colors::edit_color, 11, def_font_families::yahei, true);
}

// background:transparent;color:#d7d5d5;font:18pt;font-family:Microsoft YaHei;
inline QString unkonwn_trans() {
	return build_bg_style() + build_style(def_colors::unknown_trans, 18);
}

static const auto vertical_scroll_bar = R"(
QScrollBar:vertical
{
	width:8px;
	background:rgba(0,0,0,0%);
	margin:0px,0px,0px,0px;
	padding-top:0px;
	padding-bottom:0px;
}

QScrollBar::handle:vertical
{
	width:8px;
	background:#444547;
	border-radius:4px;
	min-height:20;
}

QScrollBar::handle:vertical:hover
{
	width:8px;
	background:#444547;
	border-radius:4px;
	min-height:20;
}

QScrollBar::add-line:vertical
{
	height:9px;
	width:8px;
	border-image:url(:/images/a/3.png);
	subcontrol-position:bottom;
}

QScrollBar::sub-line:vertical
{
	height:9px;
	width:8px;
	border-image:url(:/images/a/1.png);
	subcontrol-position:top;
}

QScrollBar::add-line:vertical:hover
{
	height:9px;
	width:8px;
	border-image:url(:/images/a/4.png);
	subcontrol-position:bottom;
}

QScrollBar::sub-line:vertical:hover
{
	height:9px;
	width:8px;
	border-image:url(:/images/2.png);
	subcontrol-positino:top;
}

QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical
{
	background:rgba(0,0,0,10%);
	border-radius:4px;
}

)";

static const auto search_bar_edit = R"(
border: 0px;
background: transparent;
selection-background-color: white;
color: #a8a9a9;
font-family: Microsoft Yahei;
font-size: 14px;
)";

static const auto progress_bar = R"(
QProgressBar{
	background-color:#dedede;border-radius:3px;
}
QProgressBar::chunk{
	background-color:#fd8b30;border-radius:3px;
}
)";

static const auto slider = R"(
QSlider::groove:horizontal
{
	border:0px;
	height:4px;
	background:rgb(0,0,0);
}
QSlider::sub-page:horizontal
{
	background:#fd8b30;
}
QSlider::add-page:horizontal
{
	background:#141414;
}
QSlider::handle:horizontal
{
	background:white;
	width:10px;
	border-radius:5px;
	margin:-3px 0px -3px 0px;
}
)";

static const auto menu = R"(
QMenu::item 
{
	font-family: Microsoft Yahei; 
	font-size: 12pt; 
	color: #808080; 
	border: 1px solid gray; 
	background-color: rgb(234,234,234);
}
QMenu::item:hover 
{
	background-color: rgb(0, 0, 255);
}
)";

static const auto group_box = R"(
QGroupBox {
    border: 1px solid gray;
    border-radius: 9px;
    margin-top: 0.5em;
	font: 24pt;
	font-family: Microsoft Yahei;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 20px;
    padding: 0 3px 0 3px;
	color: white;
}
)";

static const auto listWidgetStyleSheet = R"(
QListWidget 
{ 
	font:12pt; 
	font-family:Microsoft Yahei; 
	background-color: #2c2d30; 
	border: 1px solid black; 
} 

QListWidget::item 
{ 
	color:#fd8b30; 
	border: 1px solid silver;
} 

QListWidget::item::selected 
{ 
	color:#fd8b30; 
	border: 3px solid yellow;
}
)";


static const auto check_box = R"(
QCheckBox
{
	font-size: 16px;
	color: white;
}
QCheckBox::indicator
{
	width: 48px;
	height: 48px;
}
QCheckBox::indicator:checked
{
	image: url(:/Skin/checkbox/checkbox_checked.png);
}
QCheckBox::indicator:unchecked
{
	image: url(:/Skin/checkbox/checkbox_unchecked.png);
}
)";


static const auto check_box_48px = R"(
QCheckBox
{
	font-size: 16px;
	color: white;
}
QCheckBox::indicator
{
	width: 48px;
	height: 48px;
}
QCheckBox::indicator:checked
{
	image: url(:/Skin/checkbox/checked_checkbox_48px.png);
}
QCheckBox::indicator:unchecked
{
	image: url(:/Skin/checkbox/unchecked_checkbox_48px.png);
}
)";

} // end of namespace def_style_sheets


JLIBQT_NAMESPACE_END
