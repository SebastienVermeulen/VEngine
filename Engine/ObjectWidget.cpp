#include "pch.h"
#include "ObjectWidget.h"
#include "ComponentStructure.h"
#include "Object.h"
#include "Transform.h"

ObjectWidget::ObjectWidget(Object* pOwningObject, std::string& name)
    :Widget()
    , m_pOwningObject{ pOwningObject }
    , m_Title{name}
    , m_WasSelected{false}
{
}
ObjectWidget::~ObjectWidget() 
{
}

bool ObjectWidget::RenderUITitle(int idx)
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (m_WasSelected) 
    {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }
    ImGui::TreeNodeEx((void*)(intptr_t)idx, node_flags, (m_Title + "_%d").c_str(), idx);
    return ImGui::IsItemClicked();
}
void ObjectWidget::RenderUIElement(int idx)
{
    if(m_pOwningObject->GetTransform()->GetTransformWidget()->RenderUITitle(0))
    {
        m_pOwningObject->GetTransform()->GetTransformWidget()->RenderUIElement(0);
    }

    std::vector<Component*> components = m_pOwningObject->GetAllComponents();
    for (int idx = 0; idx < components.size(); ++idx)
    {
        Widget* pWidget = components[idx]->GetWidget();
        if (pWidget) 
        {
            if (pWidget->RenderUITitle(0)) 
            {
                pWidget->RenderUIElement(0);
            }
        }
    }
}