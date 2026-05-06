#pragma once


#include "TextSystem.h"
#include "IMaterialComponent.h"
#include "ITexture.h"

class ECS_MODELVIEW_API CFreeTypeFont : public IFont
{
public:
    class ECS_MODELVIEW_API CCharacter : public IFont::ICharacter
    {
        CFreeTypeFont& m_rkFont;
        friend class CFreeTypeFont;
        ITextureDescPtr m_spkTextureDesc;
        glm::ivec2 m_kSize;
        glm::ivec2 m_kBearing;
        unsigned long m_nCharCode;
        unsigned int m_nAdvance;
    public:
        CCharacter(CFreeTypeFont& _rkFont, ITextureDescPtr _spkDesc, const glm::ivec2& _rkSize, const glm::ivec2& _rkBearing, unsigned int _nAdvance, unsigned long _nCharCode);

        virtual ~CCharacter();

        virtual ITextureDescPtr GetTextureDesc() const;

        virtual const glm::ivec2& GetSize() const;

        virtual const glm::ivec2& GetBearing() const;

        virtual unsigned int GetAdvance() const;

        virtual unsigned long GetCharCode() const;

        virtual int GetWidthPixelSize() const;

        virtual int GetHeightPixelSize() const;
    };
protected:
    std::string m_kName;
    std::string m_kFileName;
    std::map <unsigned int, CFreeTypeFont::CCharacter> m_kFontGlyphs;
    int m_nPixelWidth;
    int m_nPixelHeight;
public:
    CFreeTypeFont(const char* _ckFontName, const char* _ckFontFileName, int _nPixelWidth, int _nPixelHeight);

    virtual ~CFreeTypeFont();

    virtual const char* GetFontName();

    virtual const char* GetFontFile();

    virtual std::shared_ptr <ITextureDesc> GetTextureDesc(unsigned int _nCharCode);

    virtual const IFont::ICharacter* GetGlyph(unsigned int _nCharCode);

    virtual int GetWidthSize();

    virtual int GetHeightSize();

    virtual bool Initialize(class IMatertialSystem& _rkSystem);
};

enum ETextContextType
{
    eTCT_CreateTextComp,
};

class ECS_MODELVIEW_API CCreateTextCompContext : public ITextContext
{
    entityx::Entity& m_rkEntity;
    std::string m_kFontName;
    std::string m_kText;
public:
    CCreateTextCompContext(entityx::Entity& _rkEntity, const char* _pcFontName, const char* _pcText);

    virtual ~CCreateTextCompContext();

    int GetContextType() const;

    entityx::Entity GetEntity() const;

    const char* GetFontName() const;

    const char* GetText() const;
};

class ECS_MODELVIEW_API CModelViewTextSystem : public ITextSystem
{
    class IMatertialSystem& m_rkMaterialSystem;
    std::map <std::string, std::shared_ptr <IFont> > m_kFontLibrary;
    entityx::EventManager* m_pkEventMgr;
    IMaterial* m_pkTextMaterial;
public:

    CModelViewTextSystem(class IMatertialSystem& _rkMaterialSystem);

    virtual ~CModelViewTextSystem();

    virtual void configure(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr);

    virtual void update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta _nDelta);

    virtual void receive(const ITextContext&);

    virtual IFont* GetFontByName(const char* _pcName);

    virtual bool CreateTextComppent(const CCreateTextCompContext& _rkContext);
};