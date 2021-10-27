#pragma once
#include "BaseUpdateStructure.h"

class Object;
class Camera;

class Scene : public BaseUpdateStructure
{
public:
	Scene();
	virtual ~Scene();

	Scene(Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene operator=(Scene& other) = delete;
	Scene& operator=(Scene&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void FixedUpdate() override;
	virtual void LateUpdate(const float deltaTime) override;
	inline virtual void Render() const { /*Can be used for debug or other render related topics*/ };

	Object* CreateObject(std::string& name);
	inline std::vector<Object*>& GetAllObjects() 
	{
		return m_Objects;
	}

protected:
	Camera* GetCamera();

private:
	std::vector<Object*> m_Objects;
};