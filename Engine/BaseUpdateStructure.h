#pragma once
class BaseUpdateStructure abstract
{
public:
	inline virtual void Init() {}
	inline virtual void Cleanup() {}

	inline virtual void Update(const float deltaTime) {}
	inline virtual void FixedUpdate() {}
	inline virtual void LateUpdate(const float deltaTime) {}
	inline virtual void Render() const {}
};
