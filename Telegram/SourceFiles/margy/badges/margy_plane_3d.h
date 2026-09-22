#pragma once

#include "ui/rp_widget.h"
#include <QColor>
#include <vector>
#include <array>

class QTimer;

namespace Margy::Badges {

class Plane3D final : public Ui::RpWidget {
	Q_OBJECT

public:
	explicit Plane3D(QWidget *parent = nullptr, const QColor &color = QColor(0x8D, 0xD1, 0xB0));
	~Plane3D() override;

	void setColor(const QColor &color);
	[[nodiscard]] QColor color() const { return _field; }

	[[nodiscard]] QSize sizeHint() const override;
	[[nodiscard]] QSize minimumSizeHint() const override;
	int resizeGetHeight(int newWidth) override { return 160; }

protected:
	void paintEvent(QPaintEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

private:
	struct Piece {
		std::vector<std::array<float, 3>> points;
		std::array<float, 3> normal{ 0.0f, 0.0f, 0.0f };
		QColor color;
		bool decal = false;
	};

	void build();
	void addPlane(float z, const std::array<float, 3> &normal, bool mirror);

	QColor _field;
	QColor _side;
	std::vector<Piece> _pieces;

	float _angle = 0.0f;
	qint64 _lastFrame = 0;
	bool _spinning = true;
	float _lastX = 0.0f;
	bool _dragging = false;

	QTimer *_timer = nullptr;
};

} // namespace Margy::Badges
