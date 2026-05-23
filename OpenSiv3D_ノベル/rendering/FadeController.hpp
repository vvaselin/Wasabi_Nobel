# pragma once
# include "core/Common.hpp"

// フェードアニメーションの状態
enum class FadeState { Idle, FadeIn, FadeOut, CrossFade };

// フェードイン・フェードアウト・クロスフェードを管理する共通クラス。
// Stopwatch を使わず Scene::DeltaTime() 積算方式にしているため、
// Array 等によるムーブ後も正常に動作する。
class FadeController
{
public:
	FadeController() = default;

	// fadeInDur    : フェードイン秒数
	// fadeOutDur   : フェードアウト秒数
	// crossFadeDur : クロスフェード秒数
	FadeController(double fadeInDur, double fadeOutDur, double crossFadeDur);

	// --- 状態遷移 ---
	void startFadeIn();
	void startFadeOut();
	void startCrossFade();

	// 毎フレーム呼ぶ。アニメーション完了時に true を返す
	bool update();

	// 現在のアルファ値
	//   FadeIn / CrossFade : 0 → 1
	//   FadeOut            : 1 → 0
	//   Idle               : 直前のアニメーションの終端値
	double alpha() const;

	FadeState state() const;
	bool isIdle() const;

private:
	double durationOf(FadeState s) const;

	FadeState m_state = FadeState::Idle;
	double    m_elapsed = 0.0; // 経過時間（秒）DeltaTime積算
	double    m_alpha = 0.0;
	double    m_fadeInDur = 1.0;
	double    m_fadeOutDur = 1.0;
	double    m_crossFadeDur = 1.0;
};
