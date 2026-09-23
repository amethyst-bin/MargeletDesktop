#include "margy/plugins/margy_snow_overlay.h"
#include "margy/plugins/margy_plugin_host.h"
#include "margy/margy_config.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QEvent>

namespace Margy::Plugins {

void SnowOverlay::Attach(QWidget *parent) {
	if (!parent) {
		return;
	}
	for (const auto child : parent->children()) {
		if (child->objectName() == u"margy_snow_overlay"_q) {
			return;
		}
	}
	new SnowOverlay(parent);
}

SnowOverlay::SnowOverlay(QWidget *parent)
: QWidget(parent) {
	setObjectName(u"margy_snow_overlay"_q);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);

	if (parent) {
		setGeometry(parent->rect());
		parent->installEventFilter(this);
	}

	Host::Instance().snowAnimation() | rpl::on_next([=](std::vector<SnowParticle> particles) {
		_particles = std::move(particles);
		update();
	}, _lifetime);

	show();
	raise();
}

bool SnowOverlay::eventFilter(QObject *obj, QEvent *e) {
	if (obj == parentWidget()) {
		if (e->type() == QEvent::Resize || e->type() == QEvent::Move) {
			setGeometry(parentWidget()->rect());
			raise();
		}
	}
	return QWidget::eventFilter(obj, e);
}

void SnowOverlay::resizeEvent(QResizeEvent *e) {
	QWidget::resizeEvent(e);
	Host::Instance().onChatResize(width(), height());
}

void SnowOverlay::paintEvent(QPaintEvent *e) {
	if (_particles.empty()) {
		return;
	}

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(255, 255, 255, 210));

	for (const auto &particle : _particles) {
		p.drawEllipse(QPointF(particle.x, particle.y), particle.r, particle.r);
	}
}

} // namespace Margy::Plugins
