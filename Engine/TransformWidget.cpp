#include "pch.h"
#include "TransformWidget.h"
#include "Transform.h"

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
    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 pos = m_pTransform->GetPosition();
    float positionArr[3] = { pos.x, pos.y, pos.z };
    ImGui::SliderFloat3("Position", positionArr, -30.0f, 30.0f);
    m_pTransform->Translate(positionArr[0], positionArr[1], positionArr[2]);

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 ros = m_pTransform->GetRotationEuler();
    float rotationArr[3] = { ros.x, ros.y, ros.z };
    ImGui::SliderFloat3("Rotation", rotationArr, -180.0f, 180.0f);
    m_pTransform->Rotate(rotationArr[0], rotationArr[1], rotationArr[2]);

    //Separate each element with a line
    ImGui::Separator();

    const DirectX::XMFLOAT3 scale = m_pTransform->GetScale();
    float scaleArray[3] = { scale.x, scale.y, scale.z };
    ImGui::SliderFloat3("Scale", scaleArray, 0.1f, 5.0f);
    m_pTransform->Scale(scaleArray[0], scaleArray[1], scaleArray[2]);

    ImGui::Indent();
    ImGui::TreePop();
}