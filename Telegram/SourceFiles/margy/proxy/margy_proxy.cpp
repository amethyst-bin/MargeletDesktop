#include "margy/proxy/margy_proxy.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "core/core_settings_proxy.h"
#include "mtproto/mtproto_proxy_data.h"

namespace Margy::Proxy {
namespace {

const auto kCommunityHost = u"proxy.margelet.org"_q;
constexpr uint32_t kCommunityPort = 443;
const auto kCommunitySecret = u"ee000000000000000000000000000000007777772e676f6f676c652e636f6d"_q;

[[nodiscard]] MTP::ProxyData CommunityProxyData() {
	MTP::ProxyData p;
	p.type = MTP::ProxyData::Type::Mtproto;
	p.host = kCommunityHost;
	p.port = kCommunityPort;
	p.password = kCommunitySecret;
	return p;
}

} // namespace

bool IsCommunityProxyActive() {
	const auto &proxySettings = Core::App().settings().proxy();
	if (!proxySettings.isEnabled()) {
		return false;
	}
	const auto selected = proxySettings.selected();
	return selected.host == kCommunityHost && selected.port == kCommunityPort;
}

void ConnectCommunityProxy() {
	const auto data = CommunityProxyData();
	Core::App().settings().proxy().addToList(data);
	Core::App().setCurrentProxy(data, MTP::ProxyData::Settings::Enabled);
}

void DisconnectCommunityProxy() {
	Core::App().setCurrentProxy(MTP::ProxyData(), MTP::ProxyData::Settings::Disabled);
}

} // namespace Margy::Proxy
