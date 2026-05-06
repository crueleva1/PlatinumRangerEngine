#pragma once


#include "GLMPipeline.h"

class ECS_MODELVIEW_API CModelViewPipeline : public CGLMDefaultPipeline
{
protected:
    struct SBoundBox
    {
        glm::vec3 m_kCenter;
        glm::vec3 m_kHalf;
        glm::vec4 m_kColor;

        SBoundBox(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor)
            : m_kCenter(_rkCenter)
            , m_kHalf(_rkHalf)
            , m_kColor()
        {
        }

        static std::shared_ptr <SBoundBox> Create(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor)
        {
            return std::make_shared <SBoundBox>(_rkCenter, _rkHalf, _rkColor);
        }

        static void SetData(std::shared_ptr <SBoundBox> spkBox, const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor)
        {
            spkBox->m_kCenter = _rkCenter;
            spkBox->m_kHalf = _rkHalf;
            spkBox->m_kColor = _rkColor;
        }
    };
    CRecyclePool <std::shared_ptr <SBoundBox>, unsigned int> m_kBoundBoxPool;
    std::vector <entityx::Entity> m_kLightEntity;
    std::vector <unsigned int> m_kRenderBoxIndex;
    static std::shared_ptr <IMeshBuffer> ms_spkBoxMesh;
    static std::shared_ptr <IBuffer> ms_spkBoxVB;
    static std::shared_ptr <IBuffer> ms_spkBoxIB;
    static std::shared_ptr <IRenderState> ms_spkBoxRenderState;
    class CGLMBaseCameraComponent* m_pkMainCamera;
    IMaterial* m_pkBasicMaterial;
    entityx::EventManager& m_rkEventMgr;
    IRenderHandler m_kTemplateRenderComp;
    entityx::Entity m_kTextEntity;
    std::string m_kScreenText;
    unsigned int m_nFrameRevision;
    static bool ms_bDrawBinTreeBox;
    static bool ms_bDrawObjectBound;

    void SetCreateMaterialEvent(const class IMaterialContextArg&);

    void AddRenderBoundBoxList(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    void AppendSceenText(const std::string& _rkText);

    void AppendSceenText(const char* _pcText, ...);

    virtual void Active(entityx::EventManager& _rkEventMgr);

    virtual void Deactive(entityx::EventManager& _rkEventMgr);

    virtual void SetScreenText();
public:
    CModelViewPipeline(entityx::EventManager& _rkEventMgr, int _nWidth, int _nHeight);

    virtual ~CModelViewPipeline();

    virtual void Prepared(int _nWidth, int _nHeight, entityx::EntityManager& _rkEntitykMgr, entityx::EventManager& _rkEventMgr);

    void RenderBox(class IRenderer& _rkRenderer, const glm::vec3& _rkCenter, const glm::vec3& _rkHalf, const glm::vec4& _rkColor, class ICameraComponent* _pkCamera);

    virtual bool IsDeferred() = 0;

    virtual void SetPassObject(entityx::EntityManager& _rkMgr);

    virtual void RenderScreen(class IRenderer& _rkRenderer, entityx::EventManager& _rkEventMgr, class ICameraComponent* _pkCamera, const glm::mat4& _rkOrtha);

    static void SetDrawBinTreeBox(bool _bEnable)
    {
        ms_bDrawBinTreeBox = _bEnable;
    }

    static void SetDrawObjectBound(bool _bEnable)
    {
        ms_bDrawObjectBound = _bEnable;
    }
};