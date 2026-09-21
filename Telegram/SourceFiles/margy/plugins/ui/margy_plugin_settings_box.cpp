#include "margy/plugins/ui/margy_plugin_settings_box.h"
#include "margy/plugins/margy_plugin_manager.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"

#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"

namespace Margy::Plugins::UI {

void PluginSettingsBox::Show(QWidget *parent, const QString &pluginId) {
	::Ui::show(::Box<PluginSettingsBox>(pluginId));
}

PluginSettingsBox::PluginSettingsBox(QWidget *parent, const QString &pluginId)
: _pluginId(pluginId) {
}

void PluginSettingsBox::prepare() {
	const auto p = Manager::Instance().plugin(_pluginId);
	const auto name = p ? p->displayName() : _pluginId;
	setTitle(rpl::single(name + u" — Настройки"_q));

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	const auto rows = Manager::Instance().settings(_pluginId);
	for (const auto &row : rows) {
		switch (row.type) {
		case SettingType::Header:
			::Ui::AddSubsectionTitle(content, rpl::single(row.title));
			break;

		case SettingType::Note: {
			const auto label = content->add(
				object_ptr<::Ui::FlatLabel>(
					content,
					row.title,
					st::boxLabel),
				st::settingsSendTypePadding);
			label->setTextColorOverride(st::windowSubTextFg->c);
			break;
		}

		case SettingType::Switch: {
			const auto current = Config::Instance().pluginPref(
				_pluginId,
				row.key,
				row.defaultValue);
			const auto isChecked = (current == u"1"_q || current.toLower() == u"true"_q);
			const auto cb = content->add(
				object_ptr<::Ui::Checkbox>(
					content,
					row.title,
					isChecked,
					st::settingsCheckbox),
				st::settingsSendTypePadding);
			cb->checkedChanges(
			) | rpl::on_next([=, key = row.key](bool checked) {
				const auto val = checked ? u"1"_q : u"0"_q;
				Config::Instance().setPluginPref(_pluginId, key, val);
				Host::Instance().onSettingChanged(_pluginId, key, val);
			}, content->lifetime());
			break;
		}

		case SettingType::Choice: {
			if (row.options.empty()) {
				break;
			}
			const auto current = Config::Instance().pluginPref(
				_pluginId,
				row.key,
				row.defaultValue);
			auto initialIdx = 0;
			for (auto i = 0; i < int(row.options.size()); ++i) {
				if (row.options[i] == current) {
					initialIdx = i;
					break;
				}
			}

			const auto btn = content->add(
				object_ptr<::Ui::SettingsButton>(
					content,
					rpl::single(row.title + u": "_q + (initialIdx < int(row.options.size()) ? row.options[initialIdx] : current)),
					st::settingsButton),
				st::settingsSendTypePadding);

			const auto state = std::make_shared<int>(initialIdx);
			btn->setClickedCallback([=, opts = row.options, key = row.key] {
				*state = (*state + 1) % int(opts.size());
				const auto selected = opts[*state];
				btn->setText(rpl::single(row.title + u": "_q + selected));
				Config::Instance().setPluginPref(_pluginId, key, selected);
				Host::Instance().onSettingChanged(_pluginId, key, selected);
			});
			break;
		}

		case SettingType::Action: {
			const auto btn = content->add(
				object_ptr<::Ui::SettingsButton>(
					content,
					rpl::single(row.title),
					st::settingsButton),
				st::settingsSendTypePadding);
			btn->setClickedCallback([=, key = row.key] {
				Host::Instance().onSettingChanged(_pluginId, key, QString());
			});
			break;
		}

		case SettingType::Text:
			break;
		}
	}

	addButton(u"Готово"_q, [=] { closeBox(); });
}

} // namespace Margy::Plugins::UI
