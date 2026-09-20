#include "margy/margy_config.h"
#include "margy/badges/margy_badge_manager.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDir>

namespace Margy {
namespace {

constexpr auto kSettingsFileName = "margy_settings.ini";

QString SettingsFilePath() {
	const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir().mkpath(dir);
	return dir + '/' + kSettingsFileName;
}

} // namespace

Config &Config::Instance() {
	static Config instance;
	return instance;
}

Config::Config() {
	load();
}

QString Config::version() const {
	return "0.1.0-alpha";
}

void Config::setBadgesEnabled(bool enabled) {
	if (_badgesEnabled != enabled) {
		_badgesEnabled = enabled;
		save();
		Badges::Manager::Instance().setEnabled(enabled);
	}
}

void Config::load() {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	_badgesEnabled = settings.value("badges/enabled", true).toBool();
}

void Config::save() {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	settings.setValue("badges/enabled", _badgesEnabled);
}

} // namespace Margy
