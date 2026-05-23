#include "SasaGUI.hpp"

namespace SasaGUI
{
	constexpr Delay::Delay(Duration duration) noexcept
		: m_duration(duration.count())
		, m_time(m_duration)
	{
		assert(duration >= 0s);
	}

	bool Delay::update(bool in, double d)
	{
		if (in)
		{
			m_time = 0.0;
		}
		else
		{
			m_time += d;
		}
		return in || m_time <= m_duration;
	}

	Repeat::Repeat(Duration interval, Duration delay)
		: m_interval(interval.count())
		, m_delay(delay.count())
	{
		assert(interval > 0s);
		assert(delay >= 0s);
	}

	bool Repeat::update(bool in, double d)
	{
		if (not in)
		{
			m_first = true;
			m_delayed = false;
			return false;
		}

		bool out = false;

		if (m_first)
		{
			out = true;
			m_accumulation = 0.0;
			m_first = false;
		}

		m_accumulation += d;

		if (not m_delayed)
		{
			if (m_accumulation < m_delay)
			{
				return out;
			}

			out = true;
			m_accumulation -= m_delay;
			m_delayed = true;
		}

		double count = std::floor(m_accumulation / m_interval);
		out |= count > 0.0;
		m_accumulation -= count * m_interval;

		return out;
	}

	ScrollBar::ScrollBar(Orientation orientation)
		: orientation(orientation)
	{
		updateLayout({ 0, 0, 0, 0 });
	}

	Rect ScrollBar::rect() const { return m_rect; }

	double ScrollBar::minimum() const { return m_minimum; }

	double ScrollBar::maximum() const { return m_maximum; }

	double ScrollBar::value() const { return m_value; }

	double ScrollBar::viewportSize() const { return m_viewportSize; }

	void ScrollBar::updateLayout(Rect rect)
	{
		m_rect = rect;
		switch (orientation)
		{
		case Orientation::Horizontal:
			m_rect.w = Max(m_rect.w, MinLength);
			m_rect.h = Thickness;
			m_trackLength = m_rect.w - Config::TrackMargin * 2;
			break;
		case Orientation::Vertical:
			m_rect.w = Thickness;
			m_rect.h = Max(m_rect.h, MinLength);
			m_trackLength = m_rect.h - Config::TrackMargin * 2;
			break;
		}
		updateThumbLength();
	}

	void ScrollBar::updateConstraints(double minimum, double maximum, double viewportSize)
	{
		m_minimum = minimum;
		m_maximum = maximum;
		m_viewportSize = viewportSize;

		m_value = Max(Min(m_value, m_maximum - m_viewportSize), m_minimum);

		updateThumbLength();
	}

	void ScrollBar::show()
	{
		m_colorTransitionDelay.update(true);
	}

	void ScrollBar::scroll(double delta, bool teleport)
	{
		if (teleport)
		{
			m_value = Max(Min(m_value + delta, m_maximum - m_viewportSize), m_minimum);
			m_scrollTarget = m_value;
			m_scrollVelocity = 0.0;
			return;
		}

		if (delta == 0.0)
		{
			return;
		}

		if (Math::Sign(delta) != Math::Sign(m_scrollVelocity))
		{
			m_scrollVelocity = 0.0;
		}
		m_scrollTarget = Max(Min(m_value + delta, m_maximum - m_viewportSize), m_minimum);
	}

	void ScrollBar::moveTo(double value)
	{
		m_value = Clamp(value, m_minimum, m_maximum - m_viewportSize);
		m_scrollTarget = m_value;
		m_scrollVelocity = 0.0;
	}

	void ScrollBar::update(Optional<Vec2> cursorXYPos, double deltaTime)
	{
		Optional<double> cursorPos = cursorXYPos.map([this](Vec2 v) { return getMainAxisValue(v); });
		double cursorDelta = prevCursorPos && cursorPos
			? *cursorPos - *prevCursorPos
			: 0.0;
		bool mouseOver = cursorPos && m_rect.contains(*cursorXYPos);
		auto thumbRect = getThumbRect();
		auto trackRect = getTrackRect();

		if (m_thumbLength == 0.0 || not MouseL.pressed())
		{
			m_thumbGrabbed = false;
			m_trackPressed = false;
		}
		else if (cursorXYPos.has_value() && MouseL.down())
		{
			if (thumbRect.contains(*cursorXYPos))
			{
				m_thumbGrabbed = true;
			}
			else if (getTrackRect().contains(*cursorXYPos))
			{
				m_trackPressed = true;
				m_largeScrollDirection = Math::Sign(*cursorPos - getMainAxisValue(thumbRect.pos));
				m_largeScrollTargetPos = *cursorPos;
			}
		}

		if (m_thumbGrabbed && cursorDelta != 0.0)
		{
			scroll(cursorDelta * (m_maximum - m_minimum - m_viewportSize) / (m_trackLength - m_thumbLength), true);
		}

		if (m_largeScrollTimer.update(m_trackPressed, deltaTime))
		{
			scroll(m_largeScrollDirection * m_viewportSize);

			double thumbPos = calculateThumbPos(m_scrollTarget);
			if (thumbPos <= m_largeScrollTargetPos &&
				m_largeScrollTargetPos <= thumbPos + m_thumbLength)
			{
				m_trackPressed = false;
			}
			if (m_scrollTarget <= m_minimum ||
				m_maximum - m_viewportSize <= m_scrollTarget)
			{
				m_trackPressed = false;
			}
		}

		m_value = Math::SmoothDamp(m_value, m_scrollTarget, m_scrollVelocity, Config::ScrollSmoothTime.count());

		bool show = (mouseOver || m_thumbGrabbed || m_trackPressed) && m_thumbLength != 0.0;
		m_colorTransition.update(m_colorTransitionDelay.update(show, deltaTime), deltaTime);

		prevCursorPos = cursorPos;
	}

	void ScrollBar::draw() const
	{
		double f = m_colorTransition.value();
		ColorF backColor = Config::BackgroundColor.lerp(Config::HoveredBackgroundColor, f);
		ColorF trackColor = Config::TrackColor.lerp(Config::HoveredTrackColor, f);
		ColorF thumbColor = Config::ThumbColor.lerp(Config::HoveredThumbColor, f);
		m_rect
			.draw(backColor);
		getTrackRect()
			.rounded(ThumbRoundness)
			.draw(trackColor);
		if (m_thumbLength != 0.0)
		{
			getThumbRect()
				.rounded(ThumbRoundness)
				.draw(thumbColor);
		}
	}

	void ScrollBar::updateThumbLength()
	{
		double range = m_maximum - m_minimum;
		if (range <= 0.0 ||
			m_viewportSize >= range)
		{
			m_thumbLength = 0;
		}
		else
		{
			m_thumbLength = Max<double>(m_trackLength * m_viewportSize / range, Config::ThumbMinLength);
		}
	}

	Rect ScrollBar::getTrackRect() const { return m_rect.stretched(-Config::TrackMargin); }

	inline double ScrollBar::calculateThumbPos(double value) const
	{
		return (m_trackLength - m_thumbLength) * value / (m_maximum - m_minimum - m_viewportSize);
	}

	RectF ScrollBar::getThumbRect() const
	{
		Point trackPos = getTrackRect().pos;
		double pos = calculateThumbPos(m_value);

		switch (orientation)
		{
		case Orientation::Horizontal:
			return {
				trackPos.x + pos,
				trackPos.y,
				m_thumbLength,
				Config::ThumbThickness
			};
		case Orientation::Vertical:
			return {
				trackPos.x,
				trackPos.y + pos,
				Config::ThumbThickness,
				m_thumbLength
			};
		}
	}

	double ScrollBar::getMainAxisValue(Vec2 input) const
	{
		switch (orientation)
		{
		case Orientation::Horizontal: return input.x;
		case Orientation::Vertical: return input.y;
		}
	}


}
