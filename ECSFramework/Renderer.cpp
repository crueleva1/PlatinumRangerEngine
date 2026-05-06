#include "ECSFrameworkPCH.h"
#include "Renderer.h"


CRenderer::CRenderer(short _nMajorVersion, short _nMinorVersion)
    :IRenderer(_nMajorVersion, _nMinorVersion)
{
    for (auto& _kActiveName : m_kActiveFactory) {
        _kActiveName = "Default";
    }
}

CRenderer::~CRenderer()
{

}

std::shared_ptr <IRenderState> CRenderer::OnCreateDefaultRenderState()
{
    IRenderStateFactoryObject* pkFactory = m_kRenderStateFactory.GetFactoryByType<IRenderStateFactoryObject>(m_kActiveFactory [eFT_RenderState]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)();
}

std::shared_ptr <IFrameBuffer> CRenderer::OnCreateFrameBuffer(int _nType, int _nWidth, int _nHeight)
{
    IFrameBufferFactoryObject* pkFactory = m_kFrameBufferFactory.GetFactoryByType<IFrameBufferFactoryObject>(m_kActiveFactory [eFT_FrameBuffer]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)(_nType, _nWidth, _nHeight);
}

std::shared_ptr <ITexture> CRenderer::OnCreateTexture(int _nTextureType)
{
    ITextureFactoryObject* pkFactory = m_kTextureFactory.GetFactoryByType<ITextureFactoryObject>(m_kActiveFactory [eFT_Texture]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)(_nTextureType);
}

std::shared_ptr <ICameraComponent> CRenderer::OnCreateCamera(ECameraType _nTextureType)
{
    ICameraCompFactoryObject* pkFactory = m_kCameraFactory.GetFactoryByType<ICameraCompFactoryObject>(m_kActiveFactory [eFT_Camera]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)(_nTextureType);
}

std::shared_ptr <IRenderComponent> CRenderer::OnCreateDefaultRenderCompoenet()
{
    IRenderCompFactorObject* pkFactory = m_kRenderComponentFactory.GetFactoryByType<IRenderCompFactorObject>(m_kActiveFactory [eFT_RenderComp]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)();
}

std::shared_ptr <IMeshBuffer> CRenderer::OnCreateRenderMeshBuffer()
{
    IMeshBufferFactory* pkFactory = m_kMeshBufferFactory.GetFactoryByType<IMeshBufferFactory>(m_kActiveFactory [eFT_MeshBuffer]);
    if (!pkFactory)
        return nullptr;
    return (*pkFactory)();
}