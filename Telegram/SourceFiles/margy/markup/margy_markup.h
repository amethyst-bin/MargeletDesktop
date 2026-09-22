#pragma once

#include "ui/text/text_entity.h"

namespace Margy::Markup {

[[nodiscard]] bool Has(const QString &text);
void Process(TextWithEntities &textWithEntities);

} // namespace Margy::Markup
