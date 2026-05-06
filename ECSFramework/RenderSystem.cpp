#include "ECSFrameworkPCH.h"
#include "RenderSystem.h"

std::shared_ptr <IRenderer> IRenderSystem::m_pkRenderer;
IRenderSystem* IRenderSystem::m_pkRenderSystem = nullptr;

void IRenderSystem::shutdown(entityx::EventManager& events)
{
    ResetSubPipeline();
    m_pkRenderer = nullptr;
}

void IRenderSystem::update(entityx::EntityManager& _rkEntityMgr, entityx::EventManager& _rkEventMgr, entityx::TimeDelta dt)
{
    if (m_pkRenderer == nullptr)
        return;
    IRenderPipeline* pkPipeline = GetHead();
    do 
    {
        int nMaxPass = 1;
        if (pkPipeline) {
            pkPipeline->SetPassObject(_rkEntityMgr);
            nMaxPass = pkPipeline->GetRenderPassCount();
        }
        m_nCurrentPass = 0;
        while (m_nCurrentPass < nMaxPass) {
            if (pkPipeline)
                pkPipeline->PassBegin(*m_pkRenderer, m_nCurrentPass);
            BeginFrame();
            pkPipeline->OnDraw(*m_pkRenderer, _rkEventMgr, m_nCurrentPass);
            EndFrame();
            if (pkPipeline)
                pkPipeline->PassEnd(*m_pkRenderer, m_nCurrentPass);
            m_nCurrentPass++;
        }
        if (!GetNext())
            break;
    } while (pkPipeline = GetRenderPipeline());
    SwapBuffer();
}

void IRenderSystem::receive(const IRenderContextArg& _rkArg)
{
    if (m_pkRenderer == nullptr)
        return;

    m_pkRenderer->receive(_rkArg);
}

void IRenderSystem::BeginFrame()
{
    int nCurrentPass = GetCurrentPass();
    IRenderPipeline* pkPipeline = GetRenderPipeline();
    m_pkRenderer->BeginFrame(pkPipeline, nCurrentPass);
}

void IRenderSystem::EndFrame()
{
    int nCurrentPass = GetCurrentPass();
    IRenderPipeline* pkPipeline = GetRenderPipeline();
    m_pkRenderer->EndFrame(pkPipeline, nCurrentPass);
}

void IRenderSystem::SwapBuffer()
{
    m_pkRenderer->OnSwapBuffers();
}