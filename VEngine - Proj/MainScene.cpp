#include "pch.h"
#include "MainScene.h"
#include "Object.h"
#include "MeshComponent.h"
#include "Material.h"
#include "Camera.h"
#include "Transform.h"
#include "PointLight.h"
#include "DirectionalLight.h"

MainScene::MainScene() 
	: m_CurrentRotation{ -90.0f, 90.0f, 135.0f }
	, m_pDirectionalLight{}
	, m_pPointLight{}
{
}
MainScene::~MainScene()
{
}

bool MainScene::Init()
{
	//Settings
	Renderer* pRenderer = EngineManager::Instance()->GetRenderer();
	pRenderer->SetRenderType(RenderType::deferred);
	
	std::string objectName = "Camera";
	Object* pObject = CreateObject(objectName);
	pObject->AddComponent(new Camera(CameraSettings::Default()));
	DirectX::XMFLOAT3 newCamPos = { 0.0f, 0.0f, -30.0f };
	pObject->GetTransform()->Translate(newCamPos);
	 
	//Objects
	objectName = "DeferredObject";
	pObject = CreateObject(objectName);
	MeshComponent* pMeshComp = dynamic_cast<MeshComponent*>(pObject->AddComponent(new MeshComponent(L"..\\Resources\\Meshes\\ShaderBall.fbx")));
	pObject->GetTransform()->Rotate(m_CurrentRotation);
	pObject->GetTransform()->Translate(0.0f, -8.0f, 0.0f);
	pMeshComp->SetMaterial(new Material(L"..\\Resources\\Shaders\\ShaderDeferred.fx"));
	pMeshComp->GetMaterial()->SetIfDeferred(RenderType::deferred);
	
	objectName = "ForwardsObject";
	pObject = CreateObject(objectName);
	pMeshComp = dynamic_cast<MeshComponent*>(pObject->AddComponent(new MeshComponent(L"..\\Resources\\Meshes\\ShaderBall.fbx")));
	pObject->GetTransform()->Rotate(m_CurrentRotation);
	pObject->GetTransform()->Translate(0.0f, -8.0f, 0.0f);
	pMeshComp->SetMaterial(new Material(L"..\\Resources\\Shaders\\ShaderForwards.fx"));
	pMeshComp->GetMaterial()->SetIfDeferred(RenderType::forwards);
	
	//Lights
	objectName = "DirectionalLight";
	pObject = CreateObject(objectName);
	m_pDirectionalLight = dynamic_cast<DirectionalLight*>(pObject->AddComponent(new DirectionalLight()));
	m_pDirectionalLight->SetIntensity(5.0f); 
	pObject->GetTransform()->Rotate(-45,180,0);
	objectName = "PointLight";
	pObject = CreateObject(objectName);
	pObject->GetTransform()->Translate(-7.0f,0.0f,-7.0f);
	m_pPointLight = dynamic_cast<PointLight*>(pObject->AddComponent(new PointLight()));
	m_pPointLight->SetIntensity(100.0f);
	m_pPointLight->SetColor(DirectX::XMFLOAT3(0.1f,0.6f,0.95f));

	return true;
}
void MainScene::Update(const float elapsedSec)
{
}
