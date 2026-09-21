#include "margy/settings/margy_settings_section.h"
#include "margy/margy_config.h"
#include "margy/badges/margy_plane_3d.h"
#include "margy/badges/margy_badge_gallery_box.h"
#include "margy/cats/margy_cats_box.h"
#include "margy/sound/margy_meow.h"
#include "margy/fonts/margy_fonts.h"
#include "margy/gradient/margy_gradient.h"
#include "margy/wall/margy_wall_box.h"
#include "margy/donate/margy_donate_box.h"
#include "margy/proxy/margy_proxy.h"
#include "margy/seizure/margy_seizure.h"
#include "margy/gifts/margy_gifts.h"
#include "margy/plugins/ui/margy_plugins_box.h"
#include "margy/plugins/ui/margy_plugin_console_box.h"

#include "ui/vertical_list.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"

#include <QDesktopServices>
#include <QUrl>

namespace Margy::Settings {

MargySettingsSection::MargySettingsSection(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> MargySettingsSection::title() {
	return rpl::single(u"Настройки Margy"_q);
}

void MargySettingsSection::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	// 3D Header Widget
	const auto planeWrap = content->add(
		object_ptr<Ui::FixedHeightWidget>(content, 150));
	const auto plane = Ui::CreateChild<Badges::Plane3D>(planeWrap, Badges::BadgeType::Margy);
	plane->resize(150, 150);
	planeWrap->widthValue(
	) | rpl::on_next([=](int w) {
		plane->move((w - 150) / 2, 0);
	}, plane->lifetime());

	// Title and Version
	const auto titleLabel = content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			u"Margy Desktop v" + Config::Instance().version(),
			st::settingsHeader),
		st::settingsHeaderPadding);
	titleLabel->setAlignment(Qt::AlignCenter);

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// Badges & Gallery
	Ui::AddSubsectionTitle(content, rpl::single(u"Бейджи и галерея"_q));
	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Включить бейджи сообщества Margy"_q,
			Config::Instance().badgesEnabled(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setBadgesEnabled(checked);
	}, content->lifetime());

	const auto galleryBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Открыть галерею бейджей"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	galleryBtn->setClickedCallback([=] {
		Badges::BadgeGalleryBox::Show(this);
	});

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// Profile & Privacy (Streamer mode)
	Ui::AddSubsectionTitle(content, rpl::single(u"Профиль и приватность"_q));
	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Показывать ID в профилях"_q,
			Config::Instance().showIds(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setShowIds(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Режим стримера (маскировка телефона)"_q,
			Config::Instance().streamerMode(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setStreamerMode(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Режим стримера: скрывать чужие юзернеймы"_q,
			Config::Instance().streamerHidesOthers(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setStreamerHidesOthers(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Режим стримера: скрывать свой юзернейм"_q,
			Config::Instance().streamerHidesUsername(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setStreamerHidesUsername(checked);
	}, content->lifetime());

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// Conveniences
	Ui::AddSubsectionTitle(content, rpl::single(u"Удобства"_q));
	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Копирование с разметкой"_q,
			Config::Instance().copyFormatting(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setCopyFormatting(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Редактор тегов аудио"_q,
			Config::Instance().tagsEnabled(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setTagsEnabled(checked);
	}, content->lifetime());

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// Cats Viewer
	Ui::AddSubsectionTitle(content, rpl::single(u"Коты Margy 🐾"_q));
	const auto catsBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Посмотреть котиков в приложении 🐱"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	catsBtn->setClickedCallback([=] {
		Cats::CatsBox::Show(this);
	});

	// Appearance & Fonts
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Внешний вид и шрифты"_q));

	const auto fontsBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Шрифты интерфейса и эмодзи (Twemoji)..."_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	fontsBtn->setClickedCallback([=] {
		Fonts::FontsBox::Show(this);
	});

	const auto gradBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Двухцветный градиент профиля..."_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	gradBtn->setClickedCallback([=] {
		Gradient::GradientBox::Show(this);
	});

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Красить свои сообщения градиентом профиля"_q,
			Config::Instance().ownBubblesGradient(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setOwnBubblesGradient(checked);
	}, content->lifetime());

	// Wall
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Стена Margy"_q));

	const auto wallBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Открыть стену профиля 📝"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	wallBtn->setClickedCallback([=] {
		Wall::WallBox::Show(this);
	});

	// Gifts and Channels
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Подарки и возможности"_q));

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Показывать скрытые подарки (Star Gifts)"_q,
			Config::Instance().unhideGifts(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setUnhideGifts(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Премиум-значки для всех (видны в Margy)"_q,
			Config::Instance().freeEmoji(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setFreeEmoji(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Закреплять каналы первыми"_q,
			Config::Instance().pinChannelFirst(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setPinChannelFirst(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Скрывать папку «Все чаты»"_q,
			Config::Instance().hideAllChatsTab(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setHideAllChatsTab(checked);
	}, content->lifetime());

	// Plugins
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Плагины Margelet"_q));

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Включить систему плагинов Python (.marp)"_q,
			Config::Instance().pluginsEnabled(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setPluginsEnabled(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Разрешить хуки методов (Method hooks)"_q,
			Config::Instance().pluginHooksEnabled(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setPluginHooksEnabled(checked);
	}, content->lifetime());

	const auto pluginsBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Управление плагинами (.marp)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	pluginsBtn->setClickedCallback([=] {
		Plugins::UI::PluginsBox::Show(this);
	});

	const auto pluginConsoleBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Консоль плагинов"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	pluginConsoleBtn->setClickedCallback([=] {
		Plugins::UI::PluginConsoleBox::Show(this);
	});

	// Proxy
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Сеть и прокси"_q));

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Встроенный MTProxy сообщества Margy"_q,
			Proxy::IsCommunityProxyActive(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		if (checked) {
			Proxy::ConnectCommunityProxy();
		} else {
			Proxy::DisconnectCommunityProxy();
		}
	}, content->lifetime());

	// Sound (Easter egg)
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Звуки и пасхалки"_q));
	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Звук мяуканья при клике на логотип"_q,
			Config::Instance().meowEnabled(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setMeowEnabled(checked);
	}, content->lifetime());

	content->add(
		object_ptr<Ui::Checkbox>(
			content,
			u"Пасхалка: режим «Приступ» (радужные цвета)"_q,
			Config::Instance().seizureMode(),
			st::settingsCheckbox),
		st::settingsSendTypePadding
	)->checkedChanges(
	) | rpl::on_next([=](bool checked) {
		Config::Instance().setSeizureMode(checked);
	}, content->lifetime());

	const auto testSoundBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Проверить звук 🐾"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	testSoundBtn->setClickedCallback([=] {
		Sound::PlayMeow();
	});

	// Support & Donate
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Поддержка проекта"_q));

	const auto donateBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Поддержать развитие Margy (Донат) 💖"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	donateBtn->setClickedCallback([=] {
		Donate::DonateBox::Show(this);
	});

	// Community & Links
	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Сообщество Margy"_q));

	const auto channelBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Канал Margelet (Telegram)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	channelBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletter"_q));
	});

	const auto stickersBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Стикерпак Margelet (Telegram)"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	stickersBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/addstickers/MargeletPackMargeletter"_q));
	});

	const auto forumBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Форум сообщества"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	forumBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletforum"_q));
	});

	const auto ghBtn = content->add(
		object_ptr<Ui::SettingsButton>(
			content,
			rpl::single(u"Репозиторий MargyDesktop"_q),
			st::settingsButton),
		st::settingsSendTypePadding);
	ghBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://github.com/amethyst-bin/MargeletDesktop"_q));
	});

	Ui::ResizeFitChild(this, content);
}

::Settings::Type MargySettingsId() {
	return MargySettingsSection::Id();
}

} // namespace Margy::Settings
