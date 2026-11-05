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
	: m_CurrentRotation{ 0.0f, 0.0f, 0.0f }//-90.0f, 90.0f, 135.0f }
	, m_pDirectionalLight{}
	, m_pPointLight{}
	, m_Materials{}
{
}
MainScene::~MainScene()
{
	for (Material* pMaterial : m_Materials)
	{
		SafeDelete(pMaterial);
	}
}

bool MainScene::Init()
{
	//Settings
	EngineSettings::Instance()->SetRendertype(RenderType::deferred);
	
	std::string objectName = "Camera";
	Object* pObject = CreateObject(objectName);
	Camera* pCamera = new Camera(CameraSettings::Default());
	pCamera->AddRenderType(RenderType::deferred);
	pObject->AddComponent(pCamera);
	DirectX::XMFLOAT3 newCamPos = { 0.0f, 0.0f, -1.0f };
	pObject->GetTransform()->Translate(newCamPos);
	 
	//Objects
	objectName = "Bunny";
	pObject = CreateObject(objectName);
	MeshComponent* pMeshCompBunny = dynamic_cast<MeshComponent*>(pObject->AddComponent(new MeshComponent(L"..\\Resources\\Meshes\\StanfordBunny.obj")));
	pObject->SetRenderType(pMeshCompBunny, RenderType::deferred);
	pObject->GetTransform()->Rotate(m_CurrentRotation);
	pObject->GetTransform()->Translate(0.0f, 0.0f, 0.0f);

	objectName = "Box1";
	pObject = CreateObject(objectName);
	MeshComponent* pMeshCompBox1 = dynamic_cast<MeshComponent*>(pObject->AddComponent(new MeshComponent(L"..\\Resources\\Meshes\\box.obj")));
	pObject->SetRenderType(pMeshCompBox1, RenderType::deferred);
	pObject->GetTransform()->Translate(0.0f, -0.1f, 0.0f);
	pObject->GetTransform()->Scale(3.0f, 0.1f, 3.0f);

	{
		// Create the mesh material
		Material* pMaterial = new Material(L"..\\Resources\\Shaders\\ShaderDeferred.fx");
		m_Materials.push_back(pMaterial);
		pMaterial->SetRendertype(RenderType::deferred);
		// Setup the textures for our material
		std::vector<MaterialTextureParam> TextureParamsDeferredMap{
			MaterialTextureParam{ false, "gAlbedoMap", L"..\\Resources\\Textures\\LionAlbedo.dds" },
			MaterialTextureParam{ false, "gMetalnessMap", L"..\\Resources\\Textures\\LionMetalness.dds" },
			MaterialTextureParam{ false, "gRoughnessMap", L"..\\Resources\\Textures\\LionRoughness.dds" } };
		pMaterial->GetTextureParams().AddMap(TextureParamsDeferredMap);

		// Set our material
		pMeshCompBunny->SetMaterial(pMaterial);
		pMeshCompBunny->SetShadowCasting(true);
		pMeshCompBox1->SetMaterial(pMaterial);
		pMeshCompBox1->SetShadowCasting(true);
	}

	//Lights
	objectName = "DirectionalLight";
	pObject = CreateObject(objectName);
	m_pDirectionalLight = dynamic_cast<DirectionalLight*>(pObject->AddComponent(new DirectionalLight()));
	pObject->AddRenderType(m_pDirectionalLight, RenderType::deferred);
	m_pDirectionalLight->SetIntensity(5.0f); 
	pObject->GetTransform()->Rotate(70, 155, 0);
	m_pDirectionalLight->SetIsShadowCasting(true);
	objectName = "PointLight";
	pObject = CreateObject(objectName);
	pObject->GetTransform()->Translate(-7.0f,0.0f,-7.0f);
	m_pPointLight = dynamic_cast<PointLight*>(pObject->AddComponent(new PointLight()));
	pObject->AddRenderType(m_pPointLight, RenderType::deferred);
	m_pPointLight->SetIntensity(100.0f);
	m_pPointLight->SetColor(DirectX::XMFLOAT3(0.1f,0.6f,0.95f));

	return true;
}
void MainScene::Update(const float elapsedSec)
{
}
