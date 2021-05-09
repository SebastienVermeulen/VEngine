#include "pch.h"
#include "MainScene.h"
#include "Object.h"
#include "MeshComponent.h"
#include "Material.h"

MainScene::MainScene() 
{
	Object* pObject = CreateObject();
	MeshComponent* pMeshComp = dynamic_cast<MeshComponent*>(pObject->AddComponent(new MeshComponent()));
	pMeshComp->SetMaterial(new Material());
}
MainScene::~MainScene()
{
}
