#pragma once

#include "settings/settings_common.h"
#include "settings/settings_common_session.h"

namespace Margy::Settings {

[[nodiscard]] ::Settings::Type MargySettingsId();

class MargySettingsSection final : public ::Settings::Section<MargySettingsSection> {
public:
	MargySettingsSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~MargySettingsSection() override = default;

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();
};

} // namespace Margy::Settings
