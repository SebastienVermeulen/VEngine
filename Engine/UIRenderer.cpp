#include "pch.h"
#include "UIRenderer.h"
#include "EngineManager.h"
#include "Project.h"
#include "Scene.h"
#include "Object.h"
#include "ObjectWidget.h"
#include "RendererWidget.h"

 using namespace ImGui;

UIRenderer::UIRenderer()
	:Singleton<UIRenderer>()
	, m_SelectedObject{0}
	, m_SelectedObjectIdx{}
	, m_DetailsSize{}
	, m_DetailsPos{}
{ 
}
UIRenderer::~UIRenderer()
{
}

void UIRenderer::RenderUI()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();

	ImGuiWindowFlags window_flags = 0;
	 window_flags |= ImGuiWindowFlags_NoMove;
	 window_flags |= ImGuiWindowFlags_NoResize;

	//We specify a default position/size in case there's no data in the .ini file.
	//We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = GetMainViewport();
	m_DetailsSize = { 350, main_viewport->Size.y };
	m_DetailsPos = { main_viewport->Size.x - 350, 0 };
	SetNextWindowPos(m_DetailsPos, ImGuiCond_Always);
	SetNextWindowSize(m_DetailsSize, ImGuiCond_Always);

	//Main body of the Demo window starts here.
	if (!Begin("Details Panel:", NULL, window_flags))
	{
		//Early out if the window is collapsed, as an optimization.
		End();
		//Assembletogether draw data
		Render();
		//Render draw data
		ImGui_ImplDX11_RenderDrawData(GetDrawData());
		return;
	}

	if (CollapsingHeader("Metrics:"))
	{
		Text("Dear ImGui %s", ImGui::GetVersion());
		Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
	}

	if (CollapsingHeader("Engine Settings:"))
	{
		RendererWidget* pRendererWidget = EngineManager::Instance()->GetActiveRenderer()->GetWidget();
		if (pRendererWidget->RenderUITitle(0))
		{
			pRendererWidget->RenderUIElement(0);
		}
	}

	RenderSceneTree();
	RenderObjectDetails();

	if (CollapsingHeader("Info:"))
	{
		TextWrapped("METRICS DISPLAY some basic info on the framework.");
		Separator();
		TextWrapped("ENGINE SETTINGS provides option on how to render. And also the settings to adjust the deferred material lighting step (which can be found inside the forwards rendered material's detail as well).");
		Separator();
		TextWrapped("SCENE TREE display a rudementory tree of the objects inside the scene.");
		Separator();
		TextWrapped("OBJECT DETAILS allows you to adjust the parameters of the objects.");
		Text("\n");
		TextWrapped("Framework made by Sebastien Vermeulen. 27/10/2021");
		TextWrapped("Using: DirectX11, Effects11, FBX SDK, DirectXTK and Visual Leak Detector as thirdparty software.");
	}

	End();
	//Assembletogether draw data
	Render();
	//Render draw data
	ImGui_ImplDX11_RenderDrawData(GetDrawData());
}

void UIRenderer::RenderSceneTree()
{
	if (!ImGui::CollapsingHeader("Scene Tree:")) 
	{
		if (m_SelectedObject == nullptr)
		{
			std::vector<Object*> objects = EngineManager::Instance()->GetProject()->GetScene<Scene>()->GetAllObjects();
			m_SelectedObject = objects[0];
		}
		return;
	}

	if (ImGui::TreeNode("Scene:"))
	{
		//Go over all objects in the scene and add them to the tree
		std::vector<Object*> objects = EngineManager::Instance()->GetProject()->GetScene<Scene>()->GetAllObjects();
		for (int idx = 0; idx < objects.size(); ++idx)
		{
			//Separate each element with a line
			ImGui::Separator();

			ObjectWidget* pObjectwidget = dynamic_cast<ObjectWidget*>(objects[idx]->GetObjectWidget());
			//Check if the element was selected
			if (m_SelectedObjectIdx == idx) 
			{
				pObjectwidget->SetWasSelected(true);
			}
			//Add to ImGui and if selected save some data
			if (pObjectwidget->RenderUITitle(idx))
			{
				m_SelectedObjectIdx = idx;
				m_SelectedObject = objects[m_SelectedObjectIdx];
			}
			//To make sure the value is reset next frame
			if (m_SelectedObjectIdx == idx)
			{
				pObjectwidget->SetWasSelected(false);
			}
		}
		ImGui::TreePop();
	}
}
void UIRenderer::RenderObjectDetails()
{
	if (!ImGui::CollapsingHeader("Object Details:"))
	{
		return;
	}

	if (ImGui::TreeNode("Object Components:"))
	{
		ObjectWidget* pObjectWidget = m_SelectedObject->GetObjectWidget();
		if (pObjectWidget)
		{
			pObjectWidget->RenderUIElement(m_SelectedObjectIdx);
		}

		ImGui::TreePop();
	}
}