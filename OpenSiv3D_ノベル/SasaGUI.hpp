#pragma once
# include "Common.hpp"

namespace SasaGUI
{
	namespace Config
	{
		struct ScrollBar
		{
			// |          ______________     |
			// |  =======|______________|==  |
			// |                             |
			//    ^---------Track---------^
			//           ^-----Thumb----^

			constexpr static ColorF BackgroundColor{ 1, 0 };
			constexpr static ColorF TrackColor{ 0, 0 };
			constexpr static ColorF ThumbColor{ 0.4, 0.7 };
			constexpr static ColorF HoveredBackgroundColor{ 1, 1 };
			constexpr static ColorF HoveredTrackColor{ 0, 0.1 };
			constexpr static ColorF HoveredThumbColor{ 0.4, 1 };

			constexpr static int32 TrackMargin = 3;
			constexpr static int32 ThumbThickness = 16;
			constexpr static int32 ThumbMinLength = 20;

			constexpr static Duration ScrollSmoothTime = 0.1s;
			constexpr static Duration LargeScrollDelay = 0.8s;
			constexpr static Duration LargeScrollInterval = 0.1s;
			constexpr static Duration FadeInDuration = 0.06s;
			constexpr static Duration FadeOutDuration = 0.5s;
			constexpr static Duration FadeOutDelay = 2s;
		};
	}

	class Delay
	{
	public:

		constexpr Delay(Duration duration) noexcept;

		bool update(bool in, double d = Scene::DeltaTime());

	private:

		double m_duration;

		double m_time;
	};

	struct Repeat
	{
	public:

		Repeat(Duration interval, Duration delay = 0s);

		bool update(bool in, double d = Scene::DeltaTime());

	private:

		double m_interval;

		double m_delay;

		bool m_first = true;

		bool m_delayed = false;

		double m_accumulation = 0.0;
	};

	enum class Orientation
	{
		Horizontal, Vertical
	};

	class ScrollBar
	{
	public:

		using Config = Config::ScrollBar;

		constexpr static int32 Thickness = Config::TrackMargin * 2 + Config::ThumbThickness;
		constexpr static int32 MinLength = Config::TrackMargin * 2 + Config::ThumbMinLength + 10;
		constexpr static double ThumbRoundness = Config::ThumbThickness * 0.5;

		ScrollBar(Orientation orientation);

	public:

		const Orientation orientation;

		Rect rect() const;

		double minimum() const;

		double maximum() const;

		double value() const;

		double viewportSize() const;

		void updateLayout(Rect rect);

		void updateConstraints(double minimum, double maximum, double viewportSize);

		void show();

		void scroll(double delta, bool teleport = false);

		void moveTo(double value);

		void update(Optional<Vec2> cursorXYPos = Cursor::PosF(), double deltaTime = Scene::DeltaTime());

		void draw() const;

	private:

		// 制限&スクロール位置

		double m_minimum = 0.0;

		double m_maximum = 1.0;

		double m_value = 0.0;

		double m_viewportSize = 1.0;

		// レイアウト

		Rect m_rect;

		double m_trackLength;

		double m_thumbLength = 0.0; // 表示しないとき0

		// 状態

		bool m_thumbGrabbed = false;

		bool m_trackPressed = false;

		// スクロール状態

		double m_scrollTarget = 0.0;

		double m_scrollVelocity = 0.0;

		int32 m_largeScrollDirection = 0;

		double m_largeScrollTargetPos = 0.0;

		// アニメーション関連

		Repeat m_largeScrollTimer{ Config::LargeScrollInterval, Config::LargeScrollDelay };

		Transition m_colorTransition{ Config::FadeInDuration, Config::FadeOutDuration };

		Delay m_colorTransitionDelay{ Config::FadeOutDelay };

		// その他

		Optional<double> prevCursorPos;

		void updateThumbLength();

		Rect getTrackRect() const;

		inline double calculateThumbPos(double value) const;

		RectF getThumbRect() const;

		double getMainAxisValue(Vec2 input) const;
	};
}



