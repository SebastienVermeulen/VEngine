#pragma once

class Object;
class Widget;

class Transform final
{
public:
	Transform();
	~Transform();

	Transform(Transform& other) = delete;
	Transform(Transform&& other) = delete;
	Transform operator=(Transform& other) = delete;
	Transform& operator=(Transform&& other) = delete;

	const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
	const DirectX::XMFLOAT3& GetWorldPosition() const { return m_WorldPosition; }
	const DirectX::XMFLOAT3& GetScale() const { return m_Scale; }
	const DirectX::XMFLOAT3& GetWorldScale() const { return m_WorldScale; }
	const DirectX::XMFLOAT4& GetRotation() const { return m_Rotation; }
	const DirectX::XMFLOAT3& GetRotationEuler() const { return m_RotationEuler; }
	const DirectX::XMFLOAT4& GetWorldRotation() const { return m_WorldRotation; }
	const DirectX::XMFLOAT4X4& GetWorld() const { return m_World; }

	const DirectX::XMFLOAT3& GetForward() const { return m_Forward; }
	const DirectX::XMFLOAT3& GetUp() const { return m_Up; }
	const DirectX::XMFLOAT3& GetRight() const { return m_Right; }

	void Translate(float x, float y, float z);
	void Translate(const DirectX::XMFLOAT3& position);
	void Translate(const DirectX::XMVECTOR& position);

	void Rotate(float x, float y, float z);
	void Rotate(const DirectX::XMFLOAT3& rotation);
	void AddRotation(float x, float y, float z);
	void AddRotation(const DirectX::XMFLOAT3& rotation);

	void Scale(float x, float y, float z);
	void Scale(const DirectX::XMFLOAT3& scale);
	void Scale(const DirectX::XMVECTOR& scale);

	inline Object* GetEngineObject() { return m_pObject; }
	inline Widget* GetTransformWidget() { return m_pTransformWidget; }

private:
	void UpdateTransforms();

	DirectX::XMFLOAT4X4 m_World{};
	DirectX::XMFLOAT4 m_Rotation, m_WorldRotation;
	DirectX::XMFLOAT3 m_RotationEuler;
	DirectX::XMFLOAT3 m_Position, m_WorldPosition;
	DirectX::XMFLOAT3 m_Scale, m_WorldScale;
	DirectX::XMFLOAT3 m_Forward, m_Up, m_Right;

	Object* m_pObject;

	Widget* m_pTransformWidget;
};