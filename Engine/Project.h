#pragma once
#include "BaseUpdateStructure.h"
#include "Scene.h"

class Project final : public BaseUpdateStructure
{
public:
	Project();
	~Project();

	Project(Project& other) = delete;
	Project(Project&& other) = delete;
	Project operator=(Project& other) = delete;
	Project& operator=(Project&& other) = delete;

	virtual void Init() override;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;
	virtual void Render() const override;

	template<class T>
	inline T* CreateScene(bool mainScene = false) 
	{
		m_Scenes.push_back(new T());
		if (mainScene)
		{
			m_pCurrentScene = m_Scenes[m_Scenes.size() - 1];
		}
		return dynamic_cast<T*>(*(m_Scenes.end() - 1));
	}

private:
	std::vector<Scene*> m_Scenes;
	Scene* m_pCurrentScene;
};