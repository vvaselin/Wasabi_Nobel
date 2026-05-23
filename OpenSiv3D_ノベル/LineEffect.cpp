#include "LineEffect.hpp"

void LineEffect::init()
{
	linework
		.setThickness(minThickness, maxThickness)
		.setLineCount(static_cast<size_t>(lineCount))
		.setOffsetRange(offsetRange);
}

void LineEffect::draw()
{
	linework.setTargetShape(target);
	linework.draw(ColorF{ 1.0 });

	offsetRange = Periodic::Sine0_1(2s) * 10 + 100;
	linework.setOffsetRange(offsetRange);
}
