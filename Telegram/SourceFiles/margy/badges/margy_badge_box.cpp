#include "margy/badges/margy_badge_box.h"
#include "margy/badges/margy_plane_3d.h"
#include "margy/badges/margy_badge_manager.h"
#include "boxes/abstract_box.h"
#include "ui/layers/generic_box.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/ui_utility.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"

#include <QDesktopServices>
#include <QUrl>
#include <QLocale>

namespace Margy::Badges {

BadgeBox::BadgeBox(QWidget *parent, const Badge &badge)
: _badge(badge) {
}

BadgeBox::~BadgeBox() = default;

void BadgeBox::Show(QWidget *parent, const Badge &badge) {
	::Ui::show(::Box<BadgeBox>(badge));
}

void BadgeBox::Show(QWidget *parent, int64_t peerId) {
	if (const auto badge = Of(peerId)) {
		Show(parent, *badge);
	}
}

void BadgeBox::prepare() {
	const auto isRu = QLocale::system().name().startsWith(u"ru"_q, Qt::CaseInsensitive);

	setTitle(rpl::single(_badge.title(isRu)));
	setDimensions(st::boxWideWidth, 360);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	// 3D Plane widget in center
	const auto planeWrap = content->add(
		object_ptr<::Ui::FixedHeightWidget>(content, 160));
	const auto plane = ::Ui::CreateChild<Plane3D>(planeWrap, _badge.color);
	plane->resize(160, 160);
	planeWrap->widthValue(
	) | rpl::on_next([=](int w) {
		plane->move((w - 160) / 2, 0);
	}, planeWrap->lifetime());

	// Description label
	content->add(
		object_ptr<::Ui::FlatLabel>(
			content,
			_badge.about(isRu),
			st::boxLabel),
		st::boxRowPadding,
		style::al_center);

	if (!_badge.url.isEmpty()) {
		const auto actionText = isRu ? u"Перейти"_q : u"Open"_q;
		addButton(rpl::single(actionText), [url = _badge.url] {
			QDesktopServices::openUrl(QUrl(url));
		});
	}

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

} // namespace Margy::Badges
