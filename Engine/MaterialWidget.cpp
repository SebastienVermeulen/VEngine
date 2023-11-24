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
    MaterialTextureParamsMapping& textureParams = m_pMaterial->GetTextureParams();
    for (MaterialTextureParam& textureParam : textureParams.m_MaterialTextureParams)
    {
        if (textureParam.m_Resource->IsValid())
        {
            textureDetected = true;
            break;
        }
    }
    if (textureDetected && ImGui::TreeNode("Textures:"))
    {
        ImGui::Unindent();
        for (MaterialTextureParam& textureParam : textureParams.m_MaterialTextureParams)
        {
            Texture* pTexture = textureParam.GetTexture();
            if (pTexture && textureParam.m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::Text(textureParam.m_Name.c_str());
                ImGui::Image((void*)pTexture->GetResourceView(), { 150.0f, 150.0f });
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Are there scalars
    bool scalarDetected = false;
    MaterialScalarParamsMapping& scalarParams = m_pMaterial->GetScalarParams();
    for (MaterialScalarParam& scalarsParam : scalarParams.m_MaterialScalarParams)
    {
        if (scalarsParam.m_Resource->IsValid())
        {
            scalarDetected = true;
            break;
        }
    }
    //Make the tree
    if (scalarDetected && ImGui::TreeNode("Scalars:"))
    {
        ImGui::Unindent();
        for (MaterialScalarParam& scalarsParam : scalarParams.m_MaterialScalarParams)
        {
            if (scalarsParam.m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::SliderFloat(scalarsParam.m_Name.c_str(), &scalarsParam.m_Value, scalarsParam.m_Min, scalarsParam.m_Max);
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Separate each element with a line
    ImGui::TreePop();
}