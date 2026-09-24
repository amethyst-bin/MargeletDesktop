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

QString Config::settingsFilePath() const {
	return SettingsFilePath();
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

void Config::setCustomFont(const QString &font) {
	if (_customFont != font) {
		_customFont = font;
		save();
	}
}

void Config::setEmojiFont(const QString &font) {
	if (_emojiFont != font) {
		_emojiFont = font;
		save();
	}
}

void Config::setIconPack(IconPack pack) {
	if (_iconPack != pack) {
		_iconPack = pack;
		save();
	}
}

void Config::setProfileGradient(const QString &gradient) {
	if (_profileGradient != gradient) {
		_profileGradient = gradient;
		save();
	}
}

void Config::setProfileGradientEnabled(bool enabled) {
	if (_profileGradientEnabled != enabled) {
		_profileGradientEnabled = enabled;
		save();
	}
}

void Config::setUnhideGifts(bool enabled) {
	if (_unhideGifts != enabled) {
		_unhideGifts = enabled;
		save();
	}
}

void Config::setSeizureMode(bool enabled) {
	if (_seizureMode != enabled) {
		_seizureMode = enabled;
		save();
	}
}

void Config::setPinChannelFirst(bool enabled) {
	if (_pinChannelFirst != enabled) {
		_pinChannelFirst = enabled;
		save();
	}
}

void Config::setOwnBubblesGradient(bool enabled) {
	if (_ownBubblesGradient != enabled) {
		_ownBubblesGradient = enabled;
		save();
	}
}

void Config::setHideAllChatsTab(bool enabled) {
	if (_hideAllChatsTab != enabled) {
		_hideAllChatsTab = enabled;
		save();
	}
}

void Config::setFreeEmoji(bool enabled) {
	if (_freeEmoji != enabled) {
		_freeEmoji = enabled;
		save();
	}
}

void Config::setPluginsEnabled(bool enabled) {
	if (_pluginsEnabled != enabled) {
		_pluginsEnabled = enabled;
		save();
	}
}

void Config::setPluginHooksEnabled(bool enabled) {
	if (_pluginHooksEnabled != enabled) {
		_pluginHooksEnabled = enabled;
		save();
	}
}

bool Config::isPluginEnabled(const QString &pluginId) const {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	return settings.value("plugins_enabled/" + pluginId, true).toBool();
}

void Config::setPluginEnabled(const QString &pluginId, bool enabled) {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	settings.setValue("plugins_enabled/" + pluginId, enabled);
}

QString Config::pluginPref(
		const QString &pluginId,
		const QString &key,
		const QString &fallback) const {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	return settings.value("plugins_prefs/" + pluginId + '/' + key, fallback).toString();
}

void Config::setPluginPref(
		const QString &pluginId,
		const QString &key,
		const QString &value) {
	QSettings settings(SettingsFilePath(), QSettings::IniFormat);
	settings.setValue("plugins_prefs/" + pluginId + '/' + key, value);
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
	_customFont = settings.value("appearance/custom_font", QString()).toString();
	_emojiFont = settings.value("appearance/emoji_font", "Default").toString();
	_profileGradient = settings.value("profile/gradient", "#8DD1B0-#B7A8E0").toString();
	_profileGradientEnabled = settings.value("profile/gradient_enabled", false).toBool();
	_unhideGifts = settings.value("profile/unhide_gifts", true).toBool();
	_seizureMode = settings.value("appearance/seizure_mode", false).toBool();
	_pinChannelFirst = settings.value("general/pin_channel_first", true).toBool();
	_ownBubblesGradient = settings.value("chat/own_bubbles_gradient", false).toBool();
	_hideAllChatsTab = settings.value("chat/hide_all_chats_tab", false).toBool();
	_freeEmoji = settings.value("general/free_emoji", true).toBool();
	_pluginsEnabled = settings.value("plugins/enabled", true).toBool();
	_pluginHooksEnabled = settings.value("plugins/hooks_enabled", true).toBool();
	_iconPack = static_cast<IconPack>(settings.value("appearance/icon_pack", 0).toInt());
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
	settings.setValue("appearance/custom_font", _customFont);
	settings.setValue("appearance/emoji_font", _emojiFont);
	settings.setValue("appearance/icon_pack", static_cast<int>(_iconPack));
	settings.setValue("profile/gradient", _profileGradient);
	settings.setValue("profile/gradient_enabled", _profileGradientEnabled);
	settings.setValue("profile/unhide_gifts", _unhideGifts);
	settings.setValue("appearance/seizure_mode", _seizureMode);
	settings.setValue("general/pin_channel_first", _pinChannelFirst);
	settings.setValue("chat/own_bubbles_gradient", _ownBubblesGradient);
	settings.setValue("chat/hide_all_chats_tab", _hideAllChatsTab);
	settings.setValue("general/free_emoji", _freeEmoji);
	settings.setValue("plugins/enabled", _pluginsEnabled);
	settings.setValue("plugins/hooks_enabled", _pluginHooksEnabled);
}

} // namespace Margy
