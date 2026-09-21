#include "margy/plugins/ui/margy_plugins_box.h"
#include "margy/plugins/ui/margy_plugin_settings_box.h"
#include "margy/plugins/ui/margy_plugin_console_box.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"
#include "boxes/abstract_box.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/ui_utility.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

namespace Margy::Plugins::UI {

void PluginsBox::Show(QWidget *parent) {
	::Ui::show(::Box<PluginsBox>());
}

PluginsBox::PluginsBox(QWidget *parent) {
}

void PluginsBox::prepare() {
	setTitle(rpl::single(u"Плагины Margelet"_q));

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	// Top actions
	const auto installBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"➕ Установить из файла (.marp)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	installBtn->setClickedCallback([=] {
		const auto file = QFileDialog::getOpenFileName(
			this,
			u"Выберите файл плагина"_q,
			QString(),
			u"Плагины Margelet (*.marp)"_q);
		if (!file.isEmpty()) {
			auto err = QString();
			if (!Manager::Instance().installPlugin(file, &err)) {
				Manager::Instance().log(u"margelet"_q, u"Ошибка установки: "_q + err, true);
			}
		}
	});

	const auto consoleBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"📋 Консоль плагинов"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	consoleBtn->setClickedCallback([=] {
		PluginConsoleBox::Show(this);
	});

	const auto restartBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"🔄 Перезапустить плагины"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	restartBtn->setClickedCallback([=] {
		Host::Instance().restart();
	});

	::Ui::AddSkip(content);
	::Ui::AddDivider(content);
	::Ui::AddSubsectionTitle(content, rpl::single(u"Установленные плагины"_q));

	_listContainer = content->add(object_ptr<::Ui::VerticalLayout>(content));

	Manager::Instance().pluginsUpdated(
	) | rpl::on_next([=] {
		rebuildList();
	}, lifetime());

	rebuildList();

	addButton(u"Закрыть"_q, [=] { closeBox(); });
}

void PluginsBox::rebuildList() {
	if (!_listContainer) {
		return;
	}

	_listContainer->clear();

	const auto plugins = Manager::Instance().installedPlugins();
	if (plugins.empty()) {
		const auto emptyLabel = _listContainer->add(
			object_ptr<::Ui::FlatLabel>(
				_listContainer,
				u"Нет установленных плагинов.\nНажмите «Установить из файла (.marp)», чтобы добавить плагин."_q,
				st::boxLabel),
			st::settingsSendTypePadding);
		emptyLabel->setAlignment(Qt::AlignCenter);
		return;
	}

	for (const auto &p : plugins) {
		const auto row = _listContainer->add(
			object_ptr<::Ui::VerticalLayout>(_listContainer),
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
				st::settingsCheckbox));

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
				PluginSettingsBox::Show(this, pluginId);
			});
			btnLayout->addWidget(settingsBtn);
		}

		const auto deleteBtn = new ::Ui::RoundButton(
			btnWrap,
			rpl::single(u"Удалить"_q),
			st::defaultBoxButton);
		deleteBtn->setClickedCallback([=, pluginId = p.id] {
			Manager::Instance().uninstallPlugin(pluginId);
		});
		btnLayout->addWidget(deleteBtn);
		btnLayout->addStretch();

		::Ui::AddDivider(_listContainer);
	}
}

} // namespace Margy::Plugins::UI
