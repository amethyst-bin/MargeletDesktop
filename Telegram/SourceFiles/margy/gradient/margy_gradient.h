#pragma once

#include <QString>
#include <QColor>
#include <QRect>
#include <QDialog>

class QPainter;

namespace Margy::Gradient {

struct Colors {
	QColor first;
	QColor second;
	bool valid = false;
};

[[nodiscard]] Colors Parse(const QString &text);
[[nodiscard]] QString Format(const QColor &first, const QColor &second);
[[nodiscard]] Colors ForPeer(int64_t peerId);
void Paint(QPainter &p, const QRect &rect, const Colors &colors);

class GradientBox final : public QDialog {
	Q_OBJECT

public:
	explicit GradientBox(QWidget *parent);
	~GradientBox() override = default;

	static void Show(QWidget *parent);

private:
	void setupUi();
	void updatePreview();

	QColor _first = QColor(0x8D, 0xD1, 0xB0);
	QColor _second = QColor(0xB7, 0xA8, 0xE0);
	QWidget *_preview = nullptr;
};

} // namespace Margy::Gradient
