#pragma once

#include "ui/layers/box_content.h"
#include <QString>
#include <QStringList>

namespace Margy::Fonts {

void ApplyConfiguredFonts();
[[nodiscard]] QStringList AvailableEmojiFontPacks();

class FontsBox final : public ::Ui::BoxContent {
public:
	explicit FontsBox(QWidget *parent = nullptr);
	~FontsBox() override = default;

	static void Show(QWidget *parent = nullptr);

protected:
	void prepare() override;
};

} // namespace Margy::Fonts
