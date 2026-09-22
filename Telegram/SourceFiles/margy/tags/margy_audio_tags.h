#pragma once

#include <QString>
#include <QByteArray>

namespace Margy::Tags {

struct TagInfo {
	QString title;
	QString artist;
	QByteArray cover;
};

[[nodiscard]] bool WriteTags(
	const QString &srcPath,
	const QString &dstPath,
	const TagInfo &tags);

[[nodiscard]] bool WriteTags(
	const QString &filePath,
	const TagInfo &tags);

} // namespace Margy::Tags
