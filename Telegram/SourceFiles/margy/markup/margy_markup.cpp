#include "margy/markup/margy_markup.h"
#include "data/stickers/data_custom_emoji.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/text/text_utilities.h"

#include <QtCore/QRegularExpression>
#include <vector>
#include <algorithm>

namespace Margy::Markup {
namespace {

constexpr char16_t kOpen = 0x2060;
constexpr char16_t kClose = 0x2061;
constexpr char16_t kTrit = 0x2062;
constexpr int kTrits = 3;

constexpr int kKindTrits = 2;
constexpr int kValueTrits = 3;
constexpr int kMarkLen = 1 + kKindTrits + kValueTrits;

constexpr int kLenTrits = 5;
constexpr int kByteTrits = 6;

const auto kHeader = u"<! Message looks better with @margeletter! >"_q;

inline bool IsTrit(QChar c) {
	return c.unicode() >= kTrit && c.unicode() < (kTrit + kTrits);
}

inline bool AllTrits(const QString &text, int from, int count) {
	if (from < 0 || from + count > text.size()) {
		return false;
	}
	for (int i = 0; i < count; ++i) {
		if (!IsTrit(text[from + i])) {
			return false;
		}
	}
	return true;
}

inline int DecodeNumber(const QString &text, int at, int count) {
	int value = 0;
	int mul = 1;
	for (int i = 0; i < count; ++i) {
		value += (text[at + i].unicode() - kTrit) * mul;
		mul *= kTrits;
	}
	return value;
}

inline QString EncodeNumber(int value, int count) {
	QString result;
	result.reserve(count);
	for (int i = 0; i < count; ++i) {
		const auto trit = value % kTrits;
		value /= kTrits;
		result.push_back(QChar(kTrit + trit));
	}
	return result;
}

inline bool HasPayload(int kind) {
	return kind == kKindButton || kind == kKindEmoji;
}

inline int ClampIndex(int val, int maxVal) {
	return (val < 0) ? 0 : (val > maxVal) ? maxVal : val;
}

struct Run {
	int kind = 0;
	int value = 0;
	int start = 0;
	int end = 0;
	int openTagStart = 0;
	int openTagEnd = 0;
	int closeTagPos = 0;
	QByteArray payload;

	[[nodiscard]] QString text() const {
		return QString::fromUtf8(payload);
	}
};

std::vector<Run> Parse(const QString &text) {
	std::vector<Run> runs;
	if (text.size() < 4) {
		return runs;
	}

	struct OpenTag {
		int kind = 0;
		int value = 0;
		int openTagStart = 0;
		int start = 0;
		QByteArray payload;
	};

	std::vector<OpenTag> stack;

	for (int i = 0; i < text.size(); ++i) {
		const auto c = text[i];
		if (c.unicode() == kOpen && i + kMarkLen <= text.size() && AllTrits(text, i + 1, kKindTrits + kValueTrits)) {
			const auto kind = DecodeNumber(text, i + 1, kKindTrits);
			const auto value = DecodeNumber(text, i + 1 + kKindTrits, kValueTrits);
			int after = i + kMarkLen;
			QByteArray payload;
			if (HasPayload(kind)) {
				if (after + kLenTrits > text.size() || !AllTrits(text, after, kLenTrits)) {
					continue;
				}
				const auto len = DecodeNumber(text, after, kLenTrits);
				after += kLenTrits;
				if (after + len * kByteTrits > text.size() || !AllTrits(text, after, len * kByteTrits)) {
					continue;
				}
				payload.resize(len);
				for (int b = 0; b < len; ++b) {
					payload[b] = static_cast<char>(DecodeNumber(text, after + b * kByteTrits, kByteTrits));
				}
				after += len * kByteTrits;
			}
			stack.push_back(OpenTag{
				.kind = kind,
				.value = value,
				.openTagStart = i,
				.start = after,
				.payload = std::move(payload),
			});
			i = after - 1;
		} else if (c.unicode() == kClose) {
			if (!stack.empty()) {
				auto top = std::move(stack.back());
				stack.pop_back();
				runs.push_back(Run{
					.kind = top.kind,
					.value = top.value,
					.start = top.start,
					.end = i,
					.openTagStart = top.openTagStart,
					.openTagEnd = top.start,
					.closeTagPos = i,
					.payload = std::move(top.payload),
				});
			}
		}
	}
	return runs;
}

} // namespace

bool Has(const QString &text) {
	static const auto headerRegex = QRegularExpression(
		u"<!\\s*Message looks better with"_q,
		QRegularExpression::CaseInsensitiveOption);
	return text.contains(QChar(kOpen)) || text.contains(headerRegex);
}

QString Open(int kind, int value, const QByteArray &payload) {
	QString result;
	result.reserve(1 + kKindTrits + kValueTrits + (payload.isEmpty() ? 0 : (kLenTrits + payload.size() * kByteTrits)));
	result.push_back(QChar(kOpen));
	result += EncodeNumber(kind, kKindTrits);
	result += EncodeNumber(value, kValueTrits);
	if (HasPayload(kind)) {
		result += EncodeNumber(payload.size(), kLenTrits);
		for (int b = 0; b < payload.size(); ++b) {
			result += EncodeNumber(static_cast<uchar>(payload[b]), kByteTrits);
		}
	}
	return result;
}

QString Close() {
	return QString(QChar(kClose));
}

void Process(TextWithEntities &textWithEntities) {
	if (!Has(textWithEntities.text)) {
		return;
	}

	static const auto headerRegex = QRegularExpression(
		u"\\n?<!\\s*Message looks better with.*?\\!\\s*>\\n?"_q,
		QRegularExpression::CaseInsensitiveOption);
	const auto match = headerRegex.match(textWithEntities.text);
	if (match.hasMatch()) {
		const auto removeStart = match.capturedStart();
		const auto removeEnd = match.capturedEnd();
		const auto removedLen = match.capturedLength();
		textWithEntities.text.remove(removeStart, removedLen);

		for (auto it = textWithEntities.entities.begin(); it != textWithEntities.entities.end();) {
			if (it->offset() >= removeEnd) {
				*it = EntityInText(it->type(), it->offset() - removedLen, it->length(), it->data());
				++it;
			} else if (it->offset() + it->length() <= removeStart) {
				++it;
			} else {
				it = textWithEntities.entities.erase(it);
			}
		}
	}

	const auto runs = Parse(textWithEntities.text);
	if (runs.empty() && !textWithEntities.text.contains(QChar(kOpen))) {
		return;
	}

	const auto origSize = textWithEntities.text.size();
	std::vector<bool> removed(origSize, false);

	for (const auto &run : runs) {
		for (int i = run.openTagStart; i < run.openTagEnd && i < origSize; ++i) {
			removed[i] = true;
		}
		if (run.closeTagPos >= 0 && run.closeTagPos < origSize) {
			removed[run.closeTagPos] = true;
		}
	}

	for (int i = 0; i < origSize; ++i) {
		const auto u = textWithEntities.text[i].unicode();
		if (u == kOpen || u == kClose || (u >= kTrit && u < kTrit + kTrits)) {
			removed[i] = true;
		}
	}

	QString cleanText;
	cleanText.reserve(origSize);
	std::vector<int> oldToNew(origSize + 1, 0);

	for (int i = 0; i < origSize; ++i) {
		oldToNew[i] = cleanText.size();
		if (!removed[i]) {
			cleanText.push_back(textWithEntities.text[i]);
		}
	}
	oldToNew[origSize] = cleanText.size();

	const auto maxIndex = int(origSize);
	for (auto it = textWithEntities.entities.begin(); it != textWithEntities.entities.end();) {
		const auto start = oldToNew[ClampIndex(it->offset(), maxIndex)];
		const auto end = oldToNew[ClampIndex(it->offset() + it->length(), maxIndex)];
		if (end > start) {
			*it = EntityInText(it->type(), start, end - start, it->data());
			++it;
		} else {
			it = textWithEntities.entities.erase(it);
		}
	}

	for (const auto &run : runs) {
		const auto start = oldToNew[ClampIndex(run.start, maxIndex)];
		const auto end = oldToNew[ClampIndex(run.end, maxIndex)];
		const auto length = end - start;
		if (length <= 0) {
			continue;
		}
		if (run.kind == kKindButton) {
			const auto url = run.text();
			if (!url.isEmpty()) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomUrl, start, length, url));
			}
		} else if (run.kind == kKindEmoji) {
			bool ok = false;
			const auto docId = run.text().trimmed().toULongLong(&ok);
			if (ok && docId != 0) {
				textWithEntities.entities.push_back(
					EntityInText(EntityType::CustomEmoji, start, length, Data::SerializeCustomEmojiId(docId)));
			}
		} else if (run.kind == kKindOutline) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Bold, start, length));
		} else if (run.kind == kKindDim) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Italic, start, length));
		} else if (run.kind == kKindRainbow) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Underline, start, length));
		} else if (run.kind == kKindSize) {
			textWithEntities.entities.push_back(
				EntityInText(EntityType::Bold, start, length));
		}
	}

	ranges::sort(textWithEntities.entities, [](const EntityInText &a, const EntityInText &b) {
		if (a.offset() != b.offset()) {
			return a.offset() < b.offset();
		}
		if (a.length() != b.length()) {
			return a.length() > b.length();
		}
		return a.type() < b.type();
	});

	textWithEntities.entities.erase(
		std::unique(
			textWithEntities.entities.begin(),
			textWithEntities.entities.end(),
			[](const EntityInText &a, const EntityInText &b) {
				return a.offset() == b.offset()
					&& a.length() == b.length()
					&& a.type() == b.type()
					&& a.data() == b.data();
			}),
		textWithEntities.entities.end());

	textWithEntities.text = std::move(cleanText);
}

void EncodeForSending(TextWithTags &textWithTags) {
	if (textWithTags.text.isEmpty() || textWithTags.tags.isEmpty()) {
		return;
	}

	struct Insertion {
		int pos = 0;
		int order = 0;
		int suborder = 0;
		QString text;
	};
	std::vector<Insertion> insertions;

	for (const auto &tag : textWithTags.tags) {
		const auto list = TextUtilities::SplitTags(tag.id);
		const auto start = std::max(0, tag.offset);
		const auto end = std::min(int(textWithTags.text.size()), tag.offset + tag.length);
		if (end <= start) {
			continue;
		}

		for (const auto &single : list) {
			auto kind = -1;
			if (single == Ui::InputField::kTagMargyDim) {
				kind = kKindDim;
			} else if (single == Ui::InputField::kTagMargyOutline) {
				kind = kKindOutline;
			} else if (single == Ui::InputField::kTagMargyRainbow) {
				kind = kKindRainbow;
			} else if (single == Ui::InputField::kTagMargySize) {
				kind = kKindSize;
			}
			if (kind >= 0) {
				insertions.push_back(Insertion{
					.pos = start,
					.order = 1,
					.suborder = -end,
					.text = Open(kind, 0),
				});
				insertions.push_back(Insertion{
					.pos = end,
					.order = 0,
					.suborder = -start,
					.text = Close(),
				});
			}
		}
	}

	if (insertions.empty()) {
		return;
	}

	ranges::sort(insertions, [](const Insertion &a, const Insertion &b) {
		if (a.pos != b.pos) {
			return a.pos < b.pos;
		}
		if (a.order != b.order) {
			return a.order < b.order;
		}
		return a.suborder < b.suborder;
	});

	const auto origSize = int(textWithTags.text.size());
	std::vector<int> startMap(origSize + 1, 0);
	std::vector<int> endMap(origSize + 1, 0);

	QString newText;
	newText.reserve(origSize + insertions.size() * 10 + 64);

	auto insIt = insertions.begin();
	for (int i = 0; i <= origSize; ++i) {
		endMap[i] = newText.size();
		while (insIt != insertions.end() && insIt->pos == i && insIt->order == 0) {
			newText += insIt->text;
			++insIt;
		}
		while (insIt != insertions.end() && insIt->pos == i && insIt->order == 1) {
			newText += insIt->text;
			++insIt;
		}
		startMap[i] = newText.size();
		if (i < origSize) {
			newText.push_back(textWithTags.text[i]);
		}
	}

	if (!newText.contains(kHeader)) {
		newText += u"\n"_q + kHeader;
	}

	for (auto &tag : textWithTags.tags) {
		const auto start = std::clamp(tag.offset, 0, origSize);
		const auto end = std::clamp(tag.offset + tag.length, 0, origSize);
		const auto newStart = startMap[start];
		const auto newEnd = endMap[end];
		tag.offset = newStart;
		tag.length = std::max(0, newEnd - newStart);
	}

	textWithTags.text = std::move(newText);
}

} // namespace Margy::Markup
