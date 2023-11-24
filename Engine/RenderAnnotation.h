#pragma once
#include "Singleton.h"

#pragma region Macros
#define V_DX11_ANNOTATE(log, ...) RenderAnnotation annotation(log);
#define V_DX11_DEANNOTATE(annotation, ...) RenderAnnotationsContainer::Instance()->RemoveAnnotation(annotation);
#pragma endregion

struct RenderAnnotation final
{
	RenderAnnotation(const std::wstring annotation);
	~RenderAnnotation();

	int m_ID;
	bool m_WasRemoved = false;
};

class RenderAnnotationsContainer final : public Singleton<RenderAnnotationsContainer>
{
public:
	RenderAnnotationsContainer();
	virtual ~RenderAnnotationsContainer();

	HRESULT Init(ID3D11DeviceContext* pDeviceContext);

	RenderAnnotationsContainer(RenderAnnotationsContainer& other) = delete;
	RenderAnnotationsContainer(RenderAnnotationsContainer&& other) = delete;
	RenderAnnotationsContainer operator=(RenderAnnotationsContainer& other) = delete;
	RenderAnnotationsContainer& operator=(RenderAnnotationsContainer&& other) = delete;

	void MakeAnnotation(const std::wstring NewAnnotation, RenderAnnotation& renderAnnotation);
	void RemoveAnnotation(RenderAnnotation& renderAnnotation);

private:
	ID3DUserDefinedAnnotation* m_pAnnotationsInterface;

	std::stack<RenderAnnotation*> m_Annotations;
};
