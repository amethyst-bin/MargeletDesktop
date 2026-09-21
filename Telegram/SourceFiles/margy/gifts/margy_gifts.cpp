#include "margy/gifts/margy_gifts.h"
#include "margy/margy_config.h"

namespace Margy::Gifts {

bool UnhideGiftsEnabled() {
	return Config::Instance().unhideGifts();
}

} // namespace Margy::Gifts
