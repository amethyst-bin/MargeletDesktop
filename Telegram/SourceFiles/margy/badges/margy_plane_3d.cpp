#include "margy/badges/margy_plane_3d.h"
#include "margy/badges/margy_badge_icons_data.h"
#include "margy/seizure/margy_seizure.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDateTime>
#include <QTimer>
#include <cmath>
#include <algorithm>

namespace Margy::Badges {
namespace {

constexpr float kHalfDepth = 0.15f;
constexpr float kHalfSize = 1.0f;
constexpr float kCorner = 0.24f;
constexpr int kCornerSteps = 8;
constexpr float kCamZ = 3.4f;
constexpr float kTilt = -10.0f;

const auto kWingLeft = QColor(0xFF, 0xFF, 0xFF);
const auto kWingRight = QColor(0xEE, 0xF3, 0xFA);
const auto kKeel = QColor(0xCC, 0xD5, 0xE9);

std::array<float, 3> Normalize(float x, float y, float z) {
	const float len = std::sqrt(x * x + y * y + z * z);
	if (len <= 0.00001f) {
		return { 0.0f, 0.0f, 1.0f };
	}
	return { x / len, y / len, z / len };
}

const auto kLight = Normalize(0.35f, 0.8f, 0.6f);

int ClampColor(float val) {
	return std::clamp(static_cast<int>(std::round(val)), 0, 255);
}

QColor MakeDarker(const QColor &c) {
	return QColor(
		ClampColor(c.red() * 0.877f),
		ClampColor(c.green() * 0.877f),
		ClampColor(c.blue() * 0.877f),
		c.alpha());
}

QColor Shade(const QColor &c, float nx, float ny, float nz) {
	const float dot = nx * kLight[0] + ny * kLight[1] + nz * kLight[2];
	const float top = std::max(dot, 0.0f);
	const float fill = std::max(-dot, 0.0f);
	const float light = 0.62f + 0.38f * top + 0.12f * fill;
	return QColor(
		ClampColor(c.red() * light),
		ClampColor(c.green() * light),
		ClampColor(c.blue() * light),
		c.alpha());
}

std::vector<std::array<float, 2>> Outline() {
	std::vector<std::array<float, 2>> points;
	const float s = kHalfSize - kCorner;
	const float centers[4][2] = { { s, s }, { -s, s }, { -s, -s }, { s, -s } };
	const float starts[4] = { 0.0f, 90.0f, 180.0f, 270.0f };

	constexpr double kPi = 3.14159265358979323846;
	for (int c = 0; c < 4; ++c) {
		for (int i = 0; i <= kCornerSteps; ++i) {
			const double a = (starts[c] + 90.0 * i / kCornerSteps) * (kPi / 180.0);
			points.push_back({
				centers[c][0] + static_cast<float>(std::cos(a)) * kCorner,
				centers[c][1] + static_cast<float>(std::sin(a)) * kCorner
			});
		}
	}
	return points;
}

void Rotate(
		float x, float y, float z,
		float sinA, float cosA,
		float sinT, float cosT,
		std::array<float, 3> &out) {
	const float rx = x * cosA + z * sinA;
	const float rz = -x * sinA + z * cosA;
	out[0] = rx;
	out[1] = y * cosT - rz * sinT;
	out[2] = y * sinT + rz * cosT;
}

} // namespace

Plane3D::Plane3D(QWidget *parent, const QColor &color, const QString &customIconId)
: Ui::RpWidget(parent)
, _field(color)
, _side(MakeDarker(color))
, _customIconId(customIconId) {
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	build();
	resize(width(), 160);

	_timer = new QTimer(this);
	connect(_timer, &QTimer::timeout, this, [this] {
		if (_spinning) {
			update();
		}
	});
	_timer->start(16); // ~60 FPS
}

Plane3D::~Plane3D() = default;

void Plane3D::setColor(const QColor &color) {
	if (_field != color) {
		_field = color;
		_side = MakeDarker(color);
		build();
		update();
	}
}

QSize Plane3D::sizeHint() const {
	return QSize(150, 150);
}

QSize Plane3D::minimumSizeHint() const {
	return QSize(60, 60);
}

void Plane3D::build() {
	_pieces.clear();
	if (_customIconId == u"kent"_q) {
		return;
	}
	const auto ring = Outline();
	const int n = static_cast<int>(ring.size());

	std::vector<std::array<float, 3>> front(n);
	std::vector<std::array<float, 3>> back(n);
	for (int i = 0; i < n; ++i) {
		front[i] = { ring[i][0], ring[i][1], kHalfDepth };
		back[n - 1 - i] = { ring[i][0], ring[i][1], -kHalfDepth };
	}
	_pieces.push_back(Piece{ std::move(front), { 0.0f, 0.0f, 1.0f }, _field, false });
	_pieces.push_back(Piece{ std::move(back), { 0.0f, 0.0f, -1.0f }, _field, false });

	for (int i = 0; i < n; ++i) {
		const auto &p1 = ring[i];
		const auto &p2 = ring[(i + 1) % n];
		const float dx = p2[0] - p1[0];
		const float dy = p2[1] - p1[1];
		const float len = std::sqrt(dx * dx + dy * dy);
		if (len <= 0.00001f) {
			continue;
		}
		const std::array<float, 3> normal{ dy / len, -dx / len, 0.0f };
		std::vector<std::array<float, 3>> quad = {
			{ p1[0], p1[1], kHalfDepth },
			{ p2[0], p2[1], kHalfDepth },
			{ p2[0], p2[1], -kHalfDepth },
			{ p1[0], p1[1], -kHalfDepth }
		};
		_pieces.push_back(Piece{ std::move(quad), normal, _side, false });
	}

	addPlane(kHalfDepth + 0.004f, { 0.0f, 0.0f, 1.0f }, false);
	addPlane(-kHalfDepth - 0.004f, { 0.0f, 0.0f, -1.0f }, true);
}

void Plane3D::addPlane(float z, const std::array<float, 3> &normal, bool mirror) {
	const float k = mirror ? -1.0f : 1.0f;
	const float up = -0.08f;
	const float m = 1.30f;

	const std::array<float, 3> nose{ 0.0f, 0.56f * m + up, z };
	const std::array<float, 3> left{ -0.52f * m * k, -0.30f * m + up, z };
	const std::array<float, 3> right{ 0.52f * m * k, -0.30f * m + up, z };
	const std::array<float, 3> keelL{ -0.04f * m * k, -0.14f * m + up, z };
	const std::array<float, 3> keelR{ 0.04f * m * k, -0.14f * m + up, z };
	const std::array<float, 3> tail{ 0.0f, -0.24f * m + up, z };

	_pieces.push_back(Piece{
		{ nose, left, keelL },
		normal,
		mirror ? kWingRight : kWingLeft,
		true
	});
	_pieces.push_back(Piece{
		{ nose, keelR, right },
		normal,
		mirror ? kWingLeft : kWingRight,
		true
	});
	_pieces.push_back(Piece{
		{ nose, keelL, tail, keelR },
		normal,
		kKeel,
		true
	});
}

void Plane3D::mousePressEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		_lastX = e->position().x();
		_dragging = true;
		_spinning = false;
	}
	QWidget::mousePressEvent(e);
}

void Plane3D::mouseMoveEvent(QMouseEvent *e) {
	if (_dragging) {
		const float currentX = e->position().x();
		_angle += (currentX - _lastX) * 0.5f;
		_lastX = currentX;
		update();
	}
	QWidget::mouseMoveEvent(e);
}

void Plane3D::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		_dragging = false;
		_spinning = true;
		_lastFrame = QDateTime::currentMSecsSinceEpoch();
	}
	QWidget::mouseReleaseEvent(e);
}

void Plane3D::paintEvent(QPaintEvent *) {
	const auto now = QDateTime::currentMSecsSinceEpoch();
	if (_lastFrame != 0 && _spinning) {
		_angle += (now - _lastFrame) * 0.04f;
	}
	_lastFrame = now;
	if (_angle > 360.0f) {
		_angle = std::fmod(_angle, 360.0f);
	}

	const int w = width();
	const int h = height();
	if (w <= 0 || h <= 0) {
		return;
	}

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	const float cx = w / 2.0f;
	const float cy = h / 2.0f;
	const float focal = kCamZ * std::min(w, h) * 0.34f;

	constexpr double kPi = 3.14159265358979323846;
	const double a = _angle * (kPi / 180.0);
	const double t = kTilt * (kPi / 180.0);
	const float sinA = static_cast<float>(std::sin(a));
	const float cosA = static_cast<float>(std::cos(a));
	const float sinT = static_cast<float>(std::sin(t));
	const float cosT = static_cast<float>(std::cos(t));

	std::array<float, 3> tmp{};

	if (_customIconId == u"kent"_q) {
		const auto img = GetKentBadgeImage();
		if (img.isNull()) {
			return;
		}

		p.setRenderHint(QPainter::SmoothPixmapTransform);

		static const auto kBackImg = img.mirrored(true, false);
		static const auto kEdgeImg = [&] {
			auto edge = img;
			for (int y = 0; y < edge.height(); ++y) {
				auto line = reinterpret_cast<QRgb*>(edge.scanLine(y));
				for (int x = 0; x < edge.width(); ++x) {
					const auto a = qAlpha(line[x]);
					if (a > 0) {
						const auto r = qRed(line[x]) * 6 / 10;
						const auto g = qGreen(line[x]) * 6 / 10;
						const auto b = qBlue(line[x]) * 6 / 10;
						line[x] = qRgba(r, g, b, a);
					}
				}
			}
			return edge;
		}();

		constexpr auto kSlices = 7;
		constexpr auto kDepth = 0.14f;
		constexpr auto kHalfImgSize = 1.05f;

		struct SliceInfo {
			int index = 0;
			float z = 0.0f;
			float camDist = 0.0f;
		};
		std::array<SliceInfo, kSlices> slices{};
		for (int i = 0; i < kSlices; ++i) {
			const auto z = -kDepth + 2.0f * kDepth * (float(i) / float(kSlices - 1));
			Rotate(0.0f, 0.0f, z, sinA, cosA, sinT, cosT, tmp);
			slices[i] = { i, z, kCamZ - tmp[2] };
		}
		std::sort(slices.begin(), slices.end(), [](const SliceInfo &a, const SliceInfo &b) {
			return a.camDist > b.camDist;
		});

		Rotate(0.0f, 0.0f, 1.0f, sinA, cosA, sinT, cosT, tmp);
		const auto frontFacing = (tmp[2] > 0.0f);

		const auto imgW = float(img.width());
		const auto imgH = float(img.height());
		const auto srcQuad = QPolygonF{
			QPointF(0.0, 0.0),
			QPointF(imgW, 0.0),
			QPointF(imgW, imgH),
			QPointF(0.0, imgH)
		};

		for (const auto &slice : slices) {
			const auto z = slice.z;
			const std::array<float, 3> corners[4] = {
				{ -kHalfImgSize, kHalfImgSize, z },
				{ kHalfImgSize, kHalfImgSize, z },
				{ kHalfImgSize, -kHalfImgSize, z },
				{ -kHalfImgSize, -kHalfImgSize, z }
			};

			QPolygonF dstQuad;
			dstQuad.reserve(4);
			for (const auto &c : corners) {
				Rotate(c[0], c[1], c[2], sinA, cosA, sinT, cosT, tmp);
				const auto denom = std::max(kCamZ - tmp[2], 0.1f);
				const auto sx = cx + tmp[0] * focal / denom;
				const auto sy = cy - tmp[1] * focal / denom;
				dstQuad.append(QPointF(sx, sy));
			}

			const auto &drawImg = (slice.index == kSlices - 1)
				? (frontFacing ? img : kBackImg)
				: (slice.index == 0)
					? (frontFacing ? kBackImg : img)
					: kEdgeImg;

			QTransform xform;
			if (QTransform::quadToQuad(srcQuad, dstQuad, xform)) {
				p.save();
				p.setTransform(xform, true);
				p.drawImage(0, 0, drawImg);
				p.restore();
			}
		}
		return;
	}

	for (int pass = 0; pass < 2; ++pass) {
		for (const auto &piece : _pieces) {
			if (piece.decal != (pass == 1)) {
				continue;
			}
			Rotate(piece.normal[0], piece.normal[1], piece.normal[2],
				sinA, cosA, sinT, cosT, tmp);
			const float nx = tmp[0];
			const float ny = tmp[1];
			const float nz = tmp[2];

			float toCamera = 0.0f;
			QPolygonF polygon;
			polygon.reserve(piece.points.size());

			for (const auto &pt : piece.points) {
				Rotate(pt[0], pt[1], pt[2], sinA, cosA, sinT, cosT, tmp);
				toCamera += nx * -tmp[0] + ny * -tmp[1] + nz * (kCamZ - tmp[2]);
				const float denom = std::max(kCamZ - tmp[2], 0.1f);
				const float sx = cx + tmp[0] * focal / denom;
				const float sy = cy - tmp[1] * focal / denom;
				polygon.append(QPointF(sx, sy));
			}

			if (toCamera <= 0.0f) {
				continue; // Backface culling
			}

			auto baseColor = piece.color;
			if (Margy::Seizure::IsEnabled() && piece.decal) {
				baseColor = Margy::Seizure::CurrentColor();
			}
			const auto color = Shade(baseColor, nx, ny, nz);
			p.setPen(QPen(color, 1.0f));
			p.setBrush(color);
			p.drawPolygon(polygon);
		}
	}
}

} // namespace Margy::Badges
