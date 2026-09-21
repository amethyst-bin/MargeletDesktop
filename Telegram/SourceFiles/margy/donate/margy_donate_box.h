#pragma once

#include <QDialog>

namespace Margy::Donate {

class DonateBox final : public QDialog {
	Q_OBJECT

public:
	explicit DonateBox(QWidget *parent);
	~DonateBox() override = default;

	static void Show(QWidget *parent);

private:
	void setupUi();
};

} // namespace Margy::Donate
