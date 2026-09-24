#include "margy/icons/margy_icon_packs.h"

#include "ui/style/style_core_icon.h"
#include "ui/painter.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtCore/QByteArray>
#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtSvg/QSvgRenderer>
#include <unordered_map>

namespace Margy {
namespace {

enum class IconId : uint8_t {
	Edit,
	Delete,
	Pin,
	Unpin,
	Copy,
	Forward,
	Share,
	Archive,
	Download,
	Link,
	Cancel,
	Info,
	Profile,
	Settings,
	Lock,
	Palette,
	SoundOn,
	SoundOff,
	ChatBubble,
	Stickers,
	Favorite,
	Manage,
	Count
};

std::unordered_map<const style::internal::IconMask*, IconId> MaskToId;
bool MasksInitialized = false;

const char *LucideSvg(IconId id) {
	switch (id) {
	case IconId::Edit:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M17 3a2.85 2.83 0 1 1 4 4L7.5 20.5 2 22l1.5-5.5Z\"/><path d=\"m15 5 4 4\"/></svg>";
	case IconId::Delete:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M3 6h18\"/><path d=\"M19 6v14c0 1-1 2-2 2H7c-1 0-2-1-2-2V6\"/><path d=\"M8 6V4c0-1 1-2 2-2h4c1 0 2 1 2 2v2\"/><line x1=\"10\" x2=\"10\" y1=\"11\" y2=\"17\"/><line x1=\"14\" x2=\"14\" y1=\"11\" y2=\"17\"/></svg>";
	case IconId::Pin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"12\" x2=\"12\" y1=\"17\" y2=\"22\"/><path d=\"M5 17h14v-1.76a2 2 0 0 0-1.11-1.79l-1.78-.9A2 2 0 0 1 15 10.76V6h1a2 2 0 0 0 0-4H8a2 2 0 0 0 0 4h1v4.76a2 2 0 0 1-1.11 1.79l-1.78.9A2 2 0 0 0 5 15.24Z\"/></svg>";
	case IconId::Unpin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"2\" x2=\"22\" y1=\"2\" y2=\"22\"/><line x1=\"12\" x2=\"12\" y1=\"17\" y2=\"22\"/><path d=\"M9 9v1.76a2 2 0 0 1-1.11 1.79l-1.78.9A2 2 0 0 0 5 15.24V17h12\"/><path d=\"M15 9.34V6h1a2 2 0 0 0 0-4H7.89\"/></svg>";
	case IconId::Copy:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect width=\"14\" height=\"14\" x=\"8\" y=\"8\" rx=\"2\" ry=\"2\"/><path d=\"M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2\"/></svg>";
	case IconId::Forward:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"15 14 20 9 15 4\"/><path d=\"M4 20v-7a4 4 0 0 1 4-4h12\"/></svg>";
	case IconId::Share:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"18\" cy=\"5\" r=\"3\"/><circle cx=\"6\" cy=\"12\" r=\"3\"/><circle cx=\"18\" cy=\"19\" r=\"3\"/><line x1=\"8.59\" x2=\"15.42\" y1=\"13.51\" y2=\"17.49\"/><line x1=\"15.41\" x2=\"8.59\" y1=\"6.51\" y2=\"10.49\"/></svg>";
	case IconId::Archive:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect width=\"20\" height=\"5\" x=\"2\" y=\"3\" rx=\"1\"/><path d=\"M4 8v11a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8\"/><path d=\"M10 12h4\"/></svg>";
	case IconId::Download:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4\"/><polyline points=\"7 10 12 15 17 10\"/><line x1=\"12\" x2=\"12\" y1=\"15\" y2=\"3\"/></svg>";
	case IconId::Link:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M10 13a5 5 0 0 0 7.54.54l3-3a5 5 0 0 0-7.07-7.07l-1.72 1.71\"/><path d=\"M14 11a5 5 0 0 0-7.54-.54l-3 3a5 5 0 0 0 7.07 7.07l1.71-1.71\"/></svg>";
	case IconId::Cancel:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"18\" x2=\"6\" y1=\"6\" y2=\"18\"/><line x1=\"6\" x2=\"18\" y1=\"6\" y2=\"18\"/></svg>";
	case IconId::Info:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"12\" cy=\"12\" r=\"10\"/><line x1=\"12\" x2=\"12\" y1=\"16\" y2=\"12\"/><line x1=\"12\" x2=\"12.01\" y1=\"8\" y2=\"8\"/></svg>";
	case IconId::Profile:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M19 21v-2a4 4 0 0 0-4-4H9a4 4 0 0 0-4 4v2\"/><circle cx=\"12\" cy=\"7\" r=\"4\"/></svg>";
	case IconId::Settings:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z\"/><circle cx=\"12\" cy=\"12\" r=\"3\"/></svg>";
	case IconId::Lock:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect width=\"18\" height=\"11\" x=\"3\" y=\"11\" rx=\"2\" ry=\"2\"/><path d=\"M7 11V7a5 5 0 0 1 10 0v4\"/></svg>";
	case IconId::Palette:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"13.5\" cy=\"6.5\" r=\".5\" fill=\"#fff\"/><circle cx=\"17.5\" cy=\"10.5\" r=\".5\" fill=\"#fff\"/><circle cx=\"8.5\" cy=\"7.5\" r=\".5\" fill=\"#fff\"/><circle cx=\"6.5\" cy=\"12.5\" r=\".5\" fill=\"#fff\"/><path d=\"M12 2C6.5 2 2 6.5 2 12s4.5 10 10 10c.926 0 1.648-.746 1.648-1.688 0-.437-.18-.835-.437-1.125-.29-.289-.438-.652-.438-1.125a1.64 1.64 0 0 1 1.668-1.668h1.996c3.051 0 5.555-2.503 5.555-5.554C21.965 6.012 17.461 2 12 2z\"/></svg>";
	case IconId::SoundOn:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polygon points=\"11 5 6 9 2 9 2 15 6 15 11 19 11 5\"/><path d=\"M15.54 8.46a5 5 0 0 1 0 7.07\"/><path d=\"M19.07 4.93a10 10 0 0 1 0 14.14\"/></svg>";
	case IconId::SoundOff:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polygon points=\"11 5 6 9 2 9 2 15 6 15 11 19 11 5\"/><line x1=\"22\" x2=\"16\" y1=\"9\" y2=\"15\"/><line x1=\"16\" x2=\"22\" y1=\"9\" y2=\"15\"/></svg>";
	case IconId::ChatBubble:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z\"/></svg>";
	case IconId::Stickers:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"12\" cy=\"12\" r=\"10\"/><path d=\"M8 14s1.5 2 4 2 4-2 4-2\"/><line x1=\"9\" x2=\"9.01\" y1=\"9\" y2=\"9\"/><line x1=\"15\" x2=\"15.01\" y1=\"9\" y2=\"9\"/></svg>";
	case IconId::Favorite:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polygon points=\"12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2\"/></svg>";
	case IconId::Manage:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"4\" x2=\"4\" y1=\"21\" y2=\"14\"/><line x1=\"4\" x2=\"4\" y1=\"10\" y2=\"3\"/><line x1=\"12\" x2=\"12\" y1=\"21\" y2=\"12\"/><line x1=\"12\" x2=\"12\" y1=\"8\" y2=\"3\"/><line x1=\"20\" x2=\"20\" y1=\"21\" y2=\"16\"/><line x1=\"20\" x2=\"20\" y1=\"12\" y2=\"3\"/><line x1=\"1\" x2=\"7\" y1=\"14\" y2=\"14\"/><line x1=\"9\" x2=\"15\" y1=\"8\" y2=\"8\"/><line x1=\"17\" x2=\"23\" y1=\"16\" y2=\"16\"/></svg>";
	default:
		return nullptr;
	}
}

const char *TablerSvg(IconId id) {
	switch (id) {
	case IconId::Edit:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M4 20h4l10.5 -10.5a2.828 2.828 0 1 0 -4 -4l-10.5 10.5v4\" /><path d=\"M13.5 6.5l4 4\" /></svg>";
	case IconId::Delete:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M4 7l16 0\" /><path d=\"M10 11l0 6\" /><path d=\"M14 11l0 6\" /><path d=\"M5 7l1 12a2 2 0 0 0 2 2h8a2 2 0 0 0 2 -2l1 -12\" /><path d=\"M9 7v-3a1 1 0 0 1 1 -1h4a1 1 0 0 1 1 1v3\" /></svg>";
	case IconId::Pin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M9 4v6l-2 4v2h10v-2l-2 -4v-6\" /><path d=\"M12 16l0 5\" /><path d=\"M8 4l8 0\" /></svg>";
	case IconId::Unpin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M3 3l18 18\" /><path d=\"M15 4.5l-3.249 3.249m-2.57 1.433l-2.181 4.318h5l2 4l1.5 -1.5l-4 -2\" /><path d=\"M9 4.5l6 6\" /></svg>";
	case IconId::Copy:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M8 8m0 2a2 2 0 0 1 2 -2h8a2 2 0 0 1 2 2v8a2 2 0 0 1 -2 2h-8a2 2 0 0 1 -2 -2z\" /><path d=\"M16 8v-2a2 2 0 0 0 -2 -2h-8a2 2 0 0 0 -2 2v8a2 2 0 0 0 2 2h2\" /></svg>";
	case IconId::Forward:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M15 14l5 -5l-5 -5\" /><path d=\"M4 20v-7a4 4 0 0 1 4 -4h12\" /></svg>";
	case IconId::Share:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M6 12m-3 0a3 3 0 1 0 6 0a3 3 0 1 0 -6 0\" /><path d=\"M18 6m-3 0a3 3 0 1 0 6 0a3 3 0 1 0 -6 0\" /><path d=\"M18 18m-3 0a3 3 0 1 0 6 0a3 3 0 1 0 -6 0\" /><path d=\"M8.7 10.7l6.6 -3.4\" /><path d=\"M8.7 13.3l6.6 3.4\" /></svg>";
	case IconId::Archive:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M3 4m0 2a2 2 0 0 1 2 -2h14a2 2 0 0 1 2 2v0a2 2 0 0 1 -2 2h-14a2 2 0 0 1 -2 -2z\" /><path d=\"M5 8v10a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-10\" /><path d=\"M10 12l4 0\" /></svg>";
	case IconId::Download:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M4 17v2a2 2 0 0 0 2 2h12a2 2 0 0 0 2 -2v-2\" /><path d=\"M7 11l5 5l5 -5\" /><path d=\"M12 4l0 12\" /></svg>";
	case IconId::Link:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M9 15l6 -6\" /><path d=\"M11 6l.463 -.536a5 5 0 0 1 7.071 7.072l-.534 .464\" /><path d=\"M13 18l-.397 .534a5.068 5.068 0 0 1 -7.127 0a4.972 4.972 0 0 1 0 -7.071l.524 -.463\" /></svg>";
	case IconId::Cancel:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M18 6l-12 12\" /><path d=\"M6 6l12 12\" /></svg>";
	case IconId::Info:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M12 12m-9 0a9 9 0 1 0 18 0a9 9 0 1 0 -18 0\" /><path d=\"M12 8l.01 0\" /><path d=\"M11 12l1 0l0 4l1 0\" /></svg>";
	case IconId::Profile:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M8 7a4 4 0 1 0 8 0a4 4 0 0 0 -8 0\" /><path d=\"M6 21v-2a4 4 0 0 1 4 -4h4a4 4 0 0 1 4 4v2\" /></svg>";
	case IconId::Settings:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M10.325 4.317c.426 -1.756 2.924 -1.756 3.35 0a1.724 1.724 0 0 0 2.573 1.066c1.543 -.94 3.31 .826 2.37 2.37a1.724 1.724 0 0 0 1.065 2.572c1.756 .426 1.756 2.924 0 3.35a1.724 1.724 0 0 0 -1.066 2.573c.94 1.543 -.826 3.31 -2.37 2.37a1.724 1.724 0 0 0 -2.572 1.065c-.426 1.756 -2.924 1.756 -3.35 0a1.724 1.724 0 0 0 -2.573 -1.066c-1.543 .94 -3.31 -.826 -2.37 -2.37a1.724 1.724 0 0 0 -1.065 -2.572c-1.756 -.426 -1.756 -2.924 0 -3.35a1.724 1.724 0 0 0 1.066 -2.573c-.94 -1.543 .826 -3.31 2.37 -2.37c1 .608 2.296 .07 2.572 -1.065z\" /><path d=\"M9 12a3 3 0 1 0 6 0a3 3 0 0 0 -6 0\" /></svg>";
	case IconId::Lock:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M5 11m0 2a2 2 0 0 1 2 -2h10a2 2 0 0 1 2 2v6a2 2 0 0 1 -2 2h-10a2 2 0 0 1 -2 -2z\" /><path d=\"M8 11v-4a4 4 0 0 1 8 0v4\" /></svg>";
	case IconId::Palette:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M12 21a9 9 0 0 1 0 -18c4.97 0 9 3.582 9 8c0 1.06 -.474 2.078 -1.318 2.828c-.844 .75 -1.989 1.172 -3.182 1.172h-2.5a2 2 0 0 0 -1 3.75a1.3 1.3 0 0 1 -1 2.25\" /><path d=\"M8.5 10.5m-1 0a1 1 0 1 0 2 0a1 1 0 1 0 -2 0\" /><path d=\"M12.5 7.5m-1 0a1 1 0 1 0 2 0a1 1 0 1 0 -2 0\" /><path d=\"M16.5 10.5m-1 0a1 1 0 1 0 2 0a1 1 0 1 0 -2 0\" /></svg>";
	case IconId::SoundOn:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M15 8a5 5 0 0 1 0 8\" /><path d=\"M17.7 5a9 9 0 0 1 0 14\" /><path d=\"M6 15h-2a1 1 0 0 1 -1 -1v-4a1 1 0 0 1 1 -1h2l3.5 -4.5a.8 .8 0 0 1 1.5 .5v14a.8 .8 0 0 1 -1.5 .5l-3.5 -4.5\" /></svg>";
	case IconId::SoundOff:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M6 15h-2a1 1 0 0 1 -1 -1v-4a1 1 0 0 1 1 -1h2l3.5 -4.5a.8 .8 0 0 1 1.5 .5v14a.8 .8 0 0 1 -1.5 .5l-3.5 -4.5\" /><path d=\"M16 10l4 4m0 -4l-4 4\" /></svg>";
	case IconId::ChatBubble:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M3 20l1.3 -3.9a9 8 0 1 1 3.4 2.9l-4.7 1\" /></svg>";
	case IconId::Stickers:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M12 12m-9 0a9 9 0 1 0 18 0a9 9 0 1 0 -18 0\" /><path d=\"M9 10l.01 0\" /><path d=\"M15 10l.01 0\" /><path d=\"M9.5 15a3.5 3.5 0 0 0 5 0\" /></svg>";
	case IconId::Favorite:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M12 17.75l-6.172 3.245l1.179 -6.873l-5 -4.867l6.9 -1l3.086 -6.253l3.086 6.253l6.9 1l-5 4.867l1.179 6.873z\" /></svg>";
	case IconId::Manage:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M4 10a2 2 0 1 0 4 0a2 2 0 0 0 -4 0\" /><path d=\"M6 4v4\" /><path d=\"M6 12v8\" /><path d=\"M10 16a2 2 0 1 0 4 0a2 2 0 0 0 -4 0\" /><path d=\"M12 4v10\" /><path d=\"M12 18v2\" /><path d=\"M16 7a2 2 0 1 0 4 0a2 2 0 0 0 -4 0\" /><path d=\"M18 4v1\" /><path d=\"M18 9v11\" /></svg>";
	default:
		return nullptr;
	}
}

const char *PhosphorSvg(IconId id) {
	switch (id) {
	case IconId::Edit:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M92.7,216H48a8,8,0,0,1-8-8V163.3a7.9,7.9,0,0,1,2.3-5.6l120-120a8,8,0,0,1,11.4,0l44.7,44.7a8,8,0,0,1,0,11.4l-120,120A7.9,7.9,0,0,1,92.7,216Z\"/><line x1=\"136\" y1=\"64\" x2=\"192\" y2=\"120\"/></svg>";
	case IconId::Delete:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"216\" y1=\"56\" x2=\"40\" y2=\"56\"/><line x1=\"104\" y1=\"104\" x2=\"104\" y2=\"168\"/><line x1=\"152\" y1=\"104\" x2=\"152\" y2=\"168\"/><path d=\"M200,56V208a8,8,0,0,1-8,8H64a8,8,0,0,1-8-8V56\"/><path d=\"M168,56V40a16,16,0,0,0-16-16H104A16,16,0,0,0,88,40V56\"/></svg>";
	case IconId::Pin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"128\" y1=\"176\" x2=\"128\" y2=\"240\"/><path d=\"M184,88a56,56,0,0,0-112,0v48L48,160v16H208V160l-24-24Z\"/></svg>";
	case IconId::Unpin:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"48\" y1=\"40\" x2=\"208\" y2=\"216\"/><line x1=\"128\" y1=\"176\" x2=\"128\" y2=\"240\"/><path d=\"M72,136v40H208V160l-24-24V88a56,56,0,0,0-20.5-43.1\"/></svg>";
	case IconId::Copy:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect x=\"40\" y=\"40\" width=\"128\" height=\"128\" rx=\"8\"/><path d=\"M88,168v40a8,8,0,0,0,8,8H208a8,8,0,0,0,8-8V96a8,8,0,0,0-8-8H168\"/></svg>";
	case IconId::Forward:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"160 56 216 112 160 168\"/><path d=\"M40,200a88,88,0,0,1,88-88h88\"/></svg>";
	case IconId::Share:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"64\" cy=\"128\" r=\"28\"/><circle cx=\"192\" cy=\"64\" r=\"28\"/><circle cx=\"192\" cy=\"192\" r=\"28\"/><line x1=\"89.1\" y1=\"114.5\" x2=\"166.9\" y2=\"77.5\"/><line x1=\"89.1\" y1=\"141.5\" x2=\"166.9\" y2=\"178.5\"/></svg>";
	case IconId::Archive:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect x=\"32\" y=\"48\" width=\"192\" height=\"48\" rx=\"8\"/><path d=\"M48,96V200a8,8,0,0,0,8,8H200a8,8,0,0,0,8-8V96\"/><line x1=\"104\" y1=\"144\" x2=\"152\" y2=\"144\"/></svg>";
	case IconId::Download:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"80 144 128 192 176 144\"/><line x1=\"128\" y1=\"40\" x2=\"128\" y2=\"192\"/><path d=\"M216,144v64a8,8,0,0,1-8,8H48a8,8,0,0,1-8-8V144\"/></svg>";
	case IconId::Link:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M136,120,96,160a40,40,0,1,1-56.6-56.6l40-40A40,40,0,0,1,136,63.4\"/><path d=\"M120,136l40-40a40,40,0,1,1,56.6,56.6l-40,40A40,40,0,0,1,120,192.6\"/></svg>";
	case IconId::Cancel:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"200\" y1=\"56\" x2=\"56\" y2=\"200\"/><line x1=\"200\" y1=\"200\" x2=\"56\" y2=\"56\"/></svg>";
	case IconId::Info:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"128\" cy=\"128\" r=\"96\"/><line x1=\"128\" y1=\"120\" x2=\"128\" y2=\"176\"/><circle cx=\"128\" cy=\"88\" r=\"10\" fill=\"#fff\"/></svg>";
	case IconId::Profile:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"128\" cy=\"96\" r=\"48\"/><path d=\"M40,216a88,88,0,0,1,176,0\"/></svg>";
	case IconId::Settings:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"128\" cy=\"128\" r=\"40\"/><path d=\"M128,40a88,88,0,0,1,88,88v8a80,80,0,0,1-160,0v-8A88,88,0,0,1,128,40Z\"/></svg>";
	case IconId::Lock:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><rect x=\"40\" y=\"88\" width=\"176\" height=\"128\" rx=\"8\"/><path d=\"M88,88V56a40,40,0,0,1,80,0V88\"/></svg>";
	case IconId::Palette:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M128,24A104,104,0,0,0,24,128c0,42.7,27.1,81.4,68,97.2a16.2,16.2,0,0,0,21.6-11.4l6.4-25.5a16.2,16.2,0,0,1,15.6-12.3H160a72,72,0,0,0,72-72C232,59.5,185.3,24,128,24Z\"/><circle cx=\"92\" cy=\"116\" r=\"12\" fill=\"#fff\"/><circle cx=\"132\" cy=\"84\" r=\"12\" fill=\"#fff\"/><circle cx=\"172\" cy=\"116\" r=\"12\" fill=\"#fff\"/></svg>";
	case IconId::SoundOn:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M152,40V216L80,160H32a8,8,0,0,1-8-8V104a8,8,0,0,1,8-8H80Z\"/><path d=\"M192,80a64,64,0,0,1,0,96\"/><path d=\"M224,48a112,112,0,0,1,0,160\"/></svg>";
	case IconId::SoundOff:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M152,40V216L80,160H32a8,8,0,0,1-8-8V104a8,8,0,0,1,8-8H80Z\"/><line x1=\"216\" y1=\"104\" x2=\"184\" y2=\"152\"/><line x1=\"216\" y1=\"152\" x2=\"184\" y2=\"104\"/></svg>";
	case IconId::ChatBubble:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M216,48H40a8,8,0,0,0-8,8V184a8,8,0,0,0,8,8H72v36a8,8,0,0,0,13.7,5.7L133.7,192H216a8,8,0,0,0,8-8V56A8,8,0,0,0,216,48Z\"/></svg>";
	case IconId::Stickers:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"128\" cy=\"128\" r=\"96\"/><circle cx=\"92\" cy=\"108\" r=\"12\" fill=\"#fff\"/><circle cx=\"164\" cy=\"108\" r=\"12\" fill=\"#fff\"/><path d=\"M169.6,152a48.1,48.1,0,0,1-83.2,0\"/></svg>";
	case IconId::Favorite:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><path d=\"M132.4,190.2l56.8,33.5a5.2,5.2,0,0,0,7.7-5.6l-15.1-64.4,49.8-43.4a5.2,5.2,0,0,0-3-9.1l-65.7-5.5L135.2,34.8a5.2,5.2,0,0,0-9.6,0L98.1,95.7,32.4,101.2a5.2,5.2,0,0,0-3,9.1l49.8,43.4L64.1,218.1a5.2,5.2,0,0,0,7.7,5.6l56.8-33.5A5.4,5.4,0,0,1,132.4,190.2Z\"/></svg>";
	case IconId::Manage:
		return "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 256 256\" fill=\"none\" stroke=\"#ffffff\" stroke-width=\"18\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"40\" y1=\"128\" x2=\"88\" y2=\"128\"/><circle cx=\"104\" cy=\"128\" r=\"16\"/><line x1=\"120\" y1=\"128\" x2=\"216\" y2=\"128\"/><line x1=\"40\" y1=\"64\" x2=\"152\" y2=\"64\"/><circle cx=\"168\" cy=\"64\" r=\"16\"/><line x1=\"184\" y1=\"64\" x2=\"216\" y2=\"64\"/><line x1=\"40\" y1=\"192\" x2=\"56\" y2=\"192\"/><circle cx=\"72\" cy=\"192\" r=\"16\"/><line x1=\"88\" y1=\"192\" x2=\"216\" y2=\"192\"/></svg>";
	default:
		return nullptr;
	}
}

void RegisterMask(const style::icon &icon, IconId id) {
	if (const auto m = icon.mask()) {
		MaskToId[m] = id;
	}
}

void EnsureMasks() {
	if (MasksInitialized) {
		return;
	}
	MasksInitialized = true;

	RegisterMask(st::menuIconEdit, IconId::Edit);
	RegisterMask(st::menuIconDelete, IconId::Delete);
	RegisterMask(st::menuIconPin, IconId::Pin);
	RegisterMask(st::menuIconUnpin, IconId::Unpin);
	RegisterMask(st::menuIconCopy, IconId::Copy);
	RegisterMask(st::menuIconForward, IconId::Forward);
	RegisterMask(st::menuIconShare, IconId::Share);
	RegisterMask(st::menuIconArchive, IconId::Archive);
	RegisterMask(st::menuIconDownload, IconId::Download);
	RegisterMask(st::menuIconLink, IconId::Link);
	RegisterMask(st::menuIconCancel, IconId::Cancel);
	RegisterMask(st::menuIconInfo, IconId::Info);
	RegisterMask(st::menuIconProfile, IconId::Profile);
	RegisterMask(st::menuIconSettings, IconId::Settings);
	RegisterMask(st::menuIcon2SV, IconId::Lock);
	RegisterMask(st::menuIconChangeColors, IconId::Palette);
	RegisterMask(st::menuIconPalette, IconId::Palette);
	RegisterMask(st::menuIconSoundOn, IconId::SoundOn);
	RegisterMask(st::menuIconSoundOff, IconId::SoundOff);
	RegisterMask(st::menuIconChatBubble, IconId::ChatBubble);
	RegisterMask(st::menuIconStickers, IconId::Stickers);
	RegisterMask(st::menuIconFave, IconId::Favorite);
	RegisterMask(st::menuIconManage, IconId::Manage);
}

} // namespace

IconPacks &IconPacks::Instance() {
	static IconPacks instance;
	return instance;
}

IconPacks::IconPacks() {
	init();
}

void IconPacks::init() {
	if (_initialized) {
		return;
	}
	_initialized = true;
	style::internal::SetIconMaskOverrideHook(&IconPacks::LookupHook);
}

IconPack IconPacks::currentPack() const {
	return Config::Instance().iconPack();
}

void IconPacks::setPack(IconPack pack) {
	Config::Instance().setIconPack(pack);
	style::internal::ResetIcons();
	for (const auto widget : QGuiApplication::topLevelWindows()) {
		widget->requestUpdate();
	}
	for (const auto widget : QApplication::topLevelWidgets()) {
		widget->update();
	}
}

QImage IconPacks::LookupHook(const style::internal::IconMask *mask, int scale, int ratio) {
	const auto pack = Config::Instance().iconPack();
	if (pack == IconPack::Default || !mask) {
		return QImage();
	}

	EnsureMasks();

	const auto it = MaskToId.find(mask);
	if (it == MaskToId.end()) {
		return QImage();
	}

	const char *svgStr = nullptr;
	if (pack == IconPack::Lucide) {
		svgStr = LucideSvg(it->second);
	} else if (pack == IconPack::Tabler) {
		svgStr = TablerSvg(it->second);
	} else if (pack == IconPack::Phosphor) {
		svgStr = PhosphorSvg(it->second);
	}

	if (!svgStr) {
		return QImage();
	}

	return IconPacks::Instance().renderSvg(QByteArray(svgStr), scale, ratio);
}

QImage IconPacks::renderSvg(const QByteArray &svg, int scale, int ratio) const {
	QSvgRenderer renderer(svg);
	if (!renderer.isValid()) {
		return QImage();
	}

	const auto baseSize = 24;
	const auto scaledW = style::ConvertScale(baseSize, scale);
	const auto scaledH = style::ConvertScale(baseSize, scale);

	auto img = QImage(
		QSize(scaledW, scaledH) * ratio,
		QImage::Format_ARGB32_Premultiplied);
	img.fill(Qt::transparent);
	img.setDevicePixelRatio(ratio);

	auto p = QPainter(&img);
	auto hq = PainterHighQualityEnabler(p);
	renderer.render(&p, QRectF(0, 0, scaledW, scaledH));

	return img;
}

} // namespace Margy
