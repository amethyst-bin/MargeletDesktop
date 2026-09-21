#include "margy/profile/margy_profile_id.h"

namespace Margy::Profile {

QString FormatPeerId(PeerId peerId) {
	const auto bare = (peerId.value & PeerId::kChatTypeMask);
	if (peerId.is<UserId>()) {
		return QString::number(bare);
	} else if (peerId.is<ChannelId>()) {
		return "-100" + QString::number(bare);
	} else if (peerId.is<ChatId>()) {
		return '-' + QString::number(bare);
	}
	return QString::number(bare);
}

} // namespace Margy::Profile
