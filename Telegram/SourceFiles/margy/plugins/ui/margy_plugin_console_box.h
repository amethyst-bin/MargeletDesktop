#pragma once

#include "ui/layers/box_content.h"

namespace Margy::Plugins::UI {

class PluginConsoleBox final : public ::Ui::BoxContent {
public:
	static void Show(QWidget *parent = nullptr);

	PluginConsoleBox(QWidget *parent = nullptr);

protected:
	void prepare() override;

private:
	void appendLine(const QString &formatted, bool isError);

	QPointer<QWidget> _logContainer;
};

} // namespace Margy::Plugins::UI
