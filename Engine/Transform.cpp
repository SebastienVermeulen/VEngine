#include "pch.h"
#include "Transform.h"
#include "Object.h"
#include "TransformWidget.h"

Transform::Transform() 
	:m_World{}
	, m_Rotation{ DirectX::XMFLOAT4{0.f,0.f,0.f,1.f} }
	, m_RotationEuler{ DirectX::XMFLOAT3{0.f,0.f,0.f} }
	, m_WorldRotation{ DirectX::XMFLOAT4{0.f,0.f,0.f,1.f} }
	, m_Position{ DirectX::XMFLOAT3{0.f,0.f,0.f} }
	, m_WorldPosition{ DirectX::XMFLOAT3{0.f,0.f,0.f} }
	, m_Scale{ DirectX::XMFLOAT3{1.f,1.f,1.f} }
	, m_WorldScale{ DirectX::XMFLOAT3{1.f,1.f,1.f} }
	, m_Forward{ DirectX::XMFLOAT3{0.f,0.f,1.f} }
	, m_Up{ DirectX::XMFLOAT3{0.f,1.f,0.f} }
	, m_Right{ DirectX::XMFLOAT3{1.f,0.f,0.f} }
	, m_pObject{nullptr}
	, m_pTransformWidget{ new TransformWidget(this) }
{
	UpdateTransforms();
}
Transform::~Transform() 
{
	SafeDelete(m_pTransformWidget);
}

void Transform::UpdateTransforms()
{
	//Calculate World Matrix
	DirectX::XMVECTOR rot = DirectX::XMLoadFloat4(&m_Rotation);
	DirectX::XMMATRIX world =
		DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
		DirectX::XMMatrixRotationQuaternion(rot) *
		DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::XMStoreFloat4x4(&m_World, world);

	//Get World Transform
	DirectX::XMVECTOR pos, scale;
	if (DirectX::XMMatrixDecompose(&scale, &rot, &pos, world))
	{
		DirectX::XMStoreFloat3(&m_WorldPosition, pos);
		DirectX::XMStoreFloat3(&m_WorldScale, scale);
		DirectX::XMStoreFloat4(&m_WorldRotation, rot);
	}

	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(rot);
	const DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0, 0, 1, 0), rotMat);
	const DirectX::XMVECTOR right = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1, 0, 0, 0), rotMat);
	const DirectX::XMVECTOR up = DirectX::XMVector3Cross(forward, right);

	DirectX::XMStoreFloat3(&m_Forward, forward);
	DirectX::XMStoreFloat3(&m_Right, right);
	DirectX::XMStoreFloat3(&m_Up, up);
}

void Transform::Translate(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	UpdateTransforms();
}
void Transform::Translate(const DirectX::XMFLOAT3& position)
{
	Translate(position.x, position.y, position.z);
}
void Transform::Translate(const DirectX::XMVECTOR& position)
{
	XMStoreFloat3(&m_Position, position);
	UpdateTransforms();
}

void Transform::Rotate(float x, float y, float z)
{
	m_RotationEuler = { x, y, z };
	XMStoreFloat4(&m_Rotation,
		DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(x),
			DirectX::XMConvertToRadians(y),
			DirectX::XMConvertToRadians(z)));
	UpdateTransforms();
}
void Transform::Rotate(const DirectX::XMFLOAT3& rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}
void Transform::AddRotation(float x, float y, float z)
{
	DirectX::XMVECTOR addedQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(x),
		DirectX::XMConvertToRadians(y),
		DirectX::XMConvertToRadians(z));
	XMStoreFloat4(&m_Rotation, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&m_Rotation), addedQuaternion));
	UpdateTransforms();
}
void Transform::AddRotation(const DirectX::XMFLOAT3& rotation)
{
	AddRotation(rotation.x, rotation.y, rotation.z);
}

void Transform::Scale(float x, float y, float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
	UpdateTransforms();
}
void Transform::Scale(const DirectX::XMFLOAT3& scale)
{
	Scale(scale.x, scale.y, scale.z);
}
void Transform::Scale(const DirectX::XMVECTOR& scale)
{
	XMStoreFloat3(&m_Scale, scale);
	UpdateTransforms();
}

