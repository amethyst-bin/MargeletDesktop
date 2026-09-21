#include "margy/badges/margy_badge_gallery_box.h"
#include "margy/badges/margy_badge_manager.h"
#include "margy/badges/margy_badge_box.h"
#include "margy/badges/margy_badge_icon.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

namespace Margy::Badges {

BadgeGalleryBox::BadgeGalleryBox(QWidget *parent)
: QDialog(parent) {
	setupUi();
}

void BadgeGalleryBox::Show(QWidget *parent) {
	auto box = new BadgeGalleryBox(parent);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->open();
}

void BadgeGalleryBox::setupUi() {
	setWindowTitle(u"Галерея бейджей Margy"_q);
	resize(420, 520);

	const auto rootLayout = new QVBoxLayout(this);
	rootLayout->setContentsMargins(16, 16, 16, 16);
	rootLayout->setSpacing(12);

	const auto header = new QLabel(u"Все доступные бейджи сообщества:"_q, this);
	header->setStyleSheet(u"font-size: 16px; font-weight: bold; color: white;"_q);
	rootLayout->addWidget(header);

	const auto scroll = new QScrollArea(this);
	scroll->setWidgetResizable(true);
	scroll->setStyleSheet(u"background: transparent; border: none;"_q);

	const auto container = new QWidget(scroll);
	const auto listLayout = new QVBoxLayout(container);
	listLayout->setContentsMargins(0, 0, 0, 0);
	listLayout->setSpacing(8);

	const auto &badges = Manager::Instance().badges();
	for (const auto &badge : badges) {
		const auto row = new QWidget(container);
		row->setStyleSheet(
			u"background-color: #222226; border-radius: 8px; padding: 6px;"_q);
		const auto rowLayout = new QHBoxLayout(row);
		rowLayout->setContentsMargins(10, 8, 10, 8);

		const auto iconLabel = new QLabel(row);
		iconLabel->setFixedSize(24, 24);
		iconLabel->setPixmap(RenderBadgeIcon(badge.type, 24));
		rowLayout->addWidget(iconLabel);

		const auto infoLayout = new QVBoxLayout();
		infoLayout->setSpacing(2);

		const auto nameLabel = new QLabel(badge.name, row);
		nameLabel->setStyleSheet(u"font-size: 14px; font-weight: bold; color: white;"_q);
		infoLayout->addWidget(nameLabel);

		const auto roleLabel = new QLabel(badge.role, row);
		roleLabel->setStyleSheet(u"font-size: 12px; color: #a0a0a0;"_q);
		infoLayout->addWidget(roleLabel);

		rowLayout->addLayout(infoLayout);
		rowLayout->addStretch();

		const auto viewBtn = new QPushButton(u"3D вид"_q, row);
		viewBtn->setStyleSheet(
			u"background-color: #333338; color: #70b0ff; border-radius: 4px; padding: 4px 10px;"_q);
		connect(viewBtn, &QPushButton::clicked, this, [=, b = badge] {
			BadgeBox::Show(this, b);
		});
		rowLayout->addWidget(viewBtn);

		listLayout->addWidget(row);
	}
	listLayout->addStretch();

	scroll->setWidget(container);
	rootLayout->addWidget(scroll);

	const auto closeBtn = new QPushButton(u"Закрыть"_q, this);
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
	rootLayout->addWidget(closeBtn);
}

} // namespace Margy::Badges
