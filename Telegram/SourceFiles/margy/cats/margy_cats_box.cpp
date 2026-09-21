#include "margy/cats/margy_cats_box.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QFile>
#include <QTimer>

namespace Margy::Cats {

CatsBox::CatsBox(QWidget *parent)
: QDialog(parent) {
	setupUi();
	showCat(CatsManager::Instance().randomCat());
}

void CatsBox::Show(QWidget *parent) {
	auto box = new CatsBox(parent);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void CatsBox::setupUi() {
	setWindowTitle(u"Коты Margy 🐾"_q);
	setFixedSize(380, 480);

	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(16, 16, 16, 16);
	layout->setSpacing(10);

	_photoLabel = new QLabel(this);
	_photoLabel->setFixedSize(348, 280);
	_photoLabel->setAlignment(Qt::AlignCenter);
	_photoLabel->setStyleSheet(
		u"background-color: #1c1c1e; border-radius: 12px; color: #888888;"_q);
	layout->addWidget(_photoLabel);

	_nameLabel = new QLabel(this);
	_nameLabel->setAlignment(Qt::AlignCenter);
	_nameLabel->setStyleSheet(u"font-size: 20px; font-weight: bold; color: white;"_q);
	layout->addWidget(_nameLabel);

	_fromLabel = new QLabel(this);
	_fromLabel->setAlignment(Qt::AlignCenter);
	_fromLabel->setStyleSheet(u"font-size: 13px; color: #a0a0a0;"_q);
	layout->addWidget(_fromLabel);

	const auto buttonsLayout = new QHBoxLayout();

	const auto addCatBtn = new QPushButton(u"Добавить своего"_q, this);
	addCatBtn->setStyleSheet(u"color: #8dd1b0; font-weight: bold;"_q);
	connect(addCatBtn, &QPushButton::clicked, this, [] {
		QDesktopServices::openUrl(QUrl(u"https://t.me/narezany?text=%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82!%20%D0%AF%20%D1%85%D0%BE%D1%87%D1%83%20%D0%B4%D0%BE%D0%B1%D0%B0%D0%B2%D0%B8%D1%82%D1%8C%20%D1%81%D0%B2%D0%BE%D0%B5%D0%B3%D0%BE%20%D0%BA%D0%BE%D1%82%D0%B8%D0%BA%D0%B0%20%D0%B2%20Margy"_q));
	});
	buttonsLayout->addWidget(addCatBtn);

	const auto nextBtn = new QPushButton(u"Ещё котик 🐱"_q, this);
	connect(nextBtn, &QPushButton::clicked, this, [=] {
		showCat(CatsManager::Instance().randomCat());
	});
	buttonsLayout->addWidget(nextBtn);

	layout->addLayout(buttonsLayout);

	const auto closeBtn = new QPushButton(u"Закрыть"_q, this);
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
	layout->addWidget(closeBtn);
}

void CatsBox::showCat(const Cat &cat) {
	_currentCat = cat;
	_nameLabel->setText(cat.nameRu.isEmpty() ? cat.name : cat.nameRu);
	_fromLabel->setText(u"От: "_q + cat.from);

	const auto localPath = CatsManager::Instance().localPhotoPath(cat);
	if (!localPath.isEmpty() && QFile::exists(localPath)) {
		auto pixmap = QPixmap(localPath);
		if (!pixmap.isNull()) {
			_photoLabel->setPixmap(pixmap.scaled(
				_photoLabel->size(),
				Qt::KeepAspectRatioByExpanding,
				Qt::SmoothTransformation));
			return;
		}
	}

	_photoLabel->setText(u"Загрузка котика 🐾..."_q);

	// Recheck after small delay in case download finishes
	QTimer::singleShot(1500, this, [=] {
		if (_currentCat.photo == cat.photo) {
			const auto retryPath = CatsManager::Instance().localPhotoPath(cat);
			if (!retryPath.isEmpty() && QFile::exists(retryPath)) {
				auto retryPix = QPixmap(retryPath);
				if (!retryPix.isNull()) {
					_photoLabel->setPixmap(retryPix.scaled(
						_photoLabel->size(),
						Qt::KeepAspectRatioByExpanding,
						Qt::SmoothTransformation));
				}
			}
		}
	});
}

} // namespace Margy::Cats
