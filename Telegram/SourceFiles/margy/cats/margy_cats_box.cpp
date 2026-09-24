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

class CatPhotoWidget final : public ::Ui::RpWidget {
public:
	explicit CatPhotoWidget(QWidget *parent = nullptr) : ::Ui::RpWidget(parent) {
		resize(width(), 260);
	}

	void setPixmap(const QPixmap &pixmap) {
		_pixmap = pixmap;
		update();
	}

	int resizeGetHeight(int newWidth) override {
		return 260;
	}

protected:
	void paintEvent(QPaintEvent *e) override {
		Painter p(this);
		const auto r = rect();
		p.fillRect(r, QColor(0x1c, 0x1c, 0x1e));
		if (!_pixmap.isNull()) {
			const auto scaled = _pixmap.scaled(
				r.size(),
				Qt::KeepAspectRatio,
				Qt::SmoothTransformation);
			p.drawPixmap(
				(r.width() - scaled.width()) / 2,
				(r.height() - scaled.height()) / 2,
				scaled);
		}
	}

private:
	QPixmap _pixmap;
};

CatsBox::CatsBox(QWidget *parent) {
}

void CatsBox::Show(QWidget *parent) {
	::Ui::show(::Box<CatsBox>());
}

void CatsBox::prepare() {
	setTitle(rpl::single(u"Коты Margy"_q));
	setDimensions(st::boxWideWidth, 480);

	const auto content = setInnerWidget(
		object_ptr<::Ui::VerticalLayout>(this));

	_photoWidget = content->add(
		object_ptr<CatPhotoWidget>(content));
	_photoWidget->resize(st::boxWideWidth, 260);
	_photoWidget->show();

	_nameLabel = content->add(
		object_ptr<::Ui::FlatLabel>(content, QString(), st::boxTitle),
		st::boxRowPadding,
		style::al_center);

	_fromLabel = content->add(
		object_ptr<::Ui::FlatLabel>(content, QString(), st::boxDividerLabel),
		st::boxRowPadding,
		style::al_center);

	showCat(CatsManager::Instance().randomCat());

	CatsManager::Instance().photoDownloaded(
	) | rpl::on_next([=](const QString &path) {
		if (_photoWidget && !_currentCat.photo.isEmpty() && path.endsWith(QFileInfo(_currentCat.photo).fileName())) {
			_currentPixmap = QPixmap(path);
			_photoWidget->setPixmap(_currentPixmap);
		}
	}, lifetime());

	addButton(rpl::single(u"Ещё котик"_q), [=] {
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
		if (_photoWidget) {
			_photoWidget->setPixmap(_currentPixmap);
		}
	} else {
		_currentPixmap = QPixmap();
		if (_photoWidget) {
			_photoWidget->setPixmap(QPixmap());
		}
		QTimer::singleShot(1500, this, [=] {
			if (_currentCat.photo == cat.photo) {
				const auto retryPath = CatsManager::Instance().localPhotoPath(cat);
				if (!retryPath.isEmpty() && QFile::exists(retryPath)) {
					_currentPixmap = QPixmap(retryPath);
					if (_photoWidget) {
						_photoWidget->setPixmap(_currentPixmap);
					}
				}
			}
		});
	}
}

} // namespace Margy::Cats
