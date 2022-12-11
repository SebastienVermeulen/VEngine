#include "pch.h"
#include "LightWidget.h"
#include "Light.h"
#include "EngineManager.h"
#include "Renderer.h"

LightWidget::LightWidget(Light* pLight)
    :Widget()
    , m_pLight{ pLight }
{
}
LightWidget::~LightWidget()
{
}

bool LightWidget::RenderUITitle(int idx)
{
    switch (m_pLight->GetLightType()) 
    {
    case LightType::directional:
        return ImGui::TreeNode("Directional Light:");
        break;
    case LightType::point:
        return ImGui::TreeNode("Point Light:");
        break;
    default:
        V_LOG(LogVerbosity::Warning, V_WTEXT("LightWidget: Undefined light type, on %s.", m_pLight->GetObject()->GetName()));
        return false;
        break;
    }
}
void LightWidget::RenderUIElement(int idx)
{
    //Separate each element with a line
    ImGui::Separator();

    ImGui::Unindent();
    DirectX::XMFLOAT3 color = m_pLight->GetColor();
    float lightColor[3] = { color.x, color.y , color.z };
    float oldColor[3] = { color.x, color.y , color.z };
    ImGui::ColorEdit3("LightColor", lightColor, ImGuiColorEditFlags_NoDragDrop);
    color = { lightColor[0], lightColor[1], lightColor[2] };
    m_pLight->SetColor(color);
    
    if (std::fabs(oldColor[0] - lightColor[0]) > DBL_EPSILON || std::fabs(oldColor[1] - lightColor[1]) > DBL_EPSILON || std::fabs(oldColor[2] - lightColor[2]) > DBL_EPSILON)
    {
         EngineManager::Instance()->GetRenderer()->SetShouldUpdateLighting(true);
    }

    //Separate each element with a line
    ImGui::Separator();

    float intensity = m_pLight->GetIntensity();
    float oldIntensity = intensity;
    switch (m_pLight->GetLightType())
    {
    case LightType::directional:
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f);                
        break;
    case LightType::point:
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 300.0f);
        break;
    default:
        V_LOG(LogVerbosity::Warning, V_WTEXT("LightWidget: Undefined light intensity, on %s.", m_pLight->GetObject()->GetName()));
        break;
    }
    ImGui::InputFloat("Exact", &intensity);       
    m_pLight->SetIntensity(intensity);

    if (std::fabs(intensity - oldIntensity) > DBL_EPSILON)
    {
        EngineManager::Instance()->GetRenderer()->SetShouldUpdateLighting(true);
    }

    //Separate each element with a line
    ImGui::Separator();

    bool shouldRender = m_pLight->ShouldRender();
    bool oldShouldRender = shouldRender;
    ImGui::Checkbox("Should Render", &shouldRender);
    m_pLight->ShouldRender(shouldRender);

    if (shouldRender != oldShouldRender)
    {
        EngineManager::Instance()->GetRenderer()->SetShouldUpdateLighting(true);
    }

    ImGui::Indent();
    ImGui::TreePop();
}
