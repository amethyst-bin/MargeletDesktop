#pragma once

#include <QString>

namespace Margy {

class Config final {
public:
	[[nodiscard]] static Config &Instance();

	[[nodiscard]] bool badgesEnabled() const { return _badgesEnabled; }
	void setBadgesEnabled(bool enabled);

	[[nodiscard]] QString version() const;

private:
	Config();
	~Config() = default;

	void load();
	void save();

	bool _badgesEnabled = true;
};

[[nodiscard]] inline bool BadgesEnabled() {
	return Config::Instance().badgesEnabled();
}

} // namespace Margy
