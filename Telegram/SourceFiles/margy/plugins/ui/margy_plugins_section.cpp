#include "margy/plugins/ui/margy_plugins_section.h"
#include "margy/plugins/ui/margy_plugin_settings_box.h"
#include "margy/plugins/ui/margy_plugin_console_box.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"
#include "settings/settings_common.h"
#include "window/window_session_controller.h"
#include "boxes/abstract_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/toast/toast.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include "settings.h"

namespace Margy::Plugins::UI {

MargyPluginsSection::MargyPluginsSection(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> MargyPluginsSection::title() {
	return rpl::single(u"Плагины Margelet"_q);
}

void MargyPluginsSection::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	Ui::AddSubsectionTitle(content, rpl::single(u"Управление плагинами"_q));

	const auto installBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Установить из файла (.marp, .zip)"_q),
		st::settingsButton,
		{ &st::menuIconInvite, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x10, 0xB9, 0x81)) });
	installBtn->setClickedCallback([=] {
		const auto file = QFileDialog::getOpenFileName(
			this,
			u"Выберите файл плагина"_q,
			QString(),
			u"Плагины Margelet (*.marp *.mtp *.zip);;Все файлы (*.*)"_q);
		if (!file.isEmpty()) {
			auto err = QString();
			if (!Manager::Instance().installPlugin(file, &err)) {
				Manager::Instance().log(u"margelet"_q, u"Ошибка установки: "_q + err, true);
				controller()->showToast(u"Ошибка установки: "_q + err);
			} else {
				controller()->showToast(u"Плагин успешно установлен"_q);
			}
		}
	});

	const auto consoleBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Консоль плагинов"_q),
		st::settingsButton,
		{ &st::menuIconShowInChat, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x06, 0xB6, 0xD4)) });
	consoleBtn->setClickedCallback([=] {
		PluginConsoleBox::Show(this);
	});

	const auto openLogBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Открыть файл логов (plugins_log.txt)"_q),
		st::settingsButton,
		{ &st::menuIconSettings, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x47, 0x55, 0x69)) });
	openLogBtn->setClickedCallback([=] {
		const auto logPath = cWorkingDir() + u"plugins_log.txt"_q;
		if (!QFile::exists(logPath)) {
			Manager::Instance().log(u"margelet"_q, u"Лог-файл создан"_q);
		}
		QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));
	});

	const auto restartBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Перезапустить движок плагинов"_q),
		st::settingsButton,
		{ &st::menuIconRestore, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0xF5, 0x9E, 0x0B)) });
	restartBtn->setClickedCallback([=] {
		Host::Instance().restart();
		controller()->showToast(u"Движок плагинов перезапущен"_q);
	});

	const auto addToggle = [&](const QString &text, bool checked, auto &&callback) {
		const auto toggle = ::Settings::AddButtonWithIcon(
			content,
			rpl::single(text),
			st::settingsButtonNoIcon
		)->toggleOn(rpl::single(checked));
		toggle->toggledChanges(
		) | rpl::on_next(std::move(callback), content->lifetime());
		return toggle;
	};

	addToggle(
		u"Включить систему плагинов Python"_q,
		Config::Instance().pluginsEnabled(),
		[=](bool checked) {
			Config::Instance().setPluginsEnabled(checked);
		});

	addToggle(
		u"Разрешить хуки методов (Method hooks)"_q,
		Config::Instance().pluginHooksEnabled(),
		[=](bool checked) {
			Config::Instance().setPluginHooksEnabled(checked);
		});

	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSkip(content);

	Ui::AddSubsectionTitle(content, rpl::single(u"Установленные плагины"_q));

	const auto listContainer = content->add(object_ptr<Ui::VerticalLayout>(content));

	const auto rebuildList = [=] {
		listContainer->clear();

		const auto plugins = Manager::Instance().installedPlugins();
		if (plugins.empty()) {
			listContainer->add(
				object_ptr<Ui::FlatLabel>(
					listContainer,
					u"Нет установленных плагинов.\nНажмите «Установить из файла», чтобы добавить плагин."_q,
					st::boxLabel),
				st::settingsSendTypePadding,
				style::al_center);
			Ui::ResizeFitChild(this, content);
			return;
		}

		for (const auto &p : plugins) {
			const auto row = listContainer->add(
				object_ptr<Ui::VerticalLayout>(listContainer),
				st::settingsSendTypePadding);

			const auto title = p.displayName()
				+ u" (v"_q + p.version + u")"_q
				+ (p.usesHooks ? u" [Хуки]"_q : QString());

			const auto isChecked = Manager::Instance().isEnabled(p.id);
			const auto checkbox = row->add(
				object_ptr<Ui::Checkbox>(
					row,
					title,
					isChecked,
					st::defaultCheckbox,
					st::defaultToggle));

			checkbox->checkedChanges(
			) | rpl::on_next([=, pluginId = p.id](bool checked) {
				Manager::Instance().setEnabled(pluginId, checked);
			}, row->lifetime());

			if (!p.author.isEmpty()) {
				row->add(
					object_ptr<Ui::FlatLabel>(
						row,
						u"Автор: "_q + p.author,
						st::boxLabel),
					QMargins(24, 2, 0, 0));
			}

			if (!p.displayDescription().isEmpty()) {
				row->add(
					object_ptr<Ui::FlatLabel>(
						row,
						p.displayDescription(),
						st::boxLabel),
					QMargins(24, 2, 0, 4));
			}

			const auto btnWrap = row->add(
				object_ptr<Ui::FixedHeightWidget>(row, 36),
				QMargins(24, 4, 0, 8));
			const auto btnLayout = new QHBoxLayout(btnWrap);
			btnLayout->setContentsMargins(0, 0, 0, 0);

			if (Manager::Instance().hasSettings(p.id)) {
				const auto settingsBtn = new Ui::RoundButton(
					btnWrap,
					rpl::single(u"Настройки"_q),
					st::defaultBoxButton);
				settingsBtn->setClickedCallback([=, pluginId = p.id] {
					PluginSettingsBox::Show(this, pluginId);
				});
				btnLayout->addWidget(settingsBtn);
			}

			const auto deleteBtn = new Ui::RoundButton(
				btnWrap,
				rpl::single(u"Удалить"_q),
				st::defaultBoxButton);
			deleteBtn->setClickedCallback([=, pluginId = p.id] {
				Manager::Instance().uninstallPlugin(pluginId);
				controller()->showToast(u"Плагин удалён"_q);
			});
			btnLayout->addWidget(deleteBtn);
			btnLayout->addStretch();

			Ui::AddDivider(listContainer);
		}

		Ui::ResizeFitChild(this, content);
	};

	Manager::Instance().pluginsUpdated(
	) | rpl::on_next([=] {
		rebuildList();
	}, content->lifetime());

	rebuildList();

	Ui::ResizeFitChild(this, content);
}

::Settings::Type MargyPluginsSectionId() {
	return MargyPluginsSection::Id();
}

} // namespace Margy::Plugins::UI
