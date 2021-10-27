#pragma once
class BaseUpdateStructure abstract
{
public:
	inline BaseUpdateStructure()
		:m_HasInit{ false }	
	{
	}

	inline virtual bool Init() { return true; }

	inline virtual void Update(const float deltaTime) {}
	inline virtual void FixedUpdate() {}
	inline virtual void LateUpdate(const float deltaTime) {}

	inline bool HasInit() const { return m_HasInit; }
	inline void MarkAsInit() { m_HasInit = true; }

private:

	bool m_HasInit;
};
