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

} // namespace Margy
