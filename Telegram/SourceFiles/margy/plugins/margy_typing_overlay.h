#pragma once

#include "margy/plugins/margy_plugin_types.h"

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <rpl/lifetime.h>

namespace Margy::Plugins {

class TypingOverlay final : public QWidget {
public:
	static void Attach(QWidget *inputField, const QString &fieldId);

	TypingOverlay(QWidget *parent, const QString &fieldId);
	~TypingOverlay() override = default;

protected:
	void paintEvent(QPaintEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *e) override;

private:
	void handleFrame(const TypingAnimFrame &frame);

	QString _fieldId;
	std::vector<SparkParticle> _sparks;
	CursorGlideState _cursor;
	QTimer _animTimer;
	bool _animating = false;
	rpl::lifetime _lifetime;
};

} // namespace Margy::Plugins
