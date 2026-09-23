#pragma once

#include "margy/plugins/margy_plugin_types.h"

#include <QtWidgets/QWidget>
#include <rpl/lifetime.h>

namespace Margy::Plugins {

class SnowOverlay final : public QWidget {
public:
	static void Attach(QWidget *parent);

	explicit SnowOverlay(QWidget *parent);
	~SnowOverlay() override = default;

protected:
	void paintEvent(QPaintEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	std::vector<SnowParticle> _particles;
	rpl::lifetime _lifetime;
};

} // namespace Margy::Plugins
