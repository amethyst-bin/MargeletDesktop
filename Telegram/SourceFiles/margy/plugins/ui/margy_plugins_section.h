#pragma once

#include "settings/settings_common.h"
#include "settings/settings_common_session.h"

namespace Margy::Plugins::UI {

[[nodiscard]] ::Settings::Type MargyPluginsSectionId();

class MargyPluginsSection final : public ::Settings::Section<MargyPluginsSection> {
public:
	MargyPluginsSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~MargyPluginsSection() override = default;

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();
};

} // namespace Margy::Plugins::UI
