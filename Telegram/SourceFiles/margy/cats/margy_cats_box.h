#pragma once

#include "margy/cats/margy_cats_manager.h"

#include <QDialog>

class QLabel;

namespace Margy::Cats {

class CatsBox final : public QDialog {
	Q_OBJECT

public:
	explicit CatsBox(QWidget *parent);
	~CatsBox() override = default;

	static void Show(QWidget *parent);

private:
	void setupUi();
	void showCat(const Cat &cat);

	QLabel *_photoLabel = nullptr;
	QLabel *_nameLabel = nullptr;
	QLabel *_fromLabel = nullptr;
	Cat _currentCat;
};

} // namespace Margy::Cats
