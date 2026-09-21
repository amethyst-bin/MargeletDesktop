#pragma once

#include <QString>

namespace Margy {

class Config final {
public:
	[[nodiscard]] static Config &Instance();

	[[nodiscard]] bool badgesEnabled() const { return _badgesEnabled; }
	void setBadgesEnabled(bool enabled);

	[[nodiscard]] bool showIds() const { return _showIds; }
	void setShowIds(bool enabled);

	[[nodiscard]] bool copyFormatting() const { return _copyFormatting; }
	void setCopyFormatting(bool enabled);

	[[nodiscard]] bool streamerMode() const { return _streamerMode; }
	void setStreamerMode(bool enabled);

	[[nodiscard]] bool streamerHidesOthers() const { return _streamerHidesOthers; }
	void setStreamerHidesOthers(bool enabled);

	[[nodiscard]] bool streamerHidesUsername() const { return _streamerHidesUsername; }
	void setStreamerHidesUsername(bool enabled);

	[[nodiscard]] bool meowEnabled() const { return _meowEnabled; }
	void setMeowEnabled(bool enabled);

	[[nodiscard]] bool meowHeard() const { return _meowHeard; }
	void setMeowHeard(bool heard);

	[[nodiscard]] bool tagsEnabled() const { return _tagsEnabled; }
	void setTagsEnabled(bool enabled);

	[[nodiscard]] bool bubblesOutline() const { return _bubblesOutline; }
	void setBubblesOutline(bool enabled);

	[[nodiscard]] QString customFont() const { return _customFont; }
	void setCustomFont(const QString &font);

	[[nodiscard]] QString emojiFont() const { return _emojiFont; }
	void setEmojiFont(const QString &font);

	[[nodiscard]] QString profileGradient() const { return _profileGradient; }
	void setProfileGradient(const QString &gradient);

	[[nodiscard]] bool profileGradientEnabled() const { return _profileGradientEnabled; }
	void setProfileGradientEnabled(bool enabled);

	[[nodiscard]] bool unhideGifts() const { return _unhideGifts; }
	void setUnhideGifts(bool enabled);

	[[nodiscard]] bool seizureMode() const { return _seizureMode; }
	void setSeizureMode(bool enabled);

	[[nodiscard]] bool pinChannelFirst() const { return _pinChannelFirst; }
	void setPinChannelFirst(bool enabled);

	[[nodiscard]] bool ownBubblesGradient() const { return _ownBubblesGradient; }
	void setOwnBubblesGradient(bool enabled);

	[[nodiscard]] bool hideAllChatsTab() const { return _hideAllChatsTab; }
	void setHideAllChatsTab(bool enabled);

	[[nodiscard]] bool freeEmoji() const { return _freeEmoji; }
	void setFreeEmoji(bool enabled);

	[[nodiscard]] QString version() const;

private:
	Config();
	~Config() = default;

	void load();
	void save();

	bool _badgesEnabled = true;
	bool _showIds = true;
	bool _copyFormatting = true;
	bool _streamerMode = false;
	bool _streamerHidesOthers = false;
	bool _streamerHidesUsername = false;
	bool _meowEnabled = false;
	bool _meowHeard = false;
	bool _tagsEnabled = true;
	bool _bubblesOutline = false;
	QString _customFont;
	QString _emojiFont = "Default";
	QString _profileGradient = "#8DD1B0-#B7A8E0";
	bool _profileGradientEnabled = false;
	bool _unhideGifts = true;
	bool _seizureMode = false;
	bool _pinChannelFirst = true;
	bool _ownBubblesGradient = false;
	bool _hideAllChatsTab = false;
	bool _freeEmoji = true;
};

[[nodiscard]] inline bool BadgesEnabled() {
	return Config::Instance().badgesEnabled();
}

[[nodiscard]] inline bool ShowIds() {
	return Config::Instance().showIds();
}

[[nodiscard]] inline bool CopyFormatting() {
	return Config::Instance().copyFormatting();
}

[[nodiscard]] inline bool StreamerMode() {
	return Config::Instance().streamerMode();
}

[[nodiscard]] inline bool StreamerHidesOthers() {
	return Config::Instance().streamerHidesOthers();
}

[[nodiscard]] inline bool StreamerHidesUsername() {
	return Config::Instance().streamerHidesUsername();
}

[[nodiscard]] inline bool MeowEnabled() {
	return Config::Instance().meowEnabled();
}

[[nodiscard]] inline bool MeowHeard() {
	return Config::Instance().meowHeard();
}

[[nodiscard]] inline bool TagsEnabled() {
	return Config::Instance().tagsEnabled();
}

[[nodiscard]] inline bool BubblesOutline() {
	return Config::Instance().bubblesOutline();
}

[[nodiscard]] inline QString CustomFont() {
	return Config::Instance().customFont();
}

[[nodiscard]] inline QString EmojiFont() {
	return Config::Instance().emojiFont();
}

[[nodiscard]] inline QString ProfileGradient() {
	return Config::Instance().profileGradient();
}

[[nodiscard]] inline bool ProfileGradientEnabled() {
	return Config::Instance().profileGradientEnabled();
}

[[nodiscard]] inline bool UnhideGifts() {
	return Config::Instance().unhideGifts();
}

[[nodiscard]] inline bool SeizureMode() {
	return Config::Instance().seizureMode();
}

[[nodiscard]] inline bool PinChannelFirst() {
	return Config::Instance().pinChannelFirst();
}

} // namespace Margy
