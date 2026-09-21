#pragma once

#include "ui/layers/box_content.h"

namespace Margy::Badges {

class BadgeGalleryBox final : public ::Ui::BoxContent {
public:
	explicit BadgeGalleryBox(QWidget *parent = nullptr);
	~BadgeGalleryBox() override = default;

	static void Show(QWidget *parent = nullptr);

protected:
	void prepare() override;
};

} // namespace Margy::Badges
