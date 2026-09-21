#include "margy/cats/margy_cats_box.h"
#include "boxes/abstract_box.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "ui/ui_utility.h"
#include "ui/painter.h"
#include "lang/lang_keys.h"
#include "styles/style_layers.h"
#include "styles/style_boxes.h"
#include "styles/style_settings.h"

#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include <QTimer>

namespace Margy::Cats {

CatsBox::CatsBox(QWidget *parent) {
}

void CatsBox::Show(QWidget *parent) {
	::Ui::show(::Box<CatsBox>());
}

void CatsBox::prepare() {
	setTitle(rpl::single(u"Коты Margy 🐾"_q));
	setDimensions(st::boxWideWidth, 480);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	_photoWrap = content->add(
		object_ptr<::Ui::FixedHeightWidget>(content, 260));
	_photoWrap->paintRequest(
	) | rpl::on_next([=](const QRect &clip) {
		Painter p(_photoWrap);
		p.fillRect(_photoWrap->rect(), QColor(0x1c, 0x1c, 0x1e));
		if (!_currentPixmap.isNull()) {
			const auto scaled = _currentPixmap.scaled(
				_photoWrap->size(),
				Qt::KeepAspectRatio,
				Qt::SmoothTransformation);
			p.drawPixmap(
				(_photoWrap->width() - scaled.width()) / 2,
				(_photoWrap->height() - scaled.height()) / 2,
				scaled);
		}
	}, _photoWrap->lifetime());

	_nameLabel = content->add(
		object_ptr<::Ui::FlatLabel>(content, QString(), st::boxTitle),
		st::boxRowPadding,
		style::al_center);

	_fromLabel = content->add(
		object_ptr<::Ui::FlatLabel>(content, QString(), st::boxDividerLabel),
		st::boxRowPadding,
		style::al_center);

	showCat(CatsManager::Instance().randomCat());

	addButton(rpl::single(u"Ещё котик 🐱"_q), [=] {
		showCat(CatsManager::Instance().randomCat());
	});

	addButton(rpl::single(u"Добавить своего"_q), [=] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/narezany?text=Hello!_I_want_to_add_my_cat_to_Margy"_q));
	});

	addButton(rpl::single(tr::lng_close(tr::now)), [=] {
		closeBox();
	});
}

void CatsBox::showCat(const Cat &cat) {
	_currentCat = cat;
	if (_nameLabel) {
		_nameLabel->setText(cat.nameRu.isEmpty() ? cat.name : cat.nameRu);
	}
	if (_fromLabel) {
		_fromLabel->setText(u"От: "_q + cat.from);
	}

	const auto localPath = CatsManager::Instance().localPhotoPath(cat);
	if (!localPath.isEmpty() && QFile::exists(localPath)) {
		_currentPixmap = QPixmap(localPath);
		if (_photoWrap) {
			_photoWrap->update();
		}
	} else {
		_currentPixmap = QPixmap();
		if (_photoWrap) {
			_photoWrap->update();
		}
		QTimer::singleShot(1500, this, [=] {
			if (_currentCat.photo == cat.photo) {
				const auto retryPath = CatsManager::Instance().localPhotoPath(cat);
				if (!retryPath.isEmpty() && QFile::exists(retryPath)) {
					_currentPixmap = QPixmap(retryPath);
					if (_photoWrap) {
						_photoWrap->update();
					}
				}
			}
		});
	}
}

} // namespace Margy::Cats
