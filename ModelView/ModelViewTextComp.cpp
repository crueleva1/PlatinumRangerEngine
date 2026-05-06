#include "MainPCH.h"
#include "ModelViewTextComp.h"
#include "GLMSceneComponent.h"
#include "TextGeometryComp.h"

CModelViewTextComp::CModelViewTextComp()
{

}

CModelViewTextComp::~CModelViewTextComp()
{

}

bool CModelViewTextComp::ValidateText(entityx::Entity& _rkEntity)
{
    IFont* pkFont = GetFont();
    if (!pkFont)
        return false;
    if (!CDefaultTextComponent::ValidateText(_rkEntity))
        return false;

    if (!_rkEntity.has_component<ISceneHandler>() || !_rkEntity.has_component<IGeometryHandler>())
        return false;
    CTextGeometryComp* pkTextGeom = dynamic_cast <CTextGeometryComp*>(_rkEntity.component<IGeometryHandler>()->get());
    if (!pkTextGeom) {
        return false;
    }

    const std::string& kText = GetTextString();
    pkTextGeom->SetCharCount(kText.size());
    int nIndex = 0;
    for (auto cChar : kText) {
        const IFont::ICharacter* pkChar = pkFont->GetGlyph(cChar);
        pkTextGeom->SetCharInfo(pkChar, nIndex++);
    }
    return true;
}