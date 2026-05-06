#pragma once

enum EClampType
{
    eCT_U,
    eCT_V,
    eCT_W,
    eCT_Max,
};

enum EClampMode
{
    eCM_Clamp,
    eCM_ClampToEdge,
    eCM_ClampToBorder,
    eCM_MirroredRepeat,
    eCM_Repeat,
    eCM_MirrorClampToEdge,
};

enum EFilterType
{
    eFT_Min,
    eFT_Mag,
    eFT_Max,
};

enum EFilterMode
{
    eFM_Linear,
    eFM_Nearest,
    eFM_Linear_Mipmap_Linear,
    eFM_Linear_Mipmap_Nearest,
    eFM_Nearest_Mipmap_Linear,
    eFM_Nearest_Mipmap_Nearest,
};

struct ECS_API ITexture
{
    struct ECS_API IPixelData
    {
        size_t m_nSize;
        unsigned int m_nWidth;
        unsigned int m_nHeight;
        unsigned int m_nDepth;
        short m_nLevel;

        IPixelData(short _nLevel)
            : m_nWidth(-1)
            , m_nHeight(-1)
            , m_nDepth(-1)
            , m_nLevel(_nLevel)
        {
        }

        virtual ~IPixelData()
        {
        }

        virtual void Allocate(size_t _nSize)
        {
            m_nSize = _nSize;
        }

        virtual void SetData(const char* _pcData, size_t _nSize)
        {
            m_nSize = _nSize;
        }

        virtual char* GetData()
        {
            return nullptr;
        }

        virtual size_t GetSize()
        {
            return m_nSize;
        }

        virtual void ClearData()
        {
        }
    };

    virtual ~ITexture()
    {
    }

    virtual bool Active(int _nStage) = 0;

    virtual bool BindTexture() = 0;

    virtual bool FlushTexture() = 0;

    virtual bool SetTextureData(const char* _pcData, unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _eTextureFormat, int _nLevel = 0) = 0;

    virtual bool SetTextureDimension(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth, int _nLevel = 0) = 0;

    virtual unsigned int GetWidth(int _nLevel = 0) = 0;

    virtual unsigned int GetHeight(int _nLevel = 0) = 0;

    virtual unsigned int GetDepth(int _nLevel = 0) = 0;

    virtual int GetTextureFormat() = 0;

    virtual void SetTextureFormat(int _eTextureFormat) = 0;

    virtual char* GetTextureData(int _nLevel = 0) = 0;

    virtual void SetLevel(int _nLevel) = 0;

    virtual int GetLevel() = 0;

    virtual unsigned int GetTextureType() = 0;

    virtual int GetClampMode(EClampType) = 0;

    virtual int GetFilterMode(EFilterType) = 0;

    virtual short GetAnisotropy() = 0;

    virtual bool SetClampMode(EClampType _eType, EClampMode _nClamp) = 0;

    virtual bool SetFilterMode(EFilterType _eType, EFilterMode _nFilter) = 0;

    virtual bool SetAnisotropy(short _nAnisotropy) = 0;

    virtual bool IsCompressed(int _nLevel) = 0;

    virtual void SetAlignment(int _nAlign) = 0;

    virtual size_t GetPixelSize(unsigned int _nWidth, unsigned int _nHeight, unsigned int _nDepth) = 0;
};


struct ECS_API ITextureDesc
{
    virtual ~ITextureDesc()
    {
    }

    virtual const char* GetName() = 0;

    virtual const char* GetFileName() = 0;

    virtual const char* GetFileExt() = 0;

    virtual int GetTextureDim() = 0;

    virtual ITexture* GetRenderData() = 0;

    virtual void SetRenderData(std::shared_ptr <ITexture> _rkRenderData) = 0;
};

typedef std::shared_ptr <ITextureDesc> ITextureDescPtr;
