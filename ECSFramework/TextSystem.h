#pragma once


#include "ITextComponent.h"
#include "ITexture.h"

class IFont
{
public:
    class ICharacter
    {
    public:
        virtual ~ICharacter()
        {}

        virtual ITextureDescPtr GetTextureDesc() const = 0;

        virtual const glm::ivec2& GetSize() const = 0;

        virtual const glm::ivec2& GetBearing() const = 0;

        virtual unsigned int GetAdvance() const = 0;

        virtual unsigned long GetCharCode() const = 0;

        virtual int GetWidthPixelSize() const = 0;

        virtual int GetHeightPixelSize() const = 0;
    };

    virtual ~IFont()
    {
    }

    virtual const char* GetFontName() = 0;

    virtual const char* GetFontFile() = 0;

    virtual std::shared_ptr <struct ITextureDesc> GetTextureDesc(unsigned int _nCharCode) = 0;

    virtual const IFont::ICharacter* GetGlyph(unsigned int _nCharCode) = 0;

    virtual int GetWidthSize() = 0;

    virtual int GetHeightSize() = 0;

    virtual bool Initialize(class IMatertialSystem& _rkSystem) = 0;
};

class ITextContext
{
public:
    virtual ~ITextContext()
    {}

    virtual int GetContextType() const = 0;
};

class ITextSystem
    : public entityx::System <ITextSystem>
    , public entityx::Receiver <ITextSystem>
{
public:
    virtual ~ITextSystem()
    {
    }

    virtual IFont* GetFontByName(const char* _pcName) = 0;

    virtual void receive(const ITextContext&) = 0;
};