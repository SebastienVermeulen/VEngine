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
        //fix de render debug view, ku via ne debug vlagge peisn ke
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
        for (MaterialScalarParam& scalarParam : scalarParams.m_MaterialScalarParams)
        {
            if (scalarParam.m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::SliderFloat(scalarParam.m_Name.c_str(), &scalarParam.m_Value, scalarParam.m_Min, scalarParam.m_Max);
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Are there vectors
    bool vectorDetected = false;
    MaterialVectorParamsMapping& vectorParams = m_pMaterial->GetVectorParams();
    for (MaterialVectorParam& vectorParam : vectorParams.m_MaterialVectorParams)
    {
        if (vectorParam.m_Resource->IsValid())
        {
            vectorDetected = true;
            break;
        }
    }
    //Make the tree
    if (vectorDetected && ImGui::TreeNode("Vectors:"))
    {
        ImGui::Unindent();
        for (MaterialVectorParam& vectorParam : vectorParams.m_MaterialVectorParams)
        {
            if (vectorParam.m_Resource->IsValid())
            {
                ImGui::Separator();
                ImGui::ColorPicker4(vectorParam.m_Name.c_str(), &vectorParam.m_Value.x);
            }
        }
        ImGui::Indent();
        ImGui::TreePop();
    }

    //Separate each element with a line
    ImGui::TreePop();
}