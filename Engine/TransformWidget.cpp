#include "pch.h"
#include "TransformWidget.h"
#include "EngineManager.h"
#include "Transform.h"
#include "RenderUtils.h"

TransformWidget::TransformWidget(Transform* pTransform)
    :Widget()
    , m_pTransform{ pTransform }
{
}
TransformWidget::~TransformWidget()
{
}

bool TransformWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("Transform:");
}
void TransformWidget::RenderUIElement(int idx)
{
    EngineManager* pEngineManager = EngineManager::Instance();
    RenderType renderType = EngineSettings::Instance()->GetRenderType();
    
    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 pos = m_pTransform->GetPosition();
    float positionArr[3] = { pos.x, pos.y, pos.z };
    ImGui::DragFloat3("Position", positionArr, 0.1f);

    //TO-DO: Make a more elegant way of having lighting updated
    //(perhaps marking as dirty? to be picked up by lightingwidgets?)
    if (std::fabs(pos.x - positionArr[0]) > DBL_EPSILON || std::fabs(pos.y - positionArr[1]) > DBL_EPSILON || std::fabs(pos.z - positionArr[2]) > DBL_EPSILON)
    {
        pEngineManager->GetRenderer(renderType)->SetShouldUpdateLighting(true);
    }

    m_pTransform->Translate(positionArr[0], positionArr[1], positionArr[2]);

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 rot = m_pTransform->GetRotationEuler();
    float rotationArr[3] = { rot.x, rot.y, rot.z };
    ImGui::DragFloat3("Rotation", rotationArr, 0.1f, -180.0f, 180.0f);
    
    if (std::fabs(rot.x - rotationArr[0]) > DBL_EPSILON || std::fabs(rot.y - rotationArr[1]) > DBL_EPSILON || std::fabs(rot.z - rotationArr[2]) > DBL_EPSILON)
    {
        pEngineManager->GetRenderer(renderType)->SetShouldUpdateLighting(true);
    }

    m_pTransform->Rotate(rotationArr[0], rotationArr[1], rotationArr[2]);

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 scale = m_pTransform->GetScale();
    float scaleArray[3] = { scale.x, scale.y, scale.z };
    ImGui::DragFloat3("Scale", scaleArray, 0.1f, 0.1f);

    if (std::fabs(scale.x - scaleArray[0]) > DBL_EPSILON || std::fabs(scale.y - scaleArray[1]) > DBL_EPSILON || std::fabs(scale.z - scaleArray[2]) > DBL_EPSILON)
    {
        pEngineManager->GetRenderer(renderType)->SetShouldUpdateLighting(true);
    }

    m_pTransform->Scale(scaleArray[0], scaleArray[1], scaleArray[2]);

    ImGui::Indent();
    ImGui::TreePop();
}