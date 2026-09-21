#pragma once

#include "ui/layers/box_content.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Margy::Plugins::UI {

class PluginsBox final : public ::Ui::BoxContent {
public:
	static void Show(QWidget *parent = nullptr);

	PluginsBox(QWidget *parent = nullptr);

protected:
	void prepare() override;

private:
	void rebuildList();

	::Ui::VerticalLayout *_listContainer = nullptr;
};

} // namespace Margy::Plugins::UI
