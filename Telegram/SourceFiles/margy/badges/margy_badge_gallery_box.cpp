#include "margy/badges/margy_badge_gallery_box.h"
#include "margy/badges/margy_badge_manager.h"
#include "margy/badges/margy_badge_box.h"
#include "margy/badges/margy_badge_icon.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/ui_utility.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

namespace Margy::Badges {

BadgeGalleryBox::BadgeGalleryBox(QWidget *parent) {
}

void BadgeGalleryBox::Show(QWidget *parent) {
	::Ui::show(::Box<BadgeGalleryBox>());
}

void BadgeGalleryBox::prepare() {
	setTitle(rpl::single(u"Галерея бейджей Margy"_q));
	setDimensions(st::boxWideWidth, 480);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	const auto badges = Manager::Instance().list();
	for (const auto &badge : badges) {
		const auto btn = content->add(
			object_ptr<::Ui::SettingsButton>(
				content,
				rpl::single(badge.title() + u" — "_q + badge.about()),
				st::settingsButton),
			st::boxRowPadding);
		btn->setClickedCallback([=, b = badge] {
			BadgeBox::Show(nullptr, b);
		});
	}

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

} // namespace Margy::Badges
