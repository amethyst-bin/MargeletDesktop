#include "margy/plugins/margy_typing_overlay.h"
#include "margy/plugins/margy_plugin_hooks.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QEvent>
#include <algorithm>

namespace Margy::Plugins {

void TypingOverlay::Attach(QWidget *inputField, const QString &fieldId) {
	if (!inputField) {
		return;
	}
	const auto existing = inputField->findChild<TypingOverlay*>();
	if (!existing) {
		new TypingOverlay(inputField, fieldId);
	}
}

TypingOverlay::TypingOverlay(QWidget *parent, const QString &fieldId)
: QWidget(parent)
, _fieldId(fieldId) {
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	if (parent) {
		setGeometry(parent->rect());
		parent->installEventFilter(this);
	}

	_animTimer.setSingleShot(true);
	QObject::connect(&_animTimer, &QTimer::timeout, [=] {
		if (_animating && Config::Instance().pluginsEnabled() && Config::Instance().pluginHooksEnabled()) {
			Hooks::OnInputStep(_fieldId);
		}
	});

	Host::Instance().typingAnimation(_fieldId) | rpl::on_next([=](const TypingAnimFrame &frame) {
		handleFrame(frame);
	}, _lifetime);

	show();
	raise();
}

bool TypingOverlay::eventFilter(QObject *obj, QEvent *e) {
	if (obj == parentWidget()) {
		if (e->type() == QEvent::Resize || e->type() == QEvent::Move) {
			setGeometry(parentWidget()->rect());
			raise();
		}
	}
	return QWidget::eventFilter(obj, e);
}

void TypingOverlay::handleFrame(const TypingAnimFrame &frame) {
	_sparks = frame.sparks;
	_cursor = frame.cursor;
	_animating = frame.hasAnimation;

	update();

	if (_animating) {
		_animTimer.start(16);
	}
}

void TypingOverlay::paintEvent(QPaintEvent *e) {
	if (_sparks.empty() && !_cursor.active) {
		return;
	}

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	for (const auto &spark : _sparks) {
		const auto alpha = std::clamp(spark.alpha, 0.0f, 1.0f);
		if (alpha <= 0.001f) {
			continue;
		}
		auto color = QColor::fromRgba(QRgb(spark.color));
		color.setAlphaF(alpha);
		p.setPen(Qt::NoPen);
		p.setBrush(color);
		p.drawEllipse(QPointF(spark.x, spark.y), spark.radius, spark.radius);
	}

	if (_cursor.active && _cursor.height > 0) {
		p.setPen(Qt::NoPen);
		p.setBrush(QColor(255, 255, 255, 200));
		p.drawRoundedRect(QRectF(_cursor.x, _cursor.y, 2.0, _cursor.height), 1.0, 1.0);
	}
}

} // namespace Margy::Plugins
