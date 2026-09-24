#pragma once

#include "margy/margy_config.h"
#include <QtGui/QImage>

namespace style {
namespace internal {
class IconMask;
} // namespace internal
} // namespace style

namespace Margy {

class IconPacks final {
public:
	[[nodiscard]] static IconPacks &Instance();

	void init();
	void setPack(IconPack pack);
	[[nodiscard]] IconPack currentPack() const;

	static QImage LookupHook(const style::internal::IconMask *mask, int scale, int ratio);

private:
	IconPacks();
	~IconPacks() = default;

	void ensureInitialized();
	[[nodiscard]] QImage renderSvg(const QByteArray &svg, int scale, int ratio) const;

	bool _initialized = false;
};

} // namespace Margy
