#include "margy/plugins/ui/margy_plugins_box.h"
#include "margy/plugins/ui/margy_plugin_settings_box.h"
#include "margy/plugins/ui/margy_plugin_console_box.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"
#include "boxes/abstract_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/ui_utility.h"
#include "ui/toast/toast.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include "settings.h"

namespace Margy::Plugins::UI {

void InitPluginsBox(not_null<::Ui::GenericBox*> box) {
	box->setTitle(rpl::single(u"Плагины Margelet"_q));
	box->setWidth(st::boxWideWidth);

	const auto rebuildList = std::make_shared<Fn<void()>>();

	// Top action buttons
	const auto installBtn = box->addRow(
		object_ptr<::Ui::SettingsButton>(
			box.get(),
			rpl::single(u"➕ Установить из файла (.marp, .mtp, .zip)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	installBtn->setClickedCallback([=] {
		const auto file = QFileDialog::getOpenFileName(
			box.get(),
			u"Выберите файл плагина"_q,
			QString(),
			u"Плагины Margelet (*.marp *.mtp *.zip);;Все файлы (*.*)"_q);
		if (!file.isEmpty()) {
			auto err = QString();
			if (!Manager::Instance().installPlugin(file, &err)) {
				Manager::Instance().log(u"margelet"_q, u"Ошибка установки: "_q + err, true);
				::Ui::Toast::Show(box.get(), u"Ошибка установки: "_q + err);
			} else {
				::Ui::Toast::Show(box.get(), u"Плагин успешно установлен!"_q);
				if (*rebuildList) {
					(*rebuildList)();
				}
			}
		}
	});

	const auto consoleBtn = box->addRow(
		object_ptr<::Ui::SettingsButton>(
			box.get(),
			rpl::single(u"📋 Консоль плагинов"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	consoleBtn->setClickedCallback([=] {
		PluginConsoleBox::Show(box.get());
	});

	const auto openLogBtn = box->addRow(
		object_ptr<::Ui::SettingsButton>(
			box.get(),
			rpl::single(u"📄 Открыть файл логов (plugins_log.txt)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	openLogBtn->setClickedCallback([=] {
		const auto logPath = cWorkingDir() + u"plugins_log.txt"_q;
		if (!QFile::exists(logPath)) {
			Manager::Instance().log(u"margelet"_q, u"Лог-файл создан"_q);
		}
		QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));
	});

	const auto restartBtn = box->addRow(
		object_ptr<::Ui::SettingsButton>(
			box.get(),
			rpl::single(u"🔄 Перезапустить плагины"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	restartBtn->setClickedCallback([=] {
		Host::Instance().restart();
	});

	::Ui::AddDivider(box->verticalLayout());
	::Ui::AddSkip(box->verticalLayout());
	box->addRow(object_ptr<::Ui::FlatLabel>(box.get(), u"Установленные плагины"_q, st::boxTitle), st::boxRowPadding);

	const auto listContainer = box->addRow(object_ptr<::Ui::VerticalLayout>(box.get()));

	*rebuildList = [=] {
		listContainer->clear();

		const auto plugins = Manager::Instance().installedPlugins();
		if (plugins.empty()) {
			listContainer->add(
				object_ptr<::Ui::FlatLabel>(
					listContainer,
					u"Нет установленных плагинов.\nНажмите «Установить из файла (.marp)», чтобы добавить плагин."_q,
					st::boxLabel),
				st::settingsSendTypePadding,
				style::al_center);
			listContainer->resizeToWidth(box->width());
			return;
		}

		for (const auto &p : plugins) {
			const auto row = listContainer->add(
				object_ptr<::Ui::VerticalLayout>(listContainer),
				st::settingsSendTypePadding);

			// Header row: Checkbox with name & version
			const auto title = p.displayName()
				+ u" (v"_q + p.version + u")"_q
				+ (p.usesHooks ? u" ⚡ [Хуки]"_q : QString());

			const auto isChecked = Manager::Instance().isEnabled(p.id);
			const auto checkbox = row->add(
				object_ptr<::Ui::Checkbox>(
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
					object_ptr<::Ui::FlatLabel>(
						row,
						u"Автор: "_q + p.author,
						st::boxLabel),
					QMargins(24, 2, 0, 0));
			}

			if (!p.displayDescription().isEmpty()) {
				row->add(
					object_ptr<::Ui::FlatLabel>(
						row,
						p.displayDescription(),
						st::boxLabel),
					QMargins(24, 2, 0, 4));
			}

			// Action buttons row
			const auto btnWrap = row->add(
				object_ptr<::Ui::FixedHeightWidget>(row, 36),
				QMargins(24, 4, 0, 8));
			const auto btnLayout = new QHBoxLayout(btnWrap);
			btnLayout->setContentsMargins(0, 0, 0, 0);

			if (Manager::Instance().hasSettings(p.id)) {
				const auto settingsBtn = new ::Ui::RoundButton(
					btnWrap,
					rpl::single(u"Настройки"_q),
					st::defaultBoxButton);
				settingsBtn->setClickedCallback([=, pluginId = p.id] {
					PluginSettingsBox::Show(box.get(), pluginId);
				});
				btnLayout->addWidget(settingsBtn);
			}

			const auto deleteBtn = new ::Ui::RoundButton(
				btnWrap,
				rpl::single(u"Удалить"_q),
				st::defaultBoxButton);
			deleteBtn->setClickedCallback([=, pluginId = p.id] {
				Manager::Instance().uninstallPlugin(pluginId);
				::Ui::Toast::Show(box.get(), u"Плагин удалён"_q);
				if (*rebuildList) {
					(*rebuildList)();
				}
			});
			btnLayout->addWidget(deleteBtn);
			btnLayout->addStretch();

			::Ui::AddDivider(listContainer);
		}
		listContainer->resizeToWidth(box->width());
		box->verticalLayout()->resizeToWidth(box->width());
	};

	Manager::Instance().pluginsUpdated(
	) | rpl::on_next([=] {
		if (*rebuildList) {
			(*rebuildList)();
		}
	}, box->lifetime());

	(*rebuildList)();

	box->addButton(rpl::single(u"Закрыть"_q), [=] { box->closeBox(); });
}

void PluginsBox::Show(QWidget *parent) {
	::Ui::show(::Box(InitPluginsBox));
}

} // namespace Margy::Plugins::UI
