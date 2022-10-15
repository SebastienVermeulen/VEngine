#include "pch.h"
#include "RendererWidget.h"
#include "Renderer.h"
#include "MaterialWidget.h"
#include "Material.h"
#include "MaterialWidget.h"

RendererWidget::RendererWidget(Renderer* pRenderer)
    :Widget()
    , m_pRenderer{ pRenderer }
{
}
RendererWidget::~RendererWidget()
{
}

bool RendererWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("Renderer:");
}
void RendererWidget::RenderUIElement(int idx)
{
    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    RenderType renderType = m_pRenderer->GetRenderType();
    ImGui::Combo("RenderType", (int*)&renderType, "Forwards\0Deferred\0\0");
    m_pRenderer->SetRendertype(renderType);

    //Separate each element with a line
    ImGui::Separator();

    bool vsync = m_pRenderer->GetFPSLimit();
    std::string label{};
    ImGui::Checkbox("Vsync", &vsync);
    m_pRenderer->SetFPSLimit(vsync);

    //Separate each element with a line
    ImGui::Separator();

    ImGui::Text("Deferred Lighting: ");
    ImGui::SameLine();
    Widget* pMaterialWidget = m_pRenderer->GetDeferredLightingPassMaterial()->GetWidget();
    if (pMaterialWidget->RenderUITitle(0))
    {
        pMaterialWidget->RenderUIElement(0);
    }

    ImGui::Indent();
    ImGui::TreePop();
}