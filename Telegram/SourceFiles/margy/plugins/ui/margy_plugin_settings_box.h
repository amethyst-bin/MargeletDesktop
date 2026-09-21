#pragma once

#include "ui/layers/box_content.h"

namespace Margy::Plugins::UI {

class PluginSettingsBox final : public ::Ui::BoxContent {
public:
	static void Show(QWidget *parent, const QString &pluginId);

	PluginSettingsBox(QWidget *parent, const QString &pluginId);

protected:
	void prepare() override;

private:
	QString _pluginId;
};

} // namespace Margy::Plugins::UI
