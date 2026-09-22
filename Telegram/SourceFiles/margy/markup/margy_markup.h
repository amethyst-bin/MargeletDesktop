#pragma once

#include "ui/text/text_entity.h"

namespace Margy::Markup {

constexpr int kKindSize = 0;
constexpr int kKindDim = 1;
constexpr int kKindRainbow = 2;
constexpr int kKindButton = 3;
constexpr int kKindEmoji = 4;
constexpr int kKindOutline = 5;

[[nodiscard]] bool Has(const QString &text);
void Process(TextWithEntities &textWithEntities);
void EncodeForSending(TextWithTags &textWithTags);
[[nodiscard]] QString Open(int kind, int value = 0, const QByteArray &payload = {});
[[nodiscard]] QString Close();

} // namespace Margy::Markup
