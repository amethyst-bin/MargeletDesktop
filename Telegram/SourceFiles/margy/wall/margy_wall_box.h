#pragma once

#include <QDialog>
#include <QString>
#include <cstdint>
#include <vector>

class QListWidget;
class QTextEdit;

namespace Margy::Wall {

struct Post {
	int64_t id = 0;
	QString author;
	QString text;
	QString timestamp;
};

class WallBox final : public QDialog {
	Q_OBJECT

public:
	explicit WallBox(QWidget *parent, int64_t peerId = 0);
	~WallBox() override = default;

	static void Show(QWidget *parent, int64_t peerId = 0);

private:
	void setupUi();
	void loadPosts();
	void addPost(const QString &text);

	int64_t _peerId = 0;
	QListWidget *_postsList = nullptr;
	QTextEdit *_input = nullptr;
};

} // namespace Margy::Wall
