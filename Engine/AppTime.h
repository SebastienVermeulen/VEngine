#pragma once
#include "pch.h"
#include "Singleton.h"

class AppTime final : public Singleton<AppTime>
{
public:
	AppTime()
		:Singleton<AppTime>()
		, m_DeltaTime{ 0.0f }
		, m_LastTime{}
		, m_AccumulatedTime{ 0.0f }
		, m_FixedTreshHold{ 1.0f / 30.0f }
	{
		m_LastTime = std::chrono::steady_clock::now();
	}
	virtual ~AppTime() {}

	inline float Update()
	{
		const std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
		m_DeltaTime = (float)(std::chrono::duration_cast<std::chrono::microseconds>(current - m_LastTime).count()) / 1000000.0f;
		m_LastTime = current;
		return m_DeltaTime;
	}
	inline bool FixedUpdate()
	{
		m_AccumulatedTime += m_DeltaTime;
		if (m_AccumulatedTime >= m_FixedTreshHold)
		{
			m_AccumulatedTime -= m_FixedTreshHold;
			return true;
		}
		return false;
	}

	inline float GetDeltaTime() { return m_DeltaTime / 1000.0f; }

private:
	//Values keeping track of delta
	float m_DeltaTime;
	std::chrono::steady_clock::time_point m_LastTime;

	//Values keeping track of the fixed time
	float m_AccumulatedTime;
	float m_FixedTreshHold;
};
