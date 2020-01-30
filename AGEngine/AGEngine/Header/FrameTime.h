#pragma once
#include <Singleton.h>
#include <Define.h>
class FrameTime :public Singleton<FrameTime> {
public:
	void FrameStart();
	void SetTimeScale(float timeScale);
	void SetMaxFrame(size_t frame);
	void SetFixedUpdateTime(float time);
	bool NeedFixedUpdate();

	//last frame time
	float DeltaTime();
	float FixedTime();
	void FrameEnd();
	float SystemDeltaTime();
private:
	void CheckFiexedTime();
	bool m_IfFixedUpdate;
	float m_DeltaTime;
	float m_FixedTime;
	TimePoint m_LastFrameStartTime;
	float m_FixedCounter;
	FrameTime();
	float m_TimeScaler;
	float m_MaxUpdateTime;
	friend class Singleton<FrameTime>;
};