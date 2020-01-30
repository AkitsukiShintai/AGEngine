#include <PCH.h>
#include<FrameTime.h>

void FrameTime::FrameStart() {
	m_LastFrameStartTime = Clock::now();
}

void  FrameTime::SetTimeScale(float timeScale) {
	m_TimeScaler = timeScale;
}

void FrameTime::SetMaxFrame(size_t frame) {
	if (frame == 0)
	{
		m_MaxUpdateTime = 0.00001f;
	}
	else
	{
		m_MaxUpdateTime = 1.0f / frame;
	}
}
void FrameTime::SetFixedUpdateTime(float time) {
	m_FixedTime = time;
}

bool FrameTime::NeedFixedUpdate() {
	CheckFiexedTime();
	return m_IfFixedUpdate;
}

void FrameTime::CheckFiexedTime() {

	if (m_FixedCounter <= 0) {
		m_IfFixedUpdate = true;
		m_FixedCounter = m_FixedTime + m_FixedCounter;
	}
	else
	{
		m_IfFixedUpdate = false;
	}
}

float FrameTime::DeltaTime() {
	return m_DeltaTime * m_TimeScaler;
}

float FrameTime::SystemDeltaTime() {
	return m_DeltaTime;
}



float FrameTime::FixedTime() {
	return m_FixedTime * m_TimeScaler;
}

void FrameTime::FrameEnd() {

	TimePoint now = Clock::now();
	auto deltaTime = now - m_LastFrameStartTime;
	m_DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count();
	while (m_DeltaTime < m_MaxUpdateTime)
	{
		TimePoint now = Clock::now();
		deltaTime = now - m_LastFrameStartTime;
		m_DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(deltaTime).count();
	}
	m_FixedCounter -= m_DeltaTime;
	//CheckFiexedTime(m_DeltaTime);
}

FrameTime::FrameTime() {
	m_IfFixedUpdate = false;
	m_DeltaTime = 0.0f;
	m_FixedTime = 1.0f / 60.0f;
	m_LastFrameStartTime = m_LastFrameStartTime = Clock::now();;
	m_TimeScaler = 1.0f;
	m_FixedCounter = m_FixedTime;
	SetMaxFrame(0);
}
