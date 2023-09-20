#include "pch.h"
#include "PostProcessWidget.h"
#include "EngineManager.h"
#include "PostProcesses.h"
#include "GreyScale.h"

// |----------------------------|
// | GreyScalePostProcessWidget |
// |----------------------------|

GreyScalePostProcessWidget::GreyScalePostProcessWidget(GreyScalePostProcess* pGreyScalePostProcess) 
	: m_pGreyScalePostProcess{ pGreyScalePostProcess }
{
}
GreyScalePostProcessWidget::~GreyScalePostProcessWidget() 
{
}

bool GreyScalePostProcessWidget::RenderUITitle(int idx)
{
    return ImGui::TreeNode("GreyScalePostProcessWidget:");
}
void GreyScalePostProcessWidget::RenderUIElement(int idx) 
{
    // TO-DO: Add some more controls here? Like a lerp percentage wise perhaps
    ImGui::Unindent();

    //Separate each element with a line
    ImGui::Separator();

    bool isActive = m_pGreyScalePostProcess->IsActive();
    ImGui::Checkbox("Active", &isActive);
    m_pGreyScalePostProcess->SetActive(isActive);

    ImGui::Indent();
    ImGui::TreePop();
}
