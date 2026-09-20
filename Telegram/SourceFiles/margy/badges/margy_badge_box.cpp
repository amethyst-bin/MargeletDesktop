#include "margy/badges/margy_badge_box.h"
#include "margy/badges/margy_plane_3d.h"
#include "margy/badges/margy_badge_manager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QLocale>

namespace Margy::Badges {

BadgeBox::BadgeBox(QWidget *parent, const Badge &badge)
: QDialog(parent) {
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setupUi(badge);
}

BadgeBox::~BadgeBox() = default;

void BadgeBox::Show(QWidget *parent, const Badge &badge) {
	auto *box = new BadgeBox(parent, badge);
	box->setAttribute(Qt::WA_DeleteOnClose);
	box->show();
}

void BadgeBox::Show(QWidget *parent, int64_t peerId) {
	if (const auto badge = Of(peerId)) {
		Show(parent, *badge);
	}
}

void BadgeBox::setupUi(const Badge &badge) {
	const bool isRu = QLocale::system().name().startsWith("ru", Qt::CaseInsensitive);

	setWindowTitle(badge.title(isRu));
	setMinimumWidth(320);

	auto *layout = new QVBoxLayout(this);
	layout->setContentsMargins(24, 20, 24, 20);
	layout->setSpacing(16);

	// Title
	auto *titleLabel = new QLabel(badge.title(isRu), this);
	titleLabel->setAlignment(Qt::AlignCenter);
	auto font = titleLabel->font();
	font.setPointSize(font.pointSize() + 3);
	font.setBold(true);
	titleLabel->setFont(font);
	layout->addWidget(titleLabel);

	// 3D Plane widget
	auto *plane = new Plane3D(this, badge.color);
	plane->setFixedSize(160, 160);
	layout->addWidget(plane, 0, Qt::AlignCenter);

	// About description
	auto *aboutLabel = new QLabel(badge.about(isRu), this);
	aboutLabel->setAlignment(Qt::AlignCenter);
	aboutLabel->setWordWrap(true);
	layout->addWidget(aboutLabel);

	// Buttons
	auto *buttonsLayout = new QHBoxLayout();
	buttonsLayout->setSpacing(12);

	if (!badge.url.isEmpty()) {
		const QString actionText = isRu ? "Перейти" : "Open";
		auto *actionButton = new QPushButton(actionText, this);
		actionButton->setCursor(Qt::PointingHandCursor);
		connect(actionButton, &QPushButton::clicked, this, [url = badge.url] {
			QDesktopServices::openUrl(QUrl(url));
		});
		buttonsLayout->addWidget(actionButton);
	}

	const QString closeText = isRu ? "Закрыть" : "Close";
	auto *closeButton = new QPushButton(closeText, this);
	closeButton->setCursor(Qt::PointingHandCursor);
	connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
	buttonsLayout->addWidget(closeButton);

	layout->addLayout(buttonsLayout);
}

} // namespace Margy::Badges
