#pragma once
#include "BaseUpdateStructure.h"
#include "Scene.h"

class Project final : public BaseUpdateStructure
{
public:
	Project();
	virtual ~Project();

	Project(Project& other) = delete;
	Project(Project&& other) = delete;
	Project operator=(Project& other) = delete;
	Project& operator=(Project&& other) = delete;

	virtual bool Init() override;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;
	void Render() const;

	template<class T>
	inline T* CreateScene(bool mainScene = false) 
	{
		T* pNewClass = new T();
		//Safety to check if the class inherits from scene
		Scene* pNewScene = dynamic_cast<Scene*>(pNewClass);
		if (pNewScene)
		{
			m_Scenes.push_back(pNewScene);
			if (mainScene)
			{
				m_pCurrentScene = pNewScene;
			}
			pNewScene->Init();
			pNewScene->MarkAsInit();
		}
		return (T*)pNewScene;
	}
	template<class T>
	inline T* GetScene() const 
	{
		return m_pCurrentScene; 
	}

private:
	std::vector<Scene*> m_Scenes;
	Scene* m_pCurrentScene;
};