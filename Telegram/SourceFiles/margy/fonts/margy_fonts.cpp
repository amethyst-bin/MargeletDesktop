#include "margy/fonts/margy_fonts.h"
#include "margy/margy_config.h"
#include "boxes/abstract_box.h"
#include "chat_helpers/emoji_sets_manager.h"
#include "ui/vertical_list.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/ui_utility.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"
#include "main/main_session.h"
#include "core/application.h"

#include <QFontDatabase>
#include <QGuiApplication>
#include <QFont>

namespace Margy::Fonts {

QStringList AvailableEmojiFontPacks() {
	return {
		u"Default (Apple)"_q,
		u"Twemoji (Twitter)"_q,
		u"JoyPixels"_q,
		u"Android (Google)"_q,
	};
}

void ApplyConfiguredFonts() {
	const auto customFamily = Config::Instance().customFont();
	if (!customFamily.isEmpty()) {
		auto font = QGuiApplication::font();
		font.setFamily(customFamily);
		QGuiApplication::setFont(font);
	}
}

FontsBox::FontsBox(QWidget *parent) {
}

void FontsBox::Show(QWidget *parent) {
	::Ui::show(::Box<FontsBox>());
}

void FontsBox::prepare() {
	setTitle(rpl::single(u"Шрифты и эмодзи Margy"_q));
	setDimensions(st::boxWideWidth, 420);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			u"Наборы эмодзи (Twemoji, JoyPixels, Apple, Android)"_q,
			st::boxTitle),
		st::boxRowPadding);

	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			u"Telegram Desktop использует растровые наборы спрайтов для эмодзи. "
			u"Вы можете загрузить и переключить активный набор эмодзи в нативном менеджере:"_q,
			st::boxLabel),
		st::boxRowPadding);

	const auto emojiBtn = content->add(
		object_ptr<::Ui::SettingsButton>(
			content,
			rpl::single(u"Открыть менеджер наборов эмодзи"_q),
			st::settingsButton),
		st::boxRowPadding);
	emojiBtn->setClickedCallback([=] {
		if (const auto session = Core::App().maybePrimarySession()) {
			::Ui::show(::Box<::Ui::Emoji::ManageSetsBox>(not_null{ session }));
		}
	});

	::Ui::AddSkip(content);
	::Ui::AddDivider(content);

	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			u"Margy Desktop\nСъешь ещё этих мягких французских булок\n0123456789"_q,
			st::boxLabel),
		st::boxRowPadding,
		style::al_center);

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

} // namespace Margy::Fonts
