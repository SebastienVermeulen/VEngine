#include "pch.h"
#include "MaterialWidget.h"
#include "Material.h"

MaterialWidget::MaterialWidget(Material* pMaterial)
    :Widget()
    , m_pMaterial{ pMaterial }
{
}
MaterialWidget::~MaterialWidget()
{
}

bool MaterialWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("Material:");
}
void MaterialWidget::RenderUIElement(int idx)
{
    //Are there textures
    bool textureDetected = false;
    std::vector<MaterialTextureParam*> textureParams = m_pMaterial->GetTextureParams();
    for (MaterialTextureParam* pTextureParam : textureParams)
    {
        if (pTextureParam->m_Resource->IsValid())
        {
            textureDetected = true;
            break;
        }
    }
    if (textureDetected && ImGui::TreeNode("Textures:"))
    {
        ImGui::Unindent();
        for (MaterialTextureParam* pTextureParam : textureParams)
        {
            Texture* pTexture = pTextureParam->m_pTexture;
            if (pTexture && pTextureParam->m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::Text(pTextureParam->m_Name.c_str());
                ImGui::Image((void*)pTextureParam->m_pTexture->GetTextureView(), { 150.0f, 150.0f });
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Are there scalars
    bool scalarDetected = false;
    std::vector<MaterialScalarParam*> scalarParams = m_pMaterial->GetScalarParams();
    for (MaterialScalarParam* pScalarsParam : scalarParams)
    {
        if (pScalarsParam->m_Resource->IsValid()) 
        {
            scalarDetected = true;
            break;
        }
    }
    //Make the tree
    if (scalarDetected && ImGui::TreeNode("Scalars:"))
    {
        ImGui::Unindent();
        for (MaterialScalarParam* pScalarsParam : scalarParams)
        {
            if (pScalarsParam->m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::SliderFloat(pScalarsParam->m_Name.c_str(), &pScalarsParam->m_Value, pScalarsParam->m_Min, pScalarsParam->m_Max);
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Separate each element with a line
    ImGui::TreePop();
}