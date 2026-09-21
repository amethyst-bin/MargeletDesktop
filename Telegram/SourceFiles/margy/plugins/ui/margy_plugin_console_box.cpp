#include "margy/plugins/ui/margy_plugin_console_box.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "boxes/abstract_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/widgets/buttons.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"

#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QClipboard>
#include <QtCore/QDateTime>

namespace Margy::Plugins::UI {

void PluginConsoleBox::Show(QWidget *parent) {
	::Ui::show(::Box<PluginConsoleBox>());
}

PluginConsoleBox::PluginConsoleBox(QWidget *parent) {
}

void PluginConsoleBox::prepare() {
	setTitle(rpl::single(u"Консоль плагинов Margelet"_q));

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	const auto textEdit = content->add(
		object_ptr<::Ui::FixedHeightWidget>(content, 350));
	const auto edit = new QPlainTextEdit(textEdit);
	edit->setReadOnly(true);
	edit->setGeometry(0, 0, textEdit->width(), 350);
	edit->setStyleSheet(u"QPlainTextEdit { background-color: #1a1a1a; color: #e0e0e0; font-family: monospace; font-size: 11px; padding: 6px; border-radius: 4px; }"_q);

	textEdit->widthValue(
	) | rpl::on_next([=](int w) {
		edit->resize(w, 350);
	}, textEdit->lifetime());

	auto formatLine = [](const ConsoleLine &line) {
		const auto timeStr = QDateTime::fromMSecsSinceEpoch(line.timestamp).toString(u"hh:mm:ss"_q);
		const auto tag = line.isError ? u"[ERR]"_q : u"[LOG]"_q;
		return timeStr + u" " + tag + u" [" + line.plugin + u"] " + line.text;
	};

	for (const auto &line : Manager::Instance().console()) {
		edit->appendPlainText(formatLine(line));
	}

	Manager::Instance().consoleUpdates(
	) | rpl::on_next([=](const ConsoleLine &line) {
		edit->appendPlainText(formatLine(line));
	}, lifetime());

	addButton(u"Очистить"_q, [=] {
		Manager::Instance().clearConsole();
		edit->clear();
	});

	addButton(u"Скопировать"_q, [=] {
		QGuiApplication::clipboard()->setText(edit->toPlainText());
	});

	addButton(u"Закрыть"_q, [=] { closeBox(); });
}

} // namespace Margy::Plugins::UI
