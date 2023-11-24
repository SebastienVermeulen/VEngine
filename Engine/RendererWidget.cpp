#include "pch.h"
#include "RendererWidget.h"
#include "Renderer.h"
#include "MaterialWidget.h"
#include "Material.h"
#include "MaterialWidget.h"
#include "EngineManager.h"
#include "EngineSettings.h"

RendererWidget::RendererWidget()
    :Widget()
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
    EngineManager* pEngineManager = EngineManager::Instance();
    EngineSettings* pEngineSettings = EngineSettings::Instance();

    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    RenderType renderType = pEngineSettings->GetRenderType();
    // TO-DO: Don't hardcode rendertype choices
    ImGui::Combo("RenderType", (int*)&renderType, "Forwards\0Deferred\0\0");
    pEngineSettings->SetRendertype(renderType);

    //Separate each element with a line
    ImGui::Separator();

    bool vsync = pEngineSettings->GetIfVSync();
    std::string label{};
    ImGui::Checkbox("Vsync", &vsync);
    pEngineSettings->SetVSync(vsync);

    //Separate each element with a line
    ImGui::Separator();

    // Only show this for Deferred rendering
    if (renderType == RenderType::deferred)
    {
        ImGui::Text("Deferred Lighting: ");
        ImGui::SameLine();
        Widget* pMaterialWidget = ((DeferredDX11*)pEngineManager->GetRenderer(renderType))->GetDeferredLightingPassMaterial()->GetWidget();
        if (pMaterialWidget->RenderUITitle(0))
        {
            pMaterialWidget->RenderUIElement(0);
        }
    }

    ImGui::Indent();
    ImGui::TreePop();
}