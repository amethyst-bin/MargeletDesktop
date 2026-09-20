#pragma once

#include "margy/badges/margy_badge_types.h"

#include <QDialog>
#include <memory>

namespace Margy::Badges {

class BadgeBox final : public QDialog {
	Q_OBJECT

public:
	explicit BadgeBox(QWidget *parent, const Badge &badge);
	~BadgeBox() override;

	static void Show(QWidget *parent, const Badge &badge);
	static void Show(QWidget *parent, int64_t peerId);

private:
	void setupUi(const Badge &badge);
};

} // namespace Margy::Badges
