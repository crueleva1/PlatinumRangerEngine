#pragma once

// Because it's relation with glm, so I want to make different with DirectX Utility Matrix So make source here

class ECSGLM_API CGLMPlane
{
    glm::vec3 m_kNormal;
    float m_fD;
public:
    enum EPlaneSide
    {
        ePS_None,
        ePS_Pos,
        ePS_Neg,
        ePS_Both,
    };

    CGLMPlane();

    CGLMPlane(glm::vec3& _rkNormal, float _fConstant);

    virtual ~CGLMPlane();

    EPlaneSide GetSide(const glm::vec3& _rkPos) const;

    EPlaneSide GetSide(const glm::vec3& _rkCenter, const glm::vec3& _rkHalf) const;

    float GetDistance(const glm::vec3& _rkPos) const;

    float Normalize();

    inline glm::vec3& GetNormal()
    {
        return m_kNormal;
    }

    inline float& GetConstant()
    {
        return m_fD;
    }
};