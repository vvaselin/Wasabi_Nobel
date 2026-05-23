# include "FadeController.hpp"

FadeController::FadeController(double fadeInDur, double fadeOutDur, double crossFadeDur)
	: m_fadeInDur{ fadeInDur }
	, m_fadeOutDur{ fadeOutDur }
	, m_crossFadeDur{ crossFadeDur }
{
}

void FadeController::startFadeIn()
{
	m_elapsed = 0.0;
	m_state = FadeState::FadeIn;
}

void FadeController::startFadeOut()
{
	m_elapsed = 0.0;
	m_state = FadeState::FadeOut;
}

void FadeController::startCrossFade()
{
	m_elapsed = 0.0;
	m_state = FadeState::CrossFade;
}

bool FadeController::update()
{
	if (m_state == FadeState::Idle) return false;

	// シーン遷移直後など DeltaTime が極端に大きい場合に
	// アニメーションが1フレームで完了するのを防ぐ（10fps以上を想定）
	const double dt = Min(Scene::DeltaTime(), 0.1);
	m_elapsed += dt;

	const double dur = durationOf(m_state);
	const double t = Clamp(m_elapsed / dur, 0.0, 1.0);

	m_alpha = (m_state == FadeState::FadeOut) ? (1.0 - t) : t;

	if (t >= 1.0)
	{
		m_state = FadeState::Idle;
		return true;
	}
	return false;
}

double FadeController::alpha() const
{
	return m_alpha;
}

FadeState FadeController::state() const
{
	return m_state;
}

bool FadeController::isIdle() const
{
	return m_state == FadeState::Idle;
}

double FadeController::durationOf(FadeState s) const
{
	switch (s)
	{
	case FadeState::FadeIn:    return m_fadeInDur;
	case FadeState::FadeOut:   return m_fadeOutDur;
	case FadeState::CrossFade: return m_crossFadeDur;
	default:                   return 1.0;
	}
}
