#pragma once

#include <QString>
#include <QStringList>
#include <QDialog>

namespace Margy::Fonts {

void ApplyConfiguredFonts();
[[nodiscard]] QStringList AvailableEmojiFontPacks();

class FontsBox final : public QDialog {
	Q_OBJECT

public:
	explicit FontsBox(QWidget *parent);
	~FontsBox() override = default;

	static void Show(QWidget *parent);

private:
	void setupUi();
};

} // namespace Margy::Fonts
