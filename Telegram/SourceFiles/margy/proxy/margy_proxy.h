#pragma once

#include <QString>

namespace Margy::Proxy {

[[nodiscard]] bool IsCommunityProxyActive();
void ConnectCommunityProxy();
void DisconnectCommunityProxy();

} // namespace Margy::Proxy
