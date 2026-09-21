#pragma once

#include "data/data_peer_id.h"

#include <QString>

namespace Margy::Profile {

[[nodiscard]] QString FormatPeerId(PeerId peerId);

} // namespace Margy::Profile
