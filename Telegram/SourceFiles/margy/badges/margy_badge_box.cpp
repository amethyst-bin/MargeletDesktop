#include "margy/badges/margy_badge_box.h"
#include "margy/badges/margy_plane_3d.h"
#include "margy/badges/margy_badge_manager.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"

#include <QDesktopServices>
#include <QUrl>
#include <QLocale>

namespace Margy::Badges {

void InitBadgeBox(not_null<::Ui::GenericBox*> box, const Badge &badge) {
	const auto isRu = QLocale::system().name().startsWith(u"ru"_q, Qt::CaseInsensitive);

	box->setTitle(rpl::single(badge.title(isRu)));
	box->setWidth(st::boxWideWidth);

	// 3D Plane widget in center
	const auto plane = box->addRow(
		object_ptr<Plane3D>(box, badge.color));
	plane->resize(st::boxWideWidth, 160);
	plane->show();

	// Description label
	box->addRow(
		object_ptr<::Ui::FlatLabel>(
			box,
			badge.about(isRu),
			st::boxLabel),
		st::boxRowPadding,
		style::al_center);

	if (!badge.url.isEmpty()) {
		const auto actionText = isRu ? u"Перейти"_q : u"Open"_q;
		box->addButton(rpl::single(actionText), [url = badge.url] {
			QDesktopServices::openUrl(QUrl(url));
		});
	}

	box->addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		box->closeBox();
	});
}

void BadgeBox::Show(QWidget *parent, const Badge &badge) {
	::Ui::show(::Box(InitBadgeBox, badge));
}

void BadgeBox::Show(QWidget *parent, int64_t peerId) {
	if (const auto badge = Of(peerId)) {
		Show(parent, *badge);
	}
}

} // namespace Margy::Badges
