#pragma once
#include "pch.h"

class AppTime final 
{
public:
	inline static float Update()
	{
		time_t val;
		time(&val);

		m_DeltaTime = val - m_LastTime;
		m_LastTime = (float)val;
		return m_DeltaTime;
	}
	inline static bool FixedUpdate()
	{
		m_AccumulatedTime += m_DeltaTime;
		if (m_AccumulatedTime >= m_FixedTreshHold)
		{
			m_AccumulatedTime -= m_FixedTreshHold;
			return true;
		}
		return false;
	}

	inline static float GetDeltaTime() { return m_DeltaTime / 1000.0f; }

private:
	//Values keeping track of delta
	static float m_DeltaTime;
	static float m_LastTime;

	//Values keeping track of the fixed time
	static float m_AccumulatedTime;
	static float m_FixedTreshHold;
};

float AppTime::m_DeltaTime = 0.0f;
float AppTime::m_LastTime = 0.0f;
float AppTime::m_AccumulatedTime = 0.0f;
float AppTime::m_FixedTreshHold = 1.0f / 30.0f;
