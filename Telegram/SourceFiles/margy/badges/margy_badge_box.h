#pragma once

#include "margy/badges/margy_badge_types.h"
#include "ui/layers/box_content.h"

namespace Margy::Badges {

class BadgeBox final : public ::Ui::BoxContent {
public:
	explicit BadgeBox(QWidget *parent, const Badge &badge);
	~BadgeBox() override;

	static void Show(QWidget *parent, const Badge &badge);
	static void Show(QWidget *parent, int64_t peerId);

protected:
	void prepare() override;

private:
	Badge _badge;
};

} // namespace Margy::Badges
