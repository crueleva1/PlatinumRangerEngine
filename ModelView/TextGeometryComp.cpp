#include "MainPCH.h"
#include "TextGeometryComp.h"
#include "MeshData.h"
#include "IMaterialComponent.h"
#include "IRenderComponent.h"

CTextGeometryComp::CTextGeometryComp()
    : CCustomGeometryComp(CreateRectData(0.0f, 1.0f, 0.0f, 1.0f), eCMT_Rect)
{
    IRenderState* pkRenderState = GetRenderState();
    pkRenderState->SetDepthTestEnable(false);
    pkRenderState->SetDepthOpMask(false);
    pkRenderState->SetCullFaceEnable(false);
    pkRenderState->SetBlendEnable(true);
}

CTextGeometryComp::~CTextGeometryComp()
{

}

void CTextGeometryComp::ActiveGeometry(size_t _nIndex)
{
    if (_nIndex >= m_kStringChar.size())
        return;
    m_nActiveChar = _nIndex;
}

size_t CTextGeometryComp::GetGeometryCount()
{
    return m_kStringChar.size();
}

size_t CTextGeometryComp::CurrentActiveGeometry()
{
    return m_nActiveChar;
}

ITextureDesc* CTextGeometryComp::GetTextureDescByName(const char* _pcName)
{
    if (m_nActiveChar >= m_kStringChar.size())
        return nullptr;
    const IFont::ICharacter* pkChar = m_kStringChar[m_nActiveChar];
    if (!pkChar)
        return nullptr;
    ITextureDesc* pkTextureDesc = pkChar->GetTextureDesc().get();
    if (strstr(pkTextureDesc->GetName(), _pcName))
        return pkTextureDesc;
    return nullptr;
}

ITextureDesc* CTextGeometryComp::GetTextureDesc(size_t _nIndex)
{
    if (_nIndex >= m_kStringChar.size())
        return nullptr;
    const IFont::ICharacter* pkChar = m_kStringChar[_nIndex];
    if (!pkChar)
        return nullptr;
    return pkChar->GetTextureDesc().get();
}

IBuffer* CTextGeometryComp::GetVertexBuffer()
{
    IVertexBuffer* pkVertexBuffer = static_cast <IVertexBuffer*>(CCustomGeometryComp::GetVertexBuffer());

    if (!m_nActiveChar.isDirty())
        return pkVertexBuffer;

    m_nActiveChar.clearDirty();

    float nRightAdvance = 0.0f;
    float nLeftAdvance = 0.0f;
    float nHeight = 0.0f;
    float nYPos = 0.0f;
    int nLine = 3; // because height offset Y pos
    for (size_t nIndex = 0; nIndex < m_nActiveChar + 1; nIndex++) {
        const IFont::ICharacter* pkCharacter = m_kStringChar [nIndex];
        if (pkCharacter->GetCharCode() == '\n') {
            nRightAdvance = 0.0f;
            nLine++;
            continue;
        }
        if (nIndex == m_nActiveChar) {
            nLeftAdvance = nRightAdvance;
            float fYOffset = nLine * pkCharacter->GetHeightPixelSize();
            nHeight = pkCharacter->GetSize().y;
            nYPos = (pkCharacter->GetBearing().y - fYOffset - nHeight);
        }
        nRightAdvance += pkCharacter->GetSize().x;
        if (!pkCharacter->GetSize().x)
            nRightAdvance += (pkCharacter->GetAdvance() >> 6);
    }

    CGLMMeshBuffer* pkMesh = dynamic_cast<CGLMMeshBuffer*>(m_spkMesh.get());
    if (!pkMesh)
        return nullptr;

    std::vector <glm::vec3>& rkVertices = pkMesh->GetVertices();
    rkVertices [0].x = nLeftAdvance;
    rkVertices [0].y = nYPos + nHeight;
    rkVertices [0].z = 0.0f;
    rkVertices [1].x = nRightAdvance;
    rkVertices [1].y = nYPos + nHeight;
    rkVertices [1].z = 0.0f;
    rkVertices [2].x = nLeftAdvance;
    rkVertices [2].y = nYPos;
    rkVertices [2].z = 0.0f;
    rkVertices [3].x = nRightAdvance;
    rkVertices [3].y = nYPos;
    rkVertices [3].z = 0.0f;

    m_spkMesh->UpdateVertexBuffer(pkVertexBuffer);
    return pkVertexBuffer;
}

size_t CTextGeometryComp::GetTextureDescCount()
{
    return 1;
}

void CTextGeometryComp::SetCharCount(int _nCount)
{
    m_kStringChar.clear();
    m_kStringChar.resize(_nCount);
}

void CTextGeometryComp::SetCharInfo(const IFont::ICharacter* _pkChar, int _nIndex)
{
    if (!_pkChar)
        return;
    if (_nIndex >= m_kStringChar.size())
        return;

    m_kStringChar[_nIndex] = _pkChar;
}