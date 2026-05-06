#pragma once


#include "GLMBaseCameraComponent.h"
#include "DataModifier.h"

struct ITexture;
class IFrameBuffer;
class ECSGLM_API CGLMCubeCameraComponent : public CGLMBaseCameraComponent
{
protected:
    struct SFaceVector
    {
        glm::vec3 m_kDir;
        glm::vec3 m_kUp;
    };
    static SFaceVector ms_akFaceVector[6];
    std::shared_ptr <IFrameBuffer> m_spkFrameBuffer;
    TDataModifier <glm::mat4> m_kTransform;
    TDataModifier <char> m_nCurrentFace;
    size_t m_anCameraNameHashs[6];
    int m_nRadius;
    bool m_bIsTransformModified;
    bool m_bCheckByDynamic;

    inline bool IsDirty()
    {
        if (m_kTransform.isDirty())
            return true;
        return m_nCurrentFace.isDirty();
    }

    inline void ClearDirty()
    {
        m_kTransform.clearDirty();
        m_nCurrentFace.clearDirty();
    }

    inline void ClearRedrawDirty()
    {
        m_bIsTransformModified = false;
    }
public:
    CGLMCubeCameraComponent();

    CGLMCubeCameraComponent(int _nRadius, int _nResolution);

    virtual ~CGLMCubeCameraComponent();

    virtual bool Initialize(std::shared_ptr <IFrameBuffer> spkFrameBuffer, int _nRadius);

    virtual void SetCurrentFace(char _nFace);

    virtual bool ActiveCurrentFace();

    virtual char GetCurrentFace();

    virtual void UpdateViewMatrix(class ISceneComponent& _rkComp);

    virtual void UpdatePerspectiveProjectMatrix(float _fNear, float _fFar, int _nWidth, int _nHeight);

    IFrameBuffer* GetTargetFrameBuffer();

    virtual bool IsInView(struct IBound* _pkBound, size_t _nHash = -1);

    virtual bool IsInView(const glm::vec3& _rkPoint);

    virtual bool IsInView(const glm::vec3& _rkPoint, const glm::vec3& _rkHalf);

    inline int GetRadius()
    {
        return m_nRadius;
    }

    inline const glm::mat4& GetTransform()
    {
        return m_kTransform.get();
    }

    virtual size_t GetCameraNameHash();

    virtual void UpdateCameraName(class ISceneComponent& _rkComp);

    void RenderComplate();

    inline void SetCheckByDyanmic(bool _bEnable)
    { 
        m_bCheckByDynamic = _bEnable;
    }
};