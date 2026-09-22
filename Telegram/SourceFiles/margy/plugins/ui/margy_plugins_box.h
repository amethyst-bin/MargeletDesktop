#pragma once

class QWidget;

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Margy::Plugins::UI {

void InitPluginsBox(not_null<::Ui::GenericBox*> box);

class PluginsBox final {
public:
	static void Show(QWidget *parent = nullptr);
};

} // namespace Margy::Plugins::UI
