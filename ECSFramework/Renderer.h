#pragma once


#include "RenderSystem.h"
#include "Factory.h"
#include <string>

class ECS_API IRenderStateFactoryObject : public IFactoryObj <IRenderState, std::string>
{
public:
    IRenderStateFactoryObject(const std::string& _rkName)
        : IFactoryObj <IRenderState, std::string>(_rkName)
    {
    }

    virtual ~IRenderStateFactoryObject()
    {
    }

    virtual std::shared_ptr <IRenderState> operator()() = 0;
};

class ECS_API IFrameBufferFactoryObject : public IFactoryObj <IFrameBuffer, std::string>
{
public:
    IFrameBufferFactoryObject(const std::string& _rkName)
        : IFactoryObj <IFrameBuffer, std::string>(_rkName)
    {
    }

    virtual ~IFrameBufferFactoryObject()
    {
    }

    virtual std::shared_ptr <IFrameBuffer> operator()(int _nType, int _nWidth, int _nHeight) = 0;
};

class ECS_API ITextureFactoryObject : public IFactoryObj <ITexture, std::string>
{
public:
    ITextureFactoryObject(const std::string& _rkName)
        : IFactoryObj <ITexture, std::string>(_rkName)
    {
    }

    virtual ~ITextureFactoryObject()
    {
    }

    virtual std::shared_ptr <ITexture> operator()(int _nTextureType) = 0;
};

class ECS_API ICameraCompFactoryObject : public IFactoryObj <ICameraComponent, std::string>
{
public:
    ICameraCompFactoryObject(const std::string& _rkName)
        :IFactoryObj <ICameraComponent, std::string>(_rkName)
    {
    }

    virtual ~ICameraCompFactoryObject()
    {
    }

    virtual std::shared_ptr <ICameraComponent> operator()(IRenderer::ECameraType _nTextureType) = 0;
};

class ECS_API IRenderCompFactorObject : public IFactoryObj <IRenderComponent, std::string>
{
public:
    IRenderCompFactorObject(const std::string& _rkName)
        :IFactoryObj <IRenderComponent, std::string>(_rkName)
    {
    }

    virtual ~IRenderCompFactorObject()
    {
    }

    virtual std::shared_ptr <IRenderComponent> operator()() = 0;
};

class ECS_API IMeshBufferFactory : public IFactoryObj <IMeshBuffer, std::string>
{
public:
    IMeshBufferFactory(const std::string& _rkName)
        :IFactoryObj <IMeshBuffer, std::string>(_rkName)
    {
    }

    virtual ~IMeshBufferFactory()
    {
    }

    virtual std::shared_ptr <IMeshBuffer> operator()() = 0;
};

class ECS_API CRenderer : public IRenderer
{
public:
    enum EFactoryType
    {
        eFT_RenderState,
        eFT_FrameBuffer,
        eFT_Texture,
        eFT_Camera,
        eFT_RenderComp,
        eFT_MeshBuffer,
        eFT_Max,
    };
private:
    IFactoryMgr <IRenderState,      std::string> m_kRenderStateFactory;
    IFactoryMgr <IFrameBuffer,      std::string> m_kFrameBufferFactory;
    IFactoryMgr <ITexture,          std::string> m_kTextureFactory;
    IFactoryMgr <ICameraComponent,  std::string> m_kCameraFactory;
    IFactoryMgr <IRenderComponent,  std::string> m_kRenderComponentFactory;
    IFactoryMgr <IMeshBuffer,       std::string> m_kMeshBufferFactory;
    std::string m_kActiveFactory [eFT_Max];
public:

    CRenderer(short _nMajorVersion, short _nMinorVersion);

    virtual ~CRenderer();

    template <class T>
    inline bool RegisterRenderStateFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kRenderStateFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterRenderStateFactory(std::string& _kType)
    {
        return m_kRenderStateFactory.UnRegisterFactory(_kType);
    }

    template <class T>
    inline bool RegisterFrameBufferFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kFrameBufferFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterFrameBufferFactory(std::string& _kType)
    {
        return m_kFrameBufferFactory.UnRegisterFactory(_kType);
    }

    template <class T>
    inline bool RegisterTextureFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kTextureFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterTextureFactory(std::string& _kType)
    {
        return m_kTextureFactory.UnRegisterFactory(_kType);
    }

    template <class T>
    inline bool RegisterCameraFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kCameraFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterCameraFactory(std::string& _kType)
    {
        return m_kCameraFactory.UnRegisterFactory(_kType);
    }
    template <class T>
    inline bool RegisterRenderComponentFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kRenderComponentFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterRenderComponentFactory(std::string& _kType)
    {
        return m_kRenderComponentFactory.UnRegisterFactory(_kType);
    }

    template <class T>
    inline bool RegisterMeshBufferFactory(std::shared_ptr <T> _pkFactory)
    {
        return m_kMeshBufferFactory.RegisterFactory(_pkFactory);
    }

    inline bool UnRegisterMeshBufferFactory(std::string& _kType)
    {
        return m_kMeshBufferFactory.UnRegisterFactory(_kType);
    }

    inline void ActiveFactory(EFactoryType _eType, const std::string& _rkName)
    {
        if (_eType < eFT_RenderState || _eType >= eFT_Max)
            return;
        m_kActiveFactory [_eType] = _rkName;
    }

    virtual std::shared_ptr <IRenderState> OnCreateDefaultRenderState();

    virtual std::shared_ptr <IFrameBuffer> OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight);

    virtual std::shared_ptr <ITexture> OnCreateTexture(int _nTextureType);

    virtual std::shared_ptr <ICameraComponent> OnCreateCamera(ECameraType _nTextureType);

    virtual std::shared_ptr <IRenderComponent> OnCreateDefaultRenderCompoenet();

    virtual std::shared_ptr <IMeshBuffer> OnCreateRenderMeshBuffer();
};