#pragma once

#include <QDialog>

namespace Margy::Badges {

class BadgeGalleryBox final : public QDialog {
	Q_OBJECT

public:
	explicit BadgeGalleryBox(QWidget *parent);
	~BadgeGalleryBox() override = default;

	static void Show(QWidget *parent);

private:
	void setupUi();
};

} // namespace Margy::Badges
