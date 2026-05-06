#include "ImguiSystemPCH.h"
#include "ImguiRenderPipeline.h"
#include "CustomGeometryComp.h"
#include "GLITexture.h"
#include "GLSLMaterialSystem.h"

CImguiRenderPipeline::CImguiRenderPipeline()
    :m_pkMaterial(nullptr)
    ,m_bIsValidDraw(true)
{

}

CImguiRenderPipeline::~CImguiRenderPipeline()
{

}

void CImguiRenderPipeline::Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkMgr, entityx::EventManager& _rkEventMgr)
{
    CGLMDefaultPipeline::Prepared(_nWidth, _nHeight, _rkMgr, _rkEventMgr);
    // Temporary
    UpdateOrthoMatrix(_nWidth, _nHeight, (_nWidth / 2), -(_nHeight / 2), true);

    CGLSLMaterialCreateInstanceFromFile kArgs;
    std::string kFileName = "data/BasicVertexColorShader.vs";
    kArgs.SetShaderName("BasicVertexColorVertexShader", eShader_Vertex);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Vertex);
    kFileName = "data/BasicVertexColorShader.fs";
    kArgs.SetShaderName("BasicVertexColorPixelShader", eShader_Pixel);
    kArgs.SetShaderFile(kFileName.c_str(), eShader_Pixel);
    const IMaterialContextArg& rkArgs = kArgs;
    _rkEventMgr.emit <IMaterialContextArg>(rkArgs);
    m_pkMaterial = kArgs.GetMaterialInstance();
    CGLSLMaterial* pkMaterial = dynamic_cast <CGLSLMaterial*>(m_pkMaterial);
    if (pkMaterial != nullptr) {
        const size_t nSize = pkMaterial->GetConstantCount();
        for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
            IConstantSemantic* pkSemantic = pkMaterial->GetConstant(nIndex);
            if (!pkSemantic)
                continue;

            if (pkSemantic->GetType() != eConstant_Matrix4x4)
                continue;

            if (strstr(pkSemantic->GetVaribleName(), "WorldMatrix") || strstr(pkSemantic->GetVaribleName(), "ViewMatrix")) {
                TConstantSemantic <glm::mat4>* pkMat = static_cast <TConstantSemantic <glm::mat4>*>(pkSemantic);
                if (pkMat) {
                    pkMat->SetValue(glm::mat4(1.0f));
                }
            }
        }
    }

    if (m_kImguiMainEntity.valid())
        return;
    m_kImguiMainEntity = _rkMgr.create();
    auto spkRenderComp = IRenderSystem::CreateDefaultRenderCompoenet();
    m_kImguiMainEntity.assign<IRenderHandler>(spkRenderComp);
    m_spkGSC = std::make_shared<CCustomGeometryComp>(IRenderSystem::CreateRenderMeshBuffer(), eCMT_Polygun);
    m_kImguiMainEntity.assign<IGeometryHandler>(m_spkGSC);
    IRenderState* pkRenderState = m_spkGSC->GetRenderState();
    if (pkRenderState) {
        pkRenderState->SetBlendEnable(true);
        pkRenderState->SetBlendFactor(eBF_SrcAlpha, true, true);
        pkRenderState->SetBlendFactor(eBF_OneMinusSrcAlpha, false, true);
        pkRenderState->SetCullFaceEnable(false);
        pkRenderState->SetDepthTestEnable(false);
        pkRenderState->SetCullMode(eCM_FrontAndBack);
        pkRenderState->SetPolygonMode(ePM_Fill);
    }
}

void CImguiRenderPipeline::Active(entityx::EventManager& _rkEventMgr)
{

}

void CImguiRenderPipeline::Deactive(entityx::EventManager& _rkEventMgr)
{

}

IFrameBuffer* CImguiRenderPipeline::GetFrameBuffer(int _nCurrentPass)
{
    return CGLMDefaultPipeline::GetFrameBuffer(_nCurrentPass);
}

void CImguiRenderPipeline::SetPassObject(entityx::EntityManager& _rkMgr)
{
    //if (m_kImguiMainEntity.has_component())
}

void CImguiRenderPipeline::OnClear(class IRenderer& _rkRenderer, int _nCurrentPass)
{

}

void CImguiRenderPipeline::PassBegin(class IRenderer& _rkRenderer, int _nCurrentPass)
{

}

void CImguiRenderPipeline::OnDraw(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, int _nCurrentPass)
{
    if (!m_bIsValidDraw)
        return;

    if (!m_kImguiMainEntity.valid())
        return;

    if (!m_pkMaterial)
        return;

    if (!m_spkGSC)
        return;

    IRenderHandler& rkRenderHandler = (*m_kImguiMainEntity.component <IRenderHandler>().get());
    if (!rkRenderHandler->SetMaterial(m_pkMaterial))
        return;
    if (!rkRenderHandler->SetAttribute(m_spkGSC->GetVertexBuffer(), m_spkGSC->GetIndiceBuffer()))
        return;
    if (!rkRenderHandler->SetRenderState(m_spkGSC->GetRenderState()))
        return;
    AssignConstant<glm::mat4>(rkRenderHandler, "uProjectionMatrix", m_kOrthoMatrix, eConstant_Matrix4x4);
    AssignConstant<ITexture*>(rkRenderHandler, "uDiffuse", m_spkFontTxture.get(), eConstant_Texture);

    if (!rkRenderHandler->ValidatePipeline(_rkRenderer))
        return;

	ImDrawData* imDrawData = ImGui::GetDrawData();
	int32_t vertexOffset = 0;
	int32_t indexOffset = 0;
	for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {
		const ImDrawList* cmd_list = imDrawData->CmdLists [i];
		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer [j];
			_rkRenderer.SetScissor(std::max((int32_t)(pcmd->ClipRect.x), 0),
								   std::max((int32_t)(pcmd->ClipRect.y), 0),
								   (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x),
								   (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y));
			_rkRenderer.OnDrawOffset(_rkEventMgr, rkRenderHandler, _nCurrentPass, pcmd->ElemCount, indexOffset, 0);
			indexOffset += pcmd->ElemCount;
		}
		vertexOffset += cmd_list->VtxBuffer.Size;
	}
}

void CImguiRenderPipeline::PassEnd(class IRenderer& _rkRenderer, int _nCurrentPass)
{

}

void CImguiRenderPipeline::OnResize(class IRenderer& _rkRenderer, int _nWidth, int _nHeight)
{
    ImGuiIO& _rkIO = ImGui::GetIO();
    _rkIO.DisplaySize.x = _nWidth;
    _rkIO.DisplaySize.y = _nHeight;
}

bool CImguiRenderPipeline::CreateFontTexture(ImGuiIO& _rkIO)
{
    unsigned char* pcPixels;
    int width, height, bytes_per_pixel;
    _rkIO.Fonts->GetTexDataAsRGBA32(&pcPixels, &width, &height, &bytes_per_pixel);
    if (m_spkFontTxture) {
        if (!m_spkFontTxture->SetTextureData((const char*)pcPixels, width, height, 1, gli::FORMAT_RGBA8_UNORM))
            return false;
        return true;
    }
    // Upload texture to graphics system
    m_spkFontTxture = IRenderSystem::CreateTexture(IRenderer::eTT_2D);
    if (!m_spkFontTxture)
        return false;
    if (!m_spkFontTxture->SetTextureData((const char*)pcPixels, width, height, 1, gli::FORMAT_RGBA8_UNORM))
        return false;

    // Store our identifier
    _rkIO.Fonts->TexID = (ImTextureID)m_spkFontTxture.get();

    return true;
}

bool CImguiRenderPipeline::CreateMeshBuffer(ImDrawData* _pkDrawData)
{
    if (!m_pkMaterial)
        return false;

    if (!_pkDrawData->TotalIdxCount || !_pkDrawData->TotalVtxCount) {
        return false;
    }

    if (!m_spkGSC)
        return false;

    size_t nAttrStripSize = 0;
    const size_t nConut = m_pkMaterial->GetVariableDescCount();
    for (size_t nIndex = 0; nIndex < nConut; nIndex++) {
        IVariableAttrDesc* pkDesc = m_pkMaterial->GetVariableDesc(nIndex);
        if (!pkDesc)
            continue;
        nAttrStripSize += pkDesc->GetVariableSize();
    }
    size_t nVBSize = nAttrStripSize * _pkDrawData->TotalVtxCount;
    size_t nIBSize = sizeof(ImDrawIdx) * _pkDrawData->TotalIdxCount;

    CCustomGeometryComp* pkCustomGeo = dynamic_cast<CCustomGeometryComp*>(m_spkGSC.get());
    if (!pkCustomGeo)
        return false;

    IBuffer* pkVertexBuffer = m_spkGSC->GetVertexBuffer();
    IBuffer* pkIndiceBuffer = m_spkGSC->GetIndiceBuffer();
    const bool bHasGpuBuffers = (pkVertexBuffer != nullptr && pkIndiceBuffer != nullptr);
    const size_t nVBAllocated = bHasGpuBuffers ? pkVertexBuffer->GetSize() : 0;
    const size_t nIBAllocated = bHasGpuBuffers ? pkIndiceBuffer->GetSize() : 0;

    const bool bNeedsGrow = !bHasGpuBuffers || (nVBSize > nVBAllocated) || (nIBSize > nIBAllocated);
    const bool bShrinkRebuild = bHasGpuBuffers
        && ((nVBAllocated > 0 && (nVBSize * 2 <= nVBAllocated))
            || (nIBAllocated > 0 && (nIBSize * 2 <= nIBAllocated)));
    const bool bReuseGpuBuffers = bHasGpuBuffers && !bNeedsGrow && !bShrinkRebuild;

    if (!bReuseGpuBuffers) {
        pkCustomGeo->PurgeBuffer(0);
    }

    IMeshBuffer* pkMeshBuffer = pkCustomGeo->GetMeshBuffer();
    CGLMMeshBuffer* pkMesh = dynamic_cast<CGLMMeshBuffer*>(pkMeshBuffer);
    if (!pkMesh)
        return false;
    std::vector <glm::vec3> kPos(_pkDrawData->TotalVtxCount);
    std::vector <glm::vec3> kTextureCoord(_pkDrawData->TotalVtxCount);
    std::vector <glm::vec4> kVertexColor(_pkDrawData->TotalVtxCount);
    std::vector <unsigned int> kIndices(_pkDrawData->TotalIdxCount);

    int nGlobalVtxOffset = 0;
    unsigned int* pkIndices = &kIndices[0];

    for (int nIndex = 0; nIndex < _pkDrawData->CmdListsCount; nIndex++) {
        const ImDrawList* pkCmdList = _pkDrawData->CmdLists[nIndex];
        const ImDrawVert* pkVtxSrc = pkCmdList->VtxBuffer.Data;

        for (int nScrIdx = 0; nScrIdx < pkCmdList->VtxBuffer.Size; nScrIdx++) {
            int nGlobalIdx = nGlobalVtxOffset + nScrIdx;
            glm::vec3& rkPos = kPos[nGlobalIdx];
            glm::vec3& rkTextureCoord = kTextureCoord[nGlobalIdx];
            glm::vec4& rkVextexColor = kVertexColor[nGlobalIdx];
            rkPos.x = pkVtxSrc->pos.x;
            rkPos.y = pkVtxSrc->pos.y;
            rkPos.z = 0.0f;  
            rkTextureCoord.x = pkVtxSrc->uv.x;
            rkTextureCoord.y = pkVtxSrc->uv.y;
            rkTextureCoord.z = 0.0f;
            rkVextexColor.r = (float)((pkVtxSrc->col & 0x000000FF) >> 0) / 255.0f;
            rkVextexColor.g = (float)((pkVtxSrc->col & 0x0000FF00) >> 8) / 255.0f;
            rkVextexColor.b = (float)((pkVtxSrc->col & 0x00FF0000) >> 16) / 255.0f;
            rkVextexColor.a = (float)((pkVtxSrc->col & 0xFF000000) >> 24) / 255.0f;
            pkVtxSrc++;
        }

        const ImDrawIdx* pkIdxSrc = pkCmdList->IdxBuffer.Data;
        for (int nIdxIdx = 0; nIdxIdx < pkCmdList->IdxBuffer.Size; nIdxIdx++) {
            pkIndices[nIdxIdx] = (unsigned int)(pkIdxSrc[nIdxIdx]) + nGlobalVtxOffset;
        }
        pkIndices += pkCmdList->IdxBuffer.Size;

        nGlobalVtxOffset += pkCmdList->VtxBuffer.Size;
    }

    glm::vec3* ppkUV[] = { kTextureCoord.data(), nullptr };
    glm::vec4* ppkVC[] = { kVertexColor.data(), nullptr };
    pkMesh->SetMeshBuffer(ePrimitive_TriangleStrip,
                         _pkDrawData->TotalVtxCount,
                         _pkDrawData->TotalIdxCount,
                         kPos.data(),
                         nullptr,
                         nullptr,
                         nullptr,
                         1,
                         ppkUV,
                         ppkVC,
                         kIndices.data());

    if (bReuseGpuBuffers) {
        pkVertexBuffer = m_spkGSC->GetVertexBuffer();
        pkIndiceBuffer = m_spkGSC->GetIndiceBuffer();
        if (!pkVertexBuffer || !pkIndiceBuffer)
            return false;
        if (!pkMeshBuffer->UpdateVertexBuffer(pkVertexBuffer, false))
            return false;
        if (!pkMeshBuffer->UpdateIndexBuffer(pkIndiceBuffer, false))
            return false;
    }

    assert(m_spkGSC->GetVertexBuffer() != nullptr);
    assert(m_spkGSC->GetIndiceBuffer() != nullptr);
    return true;
}

void CImguiRenderPipeline::InvalidateResource(ImGuiIO& _rkIO)
{
    if (m_spkFontTxture) {
        assert(m_spkFontTxture.get() == _rkIO.Fonts->TexID);
        _rkIO.Fonts->TexID = nullptr;
        m_spkFontTxture = nullptr;
    }
    if (m_spkGSC) {
        m_spkGSC->PurgeBuffer(0);
    }
}