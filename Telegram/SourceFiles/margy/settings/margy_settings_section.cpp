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
#include "settings/settings_common.h"
#include "chat_helpers/emoji_sets_manager.h"
#include "boxes/abstract_box.h"

#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "window/window_session_controller.h"
#include "styles/style_settings.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

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
	const auto plane = content->add(
		object_ptr<Badges::Plane3D>(content, QColor(0x8D, 0xD1, 0xB0)));
	plane->resize(st::boxWideWidth, 150);
	plane->show();

	// Title and Version
	content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			u"Margelet Desktop v" + Config::Instance().version(),
			st::boxTitle),
		st::settingsSendTypePadding,
		style::al_center);

	Ui::AddSkip(content);
	Ui::AddDivider(content);

	// 1. Badges Section
	Ui::AddSubsectionTitle(content, rpl::single(u"Бейджи сообщества Margelet"_q));

	const auto galleryBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Открыть галерею бейджей"_q),
		st::settingsButton,
		{ &st::menuIconStar, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0xF5, 0x9E, 0x0B)) });
	galleryBtn->setClickedCallback([=] {
		Badges::BadgeGalleryBox::Show(this);
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
		u"Включить бейджи сообщества Margy"_q,
		Config::Instance().badgesEnabled(),
		[=](bool checked) {
			Config::Instance().setBadgesEnabled(checked);
		});

	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSkip(content);

	// 2. Customization & Appearance
	Ui::AddSubsectionTitle(content, rpl::single(u"Оформление и кастомизация"_q));

	const auto fontsBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Эмодзи Шрифт"_q),
		st::settingsButton,
		{ &st::menuIconChatBubble, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x3B, 0x82, 0xF6)) });
	fontsBtn->setClickedCallback([=] {
		controller()->show(Box<Ui::Emoji::ManageSetsBox>(&controller()->session()));
	});

	const auto catsBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Коты Margy 🐾"_q),
		st::settingsButton,
		{ &st::menuIconStickers, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0xEC, 0x48, 0x99)) });
	catsBtn->setClickedCallback([=] {
		Cats::CatsBox::Show(this);
	});

	const auto gradBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Двухцветный градиент профиля..."_q),
		st::settingsButton,
		{ &st::menuIconPalette, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x8B, 0x5C, 0xF6)) });
	gradBtn->setClickedCallback([=] {
		Gradient::GradientBox::Show(this);
	});

	const auto wallBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Стена профиля 📝"_q),
		st::settingsButton,
		{ &st::menuIconEdit, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x10, 0xB9, 0x81)) });
	wallBtn->setClickedCallback([=] {
		Wall::WallBox::Show(this);
	});

	addToggle(
		u"Красить свои сообщения градиентом профиля"_q,
		Config::Instance().ownBubblesGradient(),
		[=](bool checked) {
			Config::Instance().setOwnBubblesGradient(checked);
		});

	addToggle(
		u"Премиум-значки для всех (видны в Margy)"_q,
		Config::Instance().freeEmoji(),
		[=](bool checked) {
			Config::Instance().setFreeEmoji(checked);
		});

	addToggle(
		u"Показывать скрытые подарки (Star Gifts)"_q,
		Config::Instance().unhideGifts(),
		[=](bool checked) {
			Config::Instance().setUnhideGifts(checked);
		});

	addToggle(
		u"Закреплять каналы первыми"_q,
		Config::Instance().pinChannelFirst(),
		[=](bool checked) {
			Config::Instance().setPinChannelFirst(checked);
		});

	addToggle(
		u"Скрывать папку «Все чаты»"_q,
		Config::Instance().hideAllChatsTab(),
		[=](bool checked) {
			Config::Instance().setHideAllChatsTab(checked);
		});

	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSkip(content);

	// 3. Streamer Mode & Privacy
	Ui::AddSubsectionTitle(content, rpl::single(u"Режим стримера и приватность"_q));

	addToggle(
		u"Режим стримера (полная маскировка +• ••• •••-••-••)"_q,
		Config::Instance().streamerMode(),
		[=](bool checked) {
			Config::Instance().setStreamerMode(checked);
		});

	addToggle(
		u"Скрывать чужие юзернеймы"_q,
		Config::Instance().streamerHidesOthers(),
		[=](bool checked) {
			Config::Instance().setStreamerHidesOthers(checked);
		});

	addToggle(
		u"Скрывать свой юзернейм"_q,
		Config::Instance().streamerHidesUsername(),
		[=](bool checked) {
			Config::Instance().setStreamerHidesUsername(checked);
		});

	addToggle(
		u"Показывать ID в профилях"_q,
		Config::Instance().showIds(),
		[=](bool checked) {
			Config::Instance().setShowIds(checked);
		});

	addToggle(
		u"Копирование с разметкой"_q,
		Config::Instance().copyFormatting(),
		[=](bool checked) {
			Config::Instance().setCopyFormatting(checked);
		});

	addToggle(
		u"Редактор тегов аудио"_q,
		Config::Instance().tagsEnabled(),
		[=](bool checked) {
			Config::Instance().setTagsEnabled(checked);
		});

	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSkip(content);

	// 4. Plugins Margelet
	Ui::AddSubsectionTitle(content, rpl::single(u"Плагины Margelet"_q));

	const auto pluginsBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Управление плагинами (.marp)"_q),
		st::settingsButton,
		{ &st::menuIconManage, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x06, 0xB6, 0xD4)) });
	pluginsBtn->setClickedCallback([=] {
		Plugins::UI::PluginsBox::Show(this);
	});

	const auto pluginConsoleBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Консоль плагинов"_q),
		st::settingsButton,
		{ &st::menuIconUnmute, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x47, 0x55, 0x69)) });
	pluginConsoleBtn->setClickedCallback([=] {
		Plugins::UI::PluginConsoleBox::Show(this);
	});

	addToggle(
		u"Включить систему плагинов Python (.marp)"_q,
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

	// 5. Network, Sounds & Easter Eggs
	Ui::AddSubsectionTitle(content, rpl::single(u"Сеть, звуки и пасхалки"_q));

	addToggle(
		u"Встроенный MTProxy сообщества Margy"_q,
		Proxy::IsCommunityProxyActive(),
		[=](bool checked) {
			if (checked) {
				Proxy::ConnectCommunityProxy();
			} else {
				Proxy::DisconnectCommunityProxy();
			}
		});

	addToggle(
		u"Звук мяуканья при клике на логотип"_q,
		Config::Instance().meowEnabled(),
		[=](bool checked) {
			Config::Instance().setMeowEnabled(checked);
		});

	const auto testSoundBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Проверить звук 🐾"_q),
		st::settingsButton,
		{ &st::menuIconSoundOn, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0xEA, 0xB3, 0x08)) });
	testSoundBtn->setClickedCallback([=] {
		Sound::PlayMeow();
	});

	addToggle(
		u"Пасхалка: режим «Приступ» (радужные цвета)"_q,
		Config::Instance().seizureMode(),
		[=](bool checked) {
			Config::Instance().setSeizureMode(checked);
		});

	Ui::AddSkip(content);
	Ui::AddDivider(content);
	Ui::AddSkip(content);

	// 6. Support & Community
	Ui::AddSubsectionTitle(content, rpl::single(u"Поддержка и сообщество Margelet"_q));

	const auto donateBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Поддержать развитие Margy (Донат) 💖"_q),
		st::settingsButton,
		{ &st::menuIconPremium, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0xEF, 0x44, 0x44)) });
	donateBtn->setClickedCallback([=] {
		Donate::DonateBox::Show(this);
	});

	const auto channelBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Канал Margelet (Telegram)"_q),
		st::settingsButton,
		{ &st::menuIconChannel, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x02, 0x84, 0xC7)) });
	channelBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletter"_q));
	});

	const auto stickersBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Стикерпак Margelet (Telegram)"_q),
		st::settingsButton,
		{ &st::menuIconStickers, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x8B, 0x5C, 0xF6)) });
	stickersBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/addstickers/MargeletPackMargeletter"_q));
	});

	const auto forumBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Форум сообщества"_q),
		st::settingsButton,
		{ &st::menuIconDiscussion, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x63, 0x66, 0xF1)) });
	forumBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/margeletforum"_q));
	});

	const auto ghBtn = ::Settings::AddButtonWithIcon(
		content,
		rpl::single(u"Репозиторий MargyDesktop"_q),
		st::settingsButton,
		{ &st::menuIconInvite, ::Settings::IconType::Rounded, nullptr, QBrush(QColor(0x33, 0x41, 0x55)) });
	ghBtn->setClickedCallback([] {
		QDesktopServices::openUrl(QUrl(u"https://github.com/amethyst-bin/MargeletDesktop"_q));
	});

	Ui::ResizeFitChild(this, content);
}

::Settings::Type MargySettingsId() {
	return MargySettingsSection::Id();
}

} // namespace Margy::Settings
