#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define FLEXER_IMPLEMENTATION
#include "flexer.hpp"

using namespace flexer;

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		flex = std::make_unique<Flexer>();

		EID par1 = flex->createElement({
			.bounds = Rect(10, 10, ScreenWidth() - 20, ScreenHeight() - 20),
			.axis = Axis::Vertical
		});
		EID par2 = flex->createElement({
			.parent = par1,
			.bounds = Rect(0, 0, 0, 40),
			.proportion = 0,
			.axis = Axis::Horizontal
		});

		flex->createElement({
			.parent = par2,
			.bounds = Rect(0, 0, 60, 0),
			.proportion = 1
		});

		flex->createElement({
			.parent = par2,
			.bounds = Rect(0, 0, 60, 0),
			.proportion = 0
		});

		flex->createElement({
			.parent = par2,
			.bounds = Rect(0, 0, 60, 0),
			.proportion = 1
		});

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		flex->performLayout();

		Clear(olc::BLACK);

		for (auto&& [id, rect] : flex->rects()) {
			DrawRect({ rect.x, rect.y }, { rect.width, rect.height }, olc::GREEN);
		}

		return true;
	}

	std::unique_ptr<Flexer> flex;
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}
