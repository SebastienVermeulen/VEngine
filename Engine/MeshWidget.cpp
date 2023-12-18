#include "pch.h"
#include "MeshWidget.h"
#include "MeshComponent.h"
#include "MaterialWidget.h"
#include "EngineManager.h"
#include "EngineSettings.h"
#include "Renderer.h"
#include "RenderUtils.h"

MeshWidget::MeshWidget(MeshComponent* pMeshComponent)
    :Widget()
    , m_pMeshComponent{ pMeshComponent }
{
}
MeshWidget::~MeshWidget()
{
}

bool MeshWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("Mesh:");
}
void MeshWidget::RenderUIElement(int idx)
{
    EngineManager* pEngineManager = EngineManager::Instance();
    if (!IsRenderTypeInVector(EngineSettings::Instance()->GetRenderType(), m_pMeshComponent->GetRenderTypes()))
    {
        ImGui::TreePop();
        return;
    }

    MaterialWidget* pMaterialWidget = m_pMeshComponent->GetMaterial()->GetWidget();
    if (pMaterialWidget)
    {
        if (pMaterialWidget->RenderUITitle(0)) 
        {
            pMaterialWidget->RenderUIElement(0);
        }
    }

    //Separate each element with a line
    ImGui::Separator();

    bool isVisible = m_pMeshComponent->IsVisible();
    ImGui::Checkbox("Visible", &isVisible);
    m_pMeshComponent->SetCanRender(isVisible);
    //Should Renderable Render
    m_pMeshComponent->SetVisible(isVisible);

    ImGui::TreePop();
}