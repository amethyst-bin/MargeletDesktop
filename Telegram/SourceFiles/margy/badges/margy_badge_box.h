#pragma once

#include <cstdint>

class QWidget;

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Margy::Badges {

struct Badge;

void InitBadgeBox(not_null<::Ui::GenericBox*> box, const Badge &badge);

class BadgeBox final {
public:
	static void Show(QWidget *parent, const Badge &badge);
	static void Show(QWidget *parent, int64_t peerId);
};

} // namespace Margy::Badges
