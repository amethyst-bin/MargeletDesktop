#include "margy/shell/margy_shell_theme.h"

#include "ui/style/style_core.h"
#include "ui/style/style_core_icon.h"
#include "ui/style/style_core_palette.h"
#include "window/themes/window_theme.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QRegularExpression>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

namespace Margy::ShellTheme {
namespace {

bool gHasShellColorCommand = false;
QString gCmdColorHex;
bool gCmdAmoled = false;

QString CacheFilePath() {
	return QDir::homePath() + u"/.cache/shell_color"_q;
}

QString AmoledFilePath() {
	return QDir::homePath() + u"/.cache/shell_amoled"_q;
}

std::optional<QColor> ParseColor(QString hex) {
	hex = hex.trimmed();
	if (hex.isEmpty()) {
		return std::nullopt;
	}
	if (!hex.startsWith('#') && (hex.length() == 6 || hex.length() == 8)) {
		hex.prepend('#');
	}
	const auto color = QColor(hex);
	return color.isValid() ? std::make_optional(color) : std::nullopt;
}

void SetColor(QLatin1String name, const QColor &c) {
	style::main_palette::setColor(
		name,
		uchar(c.red()),
		uchar(c.green()),
		uchar(c.blue()),
		uchar(c.alpha()));
}

class Watcher final : public QObject {
public:
	static Watcher &Instance() {
		static Watcher instance;
		return instance;
	}

	void start() {
		const auto file = CacheFilePath();
		const auto dir = QDir::homePath() + u"/.cache"_q;
		if (QFile::exists(file)) {
			_watcher.addPath(file);
			_lastModified = QFileInfo(file).lastModified();
		}
		if (QDir(dir).exists()) {
			_watcher.addPath(dir);
		}

		connect(&_watcher, &QFileSystemWatcher::fileChanged, this, [=](const QString &path) {
			if (!QFile::exists(path)) {
				_watcher.addPath(path);
			}
			reload();
		});

		connect(&_watcher, &QFileSystemWatcher::directoryChanged, this, [=](const QString &) {
			if (QFile::exists(file) && !_watcher.files().contains(file)) {
				_watcher.addPath(file);
			}
			reload();
		});

		connect(&_pollTimer, &QTimer::timeout, this, [=] {
			const auto info = QFileInfo(file);
			if (info.exists() && info.lastModified() != _lastModified) {
				_lastModified = info.lastModified();
				reload();
			}
		});
		_pollTimer.start(3000);
	}

private:
	Watcher() = default;

	void reload() {
		const auto color = ReadCacheColor();
		if (color) {
			const auto hex = color->name();
			if (hex != _lastColorHex) {
				_lastColorHex = hex;
				Apply(*color, CheckAmoled());
			}
		}
	}

	QFileSystemWatcher _watcher;
	QTimer _pollTimer;
	QDateTime _lastModified;
	QString _lastColorHex;
};

} // namespace

void SetCommandLineArgs(bool shellColor, const QString &colorHex, bool amoled) {
	gHasShellColorCommand = shellColor || !colorHex.isEmpty();
	gCmdColorHex = colorHex;
	gCmdAmoled = amoled;
}

bool HasShellColorCommand() {
	return gHasShellColorCommand;
}

QString IpcCommandString() {
	auto hex = gCmdColorHex;
	if (hex.isEmpty()) {
		const auto cached = ReadCacheColor();
		if (cached) {
			hex = cached->name();
		}
	}
	return u"CMD:shell_theme %1 %2"_q.arg(hex, gCmdAmoled ? u"1"_q : u"0"_q);
}

void HandleIpcCommand(const QString &commandArgs) {
	const auto parts = commandArgs.split(
		QRegularExpression(u"[ :]"_q),
		Qt::SkipEmptyParts);
	if (parts.isEmpty()) {
		return;
	}
	const auto color = ParseColor(parts[0]);
	if (!color) {
		return;
	}
	const auto amoled = (parts.size() >= 2) ? (parts[1] == u"1"_q) : CheckAmoled();
	Apply(*color, amoled);
}

std::optional<QColor> ReadCacheColor() {
	QFile file(CacheFilePath());
	if (file.open(QIODevice::ReadOnly)) {
		return ParseColor(QString::fromUtf8(file.readAll()));
	}
	return std::nullopt;
}

bool CheckAmoled() {
	return gCmdAmoled || QFile::exists(AmoledFilePath());
}

void Start() {
	if (HasShellColorCommand()) {
		const auto color = ParseColor(gCmdColorHex);
		if (color) {
			Apply(*color, gCmdAmoled);
		} else if (const auto cached = ReadCacheColor()) {
			Apply(*cached, gCmdAmoled);
		}
	} else {
		const auto cached = ReadCacheColor();
		if (cached) {
			Apply(*cached, CheckAmoled());
		}
	}
	Watcher::Instance().start();
}

void Apply(const QColor &accentColor, bool amoled) {
	if (!accentColor.isValid()) {
		return;
	}

	if (amoled && !Window::Theme::IsNightMode()) {
		Window::Theme::SetNightModeValue(true);
	}

	if (amoled) {
		const auto black = QColor(0, 0, 0);
		const auto bgOver = QColor(15, 15, 15);
		const auto bgRipple = QColor(25, 25, 25);
		const auto dialogsBgActive = QColor(22, 22, 22);

		SetColor(QLatin1String("windowBg"), black);
		SetColor(QLatin1String("windowBgOver"), bgOver);
		SetColor(QLatin1String("windowBgRipple"), bgRipple);
		SetColor(QLatin1String("dialogsBg"), black);
		SetColor(QLatin1String("dialogsBgOver"), QColor(12, 12, 12));
		SetColor(QLatin1String("dialogsBgActive"), dialogsBgActive);
		SetColor(QLatin1String("dialogsRippleBg"), bgRipple);
		SetColor(QLatin1String("dialogsRippleBgActive"), QColor(30, 30, 30));
		SetColor(QLatin1String("historyComposeAreaBg"), black);
		SetColor(QLatin1String("historyPinnedBg"), black);
		SetColor(QLatin1String("historyReplyBg"), black);
		SetColor(QLatin1String("historyComposeButtonBg"), black);
		SetColor(QLatin1String("msgInBg"), QColor(14, 14, 14));
		SetColor(QLatin1String("msgInBgSelected"), QColor(26, 26, 26));
		SetColor(QLatin1String("boxBg"), black);
		SetColor(QLatin1String("boxDividerBg"), QColor(20, 20, 20));
		SetColor(QLatin1String("menuBg"), QColor(8, 8, 8));
		SetColor(QLatin1String("menuBgOver"), QColor(20, 20, 20));
		SetColor(QLatin1String("menuBgRipple"), QColor(30, 30, 30));
		SetColor(QLatin1String("titleBg"), black);
		SetColor(QLatin1String("titleBgActive"), black);
		SetColor(QLatin1String("topBarBg"), black);
		SetColor(QLatin1String("emojiPanBg"), black);
		SetColor(QLatin1String("emojiPanCategories"), QColor(10, 10, 10));
		SetColor(QLatin1String("contactsBg"), black);
		SetColor(QLatin1String("contactsBgOver"), bgOver);
		SetColor(QLatin1String("searchedBarBg"), QColor(12, 12, 12));
		SetColor(QLatin1String("introBg"), black);
		SetColor(QLatin1String("filterInputActiveBg"), QColor(18, 18, 18));
		SetColor(QLatin1String("filterInputInactiveBg"), QColor(10, 10, 10));
	}

	SetColor(QLatin1String("windowBgActive"), accentColor);
	SetColor(QLatin1String("windowActiveTextFg"), accentColor);
	SetColor(QLatin1String("activeButtonBg"), accentColor);
	SetColor(QLatin1String("activeButtonBgOver"), accentColor.lighter(110));
	SetColor(QLatin1String("activeButtonBgRipple"), accentColor.darker(115));

	const auto luminance = (accentColor.red() * 299 + accentColor.green() * 587 + accentColor.blue() * 114) / 1000;
	const auto buttonFg = (luminance > 140) ? QColor(0, 0, 0) : QColor(255, 255, 255);
	SetColor(QLatin1String("activeButtonFg"), buttonFg);
	SetColor(QLatin1String("activeButtonFgOver"), buttonFg);

	SetColor(QLatin1String("dialogsUnreadBg"), accentColor);
	SetColor(QLatin1String("dialogsUnreadBgOver"), accentColor);
	SetColor(QLatin1String("dialogsUnreadBgMuted"), accentColor.darker(140));
	SetColor(QLatin1String("dialogsNameFgActive"), amoled ? accentColor : QColor(255, 255, 255));
	SetColor(QLatin1String("dialogsVerifiedIconBg"), accentColor);
	SetColor(QLatin1String("profileVerifiedCheckBg"), accentColor);

	SetColor(QLatin1String("historySendIconFg"), accentColor);
	SetColor(QLatin1String("historySendIconFgOver"), accentColor.lighter(110));
	SetColor(QLatin1String("historyReplyIconFg"), accentColor);
	SetColor(QLatin1String("historyLinkInFg"), accentColor);
	SetColor(QLatin1String("historyLinkOutFg"), accentColor);

	SetColor(QLatin1String("sliderBgActive"), accentColor);
	SetColor(QLatin1String("mediaPlayerActiveFg"), accentColor);
	SetColor(QLatin1String("msgFileInBg"), accentColor);
	SetColor(QLatin1String("msgWaveformInActive"), accentColor);

	style::internal::ResetIcons();
	style::NotifyPaletteChanged();
	if (Window::Theme::Background()) {
		Window::Theme::Background()->appliedEditedPalette();
	}
	for (auto widget : QApplication::allWidgets()) {
		widget->update();
	}
}

} // namespace Margy::ShellTheme
