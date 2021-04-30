#pragma once

template<class T>
class Singleton
{
public:
	inline virtual ~Singleton() {}

	static inline T& Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}
		return *m_pInstance;
	}
	static inline void ReleaseInstance()
	{
		SafeDelete(m_pInstance);
	}

protected:
	inline Singleton() {}

private:
	static T* m_pInstance;
};

template<class T>
T* Singleton<T>::m_pInstance = nullptr;
