#include "pch.h"
#include "CameraWidget.h"
#include "Camera.h"

CameraWidget::CameraWidget(Camera* pCamera)
    :Widget()
    , m_pCamera{ pCamera }
{
}
CameraWidget::~CameraWidget()
{
}

bool CameraWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("Camera:");
}
void CameraWidget::RenderUIElement(int idx)
{
    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    CameraSettings settings = m_pCamera->GetCameraSettings();
    float FOV = DirectX::XMConvertToDegrees(settings.FOV);
    ImGui::SliderFloat("Farplane", &settings.farPlane, 0.1f, 100.0f);

    //Separate each element with a line
    ImGui::Separator();

    ImGui::SliderFloat("Nearplane", &settings.nearPlane, 0.1f, 100.0f);

    //Separate each element with a line
    ImGui::Separator();

    ImGui::SliderFloat("FOV", &FOV, 1.0f, 180.0f);
    if (settings.farPlane < settings.nearPlane)
    {
        settings.farPlane = settings.nearPlane + 1.0f;
    }
    settings.FOV = DirectX::XMConvertToRadians(FOV);
    m_pCamera->SetCameraSettings(settings);

    ImGui::Indent();
    ImGui::TreePop();
}
