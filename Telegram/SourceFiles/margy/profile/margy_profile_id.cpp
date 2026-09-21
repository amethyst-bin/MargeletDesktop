#include "margy/profile/margy_profile_id.h"

namespace Margy::Profile {

QString FormatPeerId(PeerId peerId) {
	const auto bare = (peerId.value & PeerId::kChatTypeMask);
	if (peerId.is<UserId>()) {
		return QString::number(bare);
	} else if (peerId.is<ChannelId>()) {
		return u"-100"_q + QString::number(bare);
	} else if (peerId.is<ChatId>()) {
		return u"-"_q + QString::number(bare);
	}
	return QString::number(bare);
}

} // namespace Margy::Profile
