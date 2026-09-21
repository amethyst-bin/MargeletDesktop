#include "margy/donate/margy_donate_box.h"
#include "boxes/abstract_box.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/ui_utility.h"
#include "ui/toast/toast.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

#include <QGuiApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

namespace Margy::Donate {

DonateBox::DonateBox(QWidget *parent) {
}

void DonateBox::Show(QWidget *parent) {
	::Ui::show(::Box<DonateBox>());
}

void DonateBox::prepare() {
	setTitle(rpl::single(u"Поддержка Margy 💖"_q));
	setDimensions(st::boxWideWidth, 480);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			u"Margy — бесплатный и открытый клиент, созданный с душой. "
			u"Ваша поддержка мотивирует развивать проект!"_q,
			st::boxLabel),
		st::boxRowPadding,
		style::al_center);

	// ЮMoney
	const auto yooBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"💳 ЮMoney: 4100118596660144"_q),
			st::settingsButton),
		st::boxRowPadding);
	yooBtn->setClickedCallback([=] {
		QGuiApplication::clipboard()->setText(u"4100118596660144"_q);
		::Ui::Toast::Show(this, u"Номер ЮMoney скопирован!"_q);
	});

	// Roblox
	const auto robloxBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"🎮 Roblox: @narezany"_q),
			st::settingsButton),
		st::boxRowPadding);
	robloxBtn->setClickedCallback([=] {
		QDesktopServices::openUrl(QUrl(u"https://www.roblox.com/users/3358826725/profile"_q));
	});

	// TON
	const auto tonBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"💎 TON: UQ... (нажмите для копирования)"_q),
			st::settingsButton),
		st::boxRowPadding);
	tonBtn->setClickedCallback([=] {
		QGuiApplication::clipboard()->setText(u"UQBhfT8i-K3eJ1pY6sH3Z6iY_8c6C4L2r-D1K7B9x"_q);
		::Ui::Toast::Show(this, u"TON адрес скопирован!"_q);
	});

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

} // namespace Margy::Donate
