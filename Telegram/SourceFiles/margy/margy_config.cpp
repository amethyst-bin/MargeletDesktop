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

void Config::setShowIds(bool enabled) {
	if (_showIds != enabled) {
		_showIds = enabled;
		save();
	}
}

void Config::setCopyFormatting(bool enabled) {
	if (_copyFormatting != enabled) {
		_copyFormatting = enabled;
		save();
	}
}

void Config::setStreamerMode(bool enabled) {
	if (_streamerMode != enabled) {
		_streamerMode = enabled;
		save();
	}
}

void Config::setStreamerHidesOthers(bool enabled) {
	if (_streamerHidesOthers != enabled) {
		_streamerHidesOthers = enabled;
		save();
	}
}

void Config::setStreamerHidesUsername(bool enabled) {
	if (_streamerHidesUsername != enabled) {
		_streamerHidesUsername = enabled;
		save();
	}
}

void Config::setMeowEnabled(bool enabled) {
	if (_meowEnabled != enabled) {
		_meowEnabled = enabled;
		save();
	}
}

void Config::setMeowHeard(bool heard) {
	if (_meowHeard != heard) {
		_meowHeard = heard;
		save();
	}
}

void Config::setTagsEnabled(bool enabled) {
	if (_tagsEnabled != enabled) {
		_tagsEnabled = enabled;
		save();
	}
}

void Config::setBubblesOutline(bool enabled) {
	if (_bubblesOutline != enabled) {
		_bubblesOutline = enabled;
		save();
	}
}

void Config::load() {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	_badgesEnabled = settings.value("badges/enabled", true).toBool();
	_showIds = settings.value("profile/show_ids", true).toBool();
	_copyFormatting = settings.value("general/copy_formatting", true).toBool();
	_streamerMode = settings.value("streamer/enabled", false).toBool();
	_streamerHidesOthers = settings.value("streamer/hides_others", false).toBool();
	_streamerHidesUsername = settings.value("streamer/hides_username", false).toBool();
	_meowEnabled = settings.value("sound/meow_enabled", false).toBool();
	_meowHeard = settings.value("sound/meow_heard", false).toBool();
	_tagsEnabled = settings.value("tags/enabled", true).toBool();
	_bubblesOutline = settings.value("appearance/bubbles_outline", false).toBool();
}

void Config::save() {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	settings.setValue("badges/enabled", _badgesEnabled);
	settings.setValue("profile/show_ids", _showIds);
	settings.setValue("general/copy_formatting", _copyFormatting);
	settings.setValue("streamer/enabled", _streamerMode);
	settings.setValue("streamer/hides_others", _streamerHidesOthers);
	settings.setValue("streamer/hides_username", _streamerHidesUsername);
	settings.setValue("sound/meow_enabled", _meowEnabled);
	settings.setValue("sound/meow_heard", _meowHeard);
	settings.setValue("tags/enabled", _tagsEnabled);
	settings.setValue("appearance/bubbles_outline", _bubblesOutline);
}

} // namespace Margy
