#pragma once

#include "ui/layers/box_content.h"
#include <QString>
#include <cstdint>

namespace Ui {
class InputField;
class VerticalLayout;
} // namespace Ui

namespace Margy::Wall {

class WallBox final : public ::Ui::BoxContent {
public:
	explicit WallBox(QWidget *parent, int64_t peerId = 0);
	~WallBox() override = default;

	static void Show(QWidget *parent = nullptr, int64_t peerId = 0);

protected:
	void prepare() override;

private:
	void loadPosts();
	void addPost(const QString &text);

	int64_t _peerId = 0;
	::Ui::VerticalLayout *_postsContainer = nullptr;
	::Ui::InputField *_input = nullptr;
};

} // namespace Margy::Wall
