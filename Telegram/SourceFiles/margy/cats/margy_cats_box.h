#pragma once

#include "margy/cats/margy_cats_manager.h"
#include "ui/layers/box_content.h"

namespace Ui {
class FlatLabel;
} // namespace Ui

namespace Margy::Cats {

class CatsBox final : public ::Ui::BoxContent {
public:
	explicit CatsBox(QWidget *parent = nullptr);
	~CatsBox() override = default;

	static void Show(QWidget *parent = nullptr);

protected:
	void prepare() override;

private:
	void showCat(const Cat &cat);

	::Ui::RpWidget *_photoWrap = nullptr;
	::Ui::FlatLabel *_nameLabel = nullptr;
	::Ui::FlatLabel *_fromLabel = nullptr;
	Cat _currentCat;
	QPixmap _currentPixmap;
};

} // namespace Margy::Cats
