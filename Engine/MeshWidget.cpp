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

    bool shouldRender = m_pMeshComponent->ShouldRender();
    ImGui::Checkbox("Should Render", &shouldRender);
    m_pMeshComponent->ShouldRender(shouldRender);
    //Should Renderable Render
    if (shouldRender)
    {
        pEngineManager->GetActiveRenderer()->AddRenderable(m_pMeshComponent);
    }
    else 
    {
        pEngineManager->GetActiveRenderer()->RemoveRenderable(m_pMeshComponent);
    }

    ImGui::TreePop();
}