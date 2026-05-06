#pragma once


#include "CustomGeometryComp.h"
#include "TextSystem.h"
#include "DataModifier.h"

class ECS_MODELVIEW_API CTextGeometryComp : public CCustomGeometryComp
{
    std::vector <const IFont::ICharacter*> m_kStringChar;
    TDataModifier<size_t> m_nActiveChar;
public:
    CTextGeometryComp();

    virtual ~CTextGeometryComp();

    virtual void ActiveGeometry(size_t _nIndex);

    virtual size_t GetGeometryCount();

    virtual size_t CurrentActiveGeometry();

    virtual ITextureDesc* GetTextureDescByName(const char* _pcName);

    virtual struct ITextureDesc* GetTextureDesc(size_t _nIndex);

    virtual IBuffer* GetVertexBuffer();

    virtual size_t GetTextureDescCount();

    void SetCharCount(int _nCount);

    void SetCharInfo(const IFont::ICharacter* _pkChar, int _nIndex);
};