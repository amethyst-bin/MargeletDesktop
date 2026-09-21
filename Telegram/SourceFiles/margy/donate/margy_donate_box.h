#pragma once

#include "ui/layers/box_content.h"

namespace Margy::Donate {

class DonateBox final : public ::Ui::BoxContent {
public:
	explicit DonateBox(QWidget *parent = nullptr);
	~DonateBox() override = default;

	static void Show(QWidget *parent = nullptr);

protected:
	void prepare() override;
};

} // namespace Margy::Donate
