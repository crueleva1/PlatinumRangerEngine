#include "MainPCH.h"
#include "ModelViewTextSystem.h"
#include "GLSLMaterialSystem.h"
#include "RenderSystem.h"
#include "ModelViewTextComp.h"
#include "TextGeometryComp.h"
#include "ft2build.h"
#include "freetype/freetype.h"
#include "OpenGLTexture.h"
#include "TextureDesc.h"

class ECS_MODELVIEW_API CFontTextureDesc : public CTextureDesc
{
    glm::vec2 m_kNTextureCoord;
    glm::vec2 m_kPTextureCoord;
public:
    CFontTextureDesc(const char* _pcName, const char* _pcFileName)
        : CTextureDesc(_pcName, _pcFileName, "ttf", IRenderer::eTT_2D)
    {}

    virtual ~CFontTextureDesc()
    {}

    inline void SetTextureCoord(const glm::vec2& _rkNTextureCoord, const glm::vec2& _rkPTextureCoord)
    {
        m_kNTextureCoord = _rkNTextureCoord;
        m_kPTextureCoord = _rkPTextureCoord;
    }

    inline const glm::vec2& GetNTextureCoord()
    {
        return m_kNTextureCoord;
    }

    inline const glm::vec2& GetPTextureCoord()
    {
        return m_kPTextureCoord;
    }
};

CFreeTypeFont::CCharacter::CCharacter(CFreeTypeFont& _rkFont, ITextureDescPtr _spkDesc, const glm::ivec2& _rkSize, const glm::ivec2& _rkBearing, unsigned int _nAdvance, unsigned long _nCharCode)
    : m_rkFont (_rkFont)
    , m_spkTextureDesc (_spkDesc)
    , m_kSize (_rkSize)
    , m_kBearing (_rkBearing)
    , m_nAdvance (_nAdvance)
    , m_nCharCode (_nCharCode)
{
}

CFreeTypeFont::CCharacter::~CCharacter()
{}

ITextureDescPtr CFreeTypeFont::CCharacter::GetTextureDesc () const
{
    return m_spkTextureDesc;
}

const glm::ivec2& CFreeTypeFont::CCharacter::GetSize() const
{
    return m_kSize;
}

const glm::ivec2& CFreeTypeFont::CCharacter::GetBearing() const
{
    return m_kBearing;
}

unsigned int CFreeTypeFont::CCharacter::GetAdvance() const
{
    return m_nAdvance;
}

unsigned long CFreeTypeFont::CCharacter::GetCharCode() const
{
    return m_nCharCode;
}

int CFreeTypeFont::CCharacter::GetWidthPixelSize() const
{
    return m_rkFont.GetWidthSize();
}

int CFreeTypeFont::CCharacter::GetHeightPixelSize() const
{
    return m_rkFont.GetHeightSize();
}

CFreeTypeFont::CFreeTypeFont(const char* _pcFontName, const char* _pcFontFile, int _nPixelWidth, int _nPixelHeight)
    : m_kName(_pcFontName)
    , m_kFileName(_pcFontFile)
    , m_nPixelWidth(_nPixelWidth)
    , m_nPixelHeight(_nPixelHeight)
{

}

CFreeTypeFont::~CFreeTypeFont()
{

}

const char* CFreeTypeFont::GetFontName()
{
    return m_kName.c_str();
}

const char* CFreeTypeFont::GetFontFile()
{
    return m_kFileName.c_str();
}

std::shared_ptr <ITextureDesc> CFreeTypeFont::GetTextureDesc(unsigned int _nCharCode)
{
    auto kIt = m_kFontGlyphs.find(_nCharCode);
    if (kIt == m_kFontGlyphs.end())
        return nullptr;
    return kIt->second.m_spkTextureDesc;
}

const IFont::ICharacter* CFreeTypeFont::GetGlyph(unsigned int _nCharCode)
{
    auto kIt = m_kFontGlyphs.find (_nCharCode);
    if (kIt == m_kFontGlyphs.end ())
        return nullptr;
    return &kIt->second;
}

int CFreeTypeFont::GetWidthSize()
{
    return m_nPixelWidth;
}

int CFreeTypeFont::GetHeightSize()
{
    return m_nPixelHeight;
}

bool CFreeTypeFont::Initialize(IMatertialSystem& _rkSystem)
{
    FT_Library kFontLibrary;
    if (FT_Init_FreeType(&kFontLibrary))
        return false;

    FT_Face kFontFace;
    if (FT_New_Face(kFontLibrary, m_kFileName.c_str(), 0, &kFontFace))
        return false;

    FT_Set_Pixel_Sizes(kFontFace, m_nPixelWidth, m_nPixelHeight);

    std::shared_ptr <CTextureDesc> spkTextureDesc = nullptr;
    for (short nCount = 0; nCount < 128; nCount++) {
        if (FT_Load_Char(kFontFace, nCount, FT_LOAD_RENDER))
            continue;
        char cBuffer [256] = {};
        sprintf_s(cBuffer, sizeof(cBuffer), "%s_%hd", m_kName.c_str(), nCount);
        spkTextureDesc = std::make_shared <CTextureDesc> ("uDiffuse", cBuffer, "ttf", IRenderer::eTT_2D);
        std::shared_ptr <ITexture> spkTexture = _rkSystem.CreateRenderData(spkTextureDesc.get(), true);
        if (!spkTexture)
            return false;
        spkTexture->SetTextureData((const char*)kFontFace->glyph->bitmap.buffer, kFontFace->glyph->bitmap.width, kFontFace->glyph->bitmap.rows, 1, gli::FORMAT_R8_UNORM);
        spkTexture->SetAlignment(1);
        spkTexture->SetClampMode(eCT_U, eCM_ClampToEdge);
        spkTexture->SetClampMode(eCT_V, eCM_ClampToEdge);
        spkTexture->SetFilterMode(eFT_Min, eFM_Linear);
        spkTexture->SetFilterMode(eFT_Mag, eFM_Linear);
        // flush
        spkTexture->BindTexture();
        spkTextureDesc->SetRenderData(spkTexture);
        glm::ivec2 kSize(kFontFace->glyph->bitmap.width, kFontFace->glyph->bitmap.rows);
        glm::ivec2 kBearing(kFontFace->glyph->bitmap_left, kFontFace->glyph->bitmap_top);
        CCharacter kChar(*this,  spkTextureDesc, kSize, kBearing, kFontFace->glyph->advance.x, nCount);
        m_kFontGlyphs.insert(std::make_pair(nCount, kChar));
    }
    FT_Done_Face(kFontFace);
    FT_Done_FreeType(kFontLibrary);
    return true;
}

CCreateTextCompContext::CCreateTextCompContext(entityx::Entity& _rkEntity, const char* _pcFontName, const char* _pcText)
    : m_rkEntity(_rkEntity)
    , m_kFontName(_pcFontName)
    , m_kText(_pcText)
{

}

CCreateTextCompContext::~CCreateTextCompContext()
{

}

int CCreateTextCompContext::GetContextType() const
{
    return eTCT_CreateTextComp;
}

entityx::Entity CCreateTextCompContext::GetEntity() const
{
    return m_rkEntity;
}

const char* CCreateTextCompContext::GetFontName () const
{
    return m_kFontName.c_str();
}

const char* CCreateTextCompContext::GetText() const
{
    return m_kText.c_str();
}


CModelViewTextSystem::CModelViewTextSystem(IMatertialSystem& _rkMaterialSystem)
    : m_rkMaterialSystem(_rkMaterialSystem)
    , m_pkTextMaterial(nullptr)
{

}

CModelViewTextSystem::~CModelViewTextSystem()
{

}

void CModelViewTextSystem::configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr)
{
    std::shared_ptr <CFreeTypeFont> spkFont = std::make_shared <CFreeTypeFont> ("monaco-20" ,"data/MONACO.TTF", 20, 20);
    bool bResult = spkFont->Initialize(m_rkMaterialSystem);
    assert(bResult);
    m_kFontLibrary.insert(std::make_pair(spkFont->GetFontName(), spkFont));

    CGLSLMaterialCreateInstanceFromFile kArgs;
    kArgs.SetShaderName("TextVertexShader", eShader_Vertex);
    kArgs.SetShaderFile("data/TextShader.vs", eShader_Vertex);
    kArgs.SetShaderName("TextPixelShader", eShader_Pixel);
    kArgs.SetShaderFile("data/TextShader.fs", eShader_Pixel);
    const IMaterialContextArg& rkIArgs = kArgs;
    _rkEventMgr.emit <IMaterialContextArg> (rkIArgs);
    m_pkTextMaterial = kArgs.GetMaterialInstance();

    _rkEventMgr.subscribe <ITextContext, ITextSystem>(*this);
    m_pkEventMgr = &_rkEventMgr;
}

void CModelViewTextSystem::update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta _nDelta)
{
    for (auto kEntity : _rkEntityMgr.entities_with_components <ITextHandler>()) {
        ITextHandler& rkText = (*kEntity.component <ITextHandler> ().get());
        rkText->ValidateText(kEntity);
    }
}

void CModelViewTextSystem::receive(const ITextContext& _rkContext)
{
    ETextContextType eType = (ETextContextType)_rkContext.GetContextType();
    switch (eType)
    {
        case eTCT_CreateTextComp:
        {
            const CCreateTextCompContext& rkContext = static_cast<const CCreateTextCompContext&>(_rkContext);
            CreateTextComppent(rkContext);
            break;
        }
        default:
            break;
    }
}

IFont* CModelViewTextSystem::GetFontByName(const char* _pcName)
{
    auto kIt = m_kFontLibrary.find(_pcName);
    if (kIt == m_kFontLibrary.end())
        return nullptr;
    return kIt->second.get();
}

bool CModelViewTextSystem::CreateTextComppent(const CCreateTextCompContext& _rkContext)
{
    entityx::Entity kEntity = _rkContext.GetEntity();
    if (!kEntity)
        return false;
    IFont* pkFont = GetFontByName(_rkContext.GetFontName());
    if (!pkFont)
        return false;

    ITextComponent* pkTextComp = nullptr;
    if (kEntity.has_component <ITextHandler>()) {
        pkTextComp = kEntity.component<ITextHandler>()->get();
    }

    if (!pkTextComp) {
        std::shared_ptr <ITextComponent> spkTextComp = std::make_shared <CModelViewTextComp>();
        pkTextComp = spkTextComp.get();
        kEntity.assign <ITextHandler>(spkTextComp);
    }

    pkTextComp->SetFont(pkFont);
    pkTextComp->SetText(_rkContext.GetText());

    CTextGeometryComp* pkGeomTextComp = nullptr;
    if (kEntity.has_component <IGeometryHandler>()) {
        CTextGeometryComp* pkGeomTextComp = dynamic_cast <CTextGeometryComp*>(kEntity.component<IGeometryHandler>()->get());
        if (!pkGeomTextComp)
            kEntity.remove <IGeometryHandler>();
    }

    if (!pkGeomTextComp) {
        std::shared_ptr <CTextGeometryComp> spkGeom = std::make_shared <CTextGeometryComp>();
        pkGeomTextComp = spkGeom.get();
        kEntity.assign<IGeometryHandler>(spkGeom);
    }

    CGLSLMaterialCreateComponentFromFile kArgs(kEntity);
    std::string kFileName = "data/TextShader.vs";
    kArgs.SetShaderName("TextVertexShader", eShader_Vertex);
    kFileName = "data/TextShader.fs";
    kArgs.SetShaderName("TextPixelShader", eShader_Pixel);
    const IMaterialContextArg& rkIArgs = kArgs;
    m_pkEventMgr->emit <IMaterialContextArg>(rkIArgs);

    return true;
}