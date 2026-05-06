#pragma once

#include <memory>
#include "ScopeHelper.h"
#include "RecyclePool.h"

template <size_t nDimension>
class TPoint
{
    float m_afPoint[nDimension];
public:
    TPoint()
    {
        memset(m_afPoint, 0, sizeof(m_afPoint));
    }

    TPoint(const TPoint<nDimension>& _rkPoint)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++)
            m_afPoint[nIndex] = _rkPoint.m_afPoint[nIndex];
    }

    inline float& operator[](size_t _nIndex)
    {
        return m_afPoint[_nIndex];
    }

    inline const float& operator[](size_t _nIndex) const
    {
        return m_afPoint[_nIndex];
    }

    inline size_t GetBlockSide(const TPoint<nDimension>& _rkPoint) const
    {
        size_t nSide = 0;
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            nSide += (_rkPoint[nIndex] <= m_afPoint[nIndex]) ? (1 << nIndex) : 0;
        }
        return nSide;
    }

    inline TPoint<nDimension>& operator/(const float _fScale)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_afPoint [nIndex] /= _fScale;
        }
        return *this;
    }

    inline void normalize()
    {
        TPoint <nDimension>& rkThis = *this;
        float fDistance = GetDistance();
        rkThis = rkThis / fDistance;
    }

    inline TPoint<nDimension>& operator=(const TPoint<nDimension>& _rkPoint)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_afPoint [nIndex] = _rkPoint [nIndex];
        }
        return *this;
    }

    inline float GetDistance() const
    {
        TPoint <nDimension> kThis = *this;
        float fDistance = kThis * kThis;
        fDistance = std::sqrt(fDistance);
        return fDistance;
    }

    inline float GetDistance(const TPoint <nDimension>& _rkPoint) const
    {
        TPoint <nDimension>& rkThis = *this;
        float fDistance = 0.0f;
        // Vector
        TPoint <nDimension> kVector = _rkPoint - rkThis;
        // dot
        fDistance = kVector * kVector;
        // sqrt
        fDistance = std::sqrt(fDistance);
        return fDistance;
    }

    inline bool IsSameDir(const TPoint<nDimension>& _rkVecter) const
    {
        TPoint <nDimension> kSelfVector = *this;
        TPoint <nDimension> kOtherVector = _rkVecter;
        kSelfVector.normalize();
        kOtherVector.normalize();
        return (kSelfVector == kOtherVector);
    }
};

template <size_t nDimension>
TPoint<nDimension> operator+(const TPoint<nDimension>& _rkPointLeft, const TPoint<nDimension>& _rkPointRight)
{
    TPoint<nDimension> kReturn;
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        kReturn [nIndex] = _rkPointLeft[nIndex] + _rkPointRight[nIndex];
    }
    return kReturn;
}

template <size_t nDimension>
TPoint<nDimension> operator-(const TPoint<nDimension>& _rkPointLeft, const TPoint<nDimension>& _rkPointRight)
{
    TPoint<nDimension> kReturn;
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        kReturn[nIndex] = _rkPointLeft [nIndex] - _rkPointRight [nIndex];
    }
    return kReturn;
}

template <size_t nDimension>
float operator*(const TPoint<nDimension>& _rkPoint1, const TPoint<nDimension>& _rkPoint2)
{
    float fDot = 0.0f;
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        fDot += _rkPoint1 [nIndex] * _rkPoint2 [nIndex];
    }
    return fDot;
}

template <size_t nDimension>
bool operator==(const TPoint<nDimension>& _rkPoint1, const TPoint<nDimension>& _rkPoint2)
{
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        if (_rkPoint1 [nIndex] != _rkPoint2 [nIndex])
            return false;
    }
    return true;
}

template <size_t nDimension>
bool operator!=(const TPoint<nDimension>& _rkPoint1, const TPoint<nDimension>& _rkPoint2)
{
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        if (_rkPoint1 [nIndex] != _rkPoint2 [nIndex])
            return true;
    }
    return false;
}

template <size_t nDimension>
bool operator<(const TPoint<nDimension>& _rkPoint1, const TPoint<nDimension>& _rkPoint2)
{
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        if (_rkPoint1 [nIndex] < _rkPoint2 [nIndex])
            return true;
    }
    return false;
}

template <size_t nDimension>
bool operator>(const TPoint<nDimension>& _rkPoint1, const TPoint<nDimension>& _rkPoint2)
{
    for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
        if (_rkPoint1 [nIndex] > _rkPoint2 [nIndex])
            return true;
    }
    return false;
}

template <size_t nDimension>
class TBinTreeNode
{
public:
    static const size_t ms_nMaxBlock = 1 << nDimension;
private:
    TPoint <nDimension> m_kCenter;
    TPoint <nDimension> m_kLength;
    std::shared_ptr <TBinTreeNode<nDimension> > m_aspkChildBlock[ms_nMaxBlock];
    TBinTreeNode<nDimension>* m_pkParent;
public:

    TBinTreeNode()
        :m_pkParent(nullptr)
    {
    }

    TBinTreeNode(const TBinTreeNode& _rkNode)
        :m_pkParent(nullptr)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_kCenter[nIndex] = _rkNode.m_kCenter[nIndex];
            m_kLength[nIndex] = _rkNode.m_kLength[nIndex];
        }
    }

    TBinTreeNode(const TPoint<nDimension>& _rkCenter, float _nLength)
        :m_pkParent(nullptr)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_kCenter[nIndex] = _rkCenter[nIndex];
            m_kLength[nIndex] = _nLength;
        }
    }

    TBinTreeNode(const TPoint<nDimension>& _rkCenter, const TPoint<nDimension>& _rkLength)
        :m_pkParent(nullptr)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_kCenter[nIndex] = _rkCenter[nIndex];
            m_kLength[nIndex] = _rkLength[nIndex];
        }
    }

    virtual ~TBinTreeNode()
    {

    }

    void SetChild(size_t _nBlock, std::shared_ptr<TBinTreeNode <nDimension> > _spkChild)
    {
        if (_nBlock >= ms_nMaxBlock)
            return;
        m_aspkChildBlock[_nBlock] = _spkChild;
        _spkChild->m_pkParent = this;
    }

    std::shared_ptr <TBinTreeNode<nDimension> > GetChild(size_t _nBlock)
    {
        if (_nBlock >= ms_nMaxBlock)
            return nullptr;
        return m_aspkChildBlock[_nBlock];
    }

    TBinTreeNode <nDimension>* GetParent() const
    {
        return m_pkParent;
    }

    const TPoint<nDimension>& GetBlockCenter() const
    {
        return m_kCenter;
    }

    void SetBlockCenter(const TPoint<nDimension>& _rkPoint)
    {
        m_kCenter = _rkPoint;
    }

    void SetLength(const TPoint<nDimension>& _rkLength)
    {
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            m_kLength[nIndex] = _rkLength[nIndex];
        }
    }

    const TPoint<nDimension>& GetLength() const
    {
        return m_kLength;
    }
    
    virtual bool IsEnd() const
    {
        return false;
    }
};

template <typename TData, size_t nDimension>
class TBinTreeEndNode : public TBinTreeNode <nDimension>
{
    std::set <TData> m_kData;
public:
    TBinTreeEndNode()
    {

    }

    TBinTreeEndNode(const TPoint<nDimension>& _rkCenter, float _nLength)
        :TBinTreeNode(_rkCenter, _nLength)
    {
    }

    TBinTreeEndNode (const TPoint<nDimension>& _rkCenter, const TPoint<nDimension>& _rkLength)
        :TBinTreeNode(_rkCenter, _rkLength)
    {
    }

    virtual ~TBinTreeEndNode()
    {

    }

    bool SetData(const TData& _rkData)
    {
        auto kIt = m_kData.find(_rkData);
        if (kIt != m_kData.end())
            return false;

        m_kData.insert(_rkData);
        return true;
    }

    const std::set <TData>& GetDatas() const
    {
        return m_kData;
    }

    bool IsEnd() const
    {
        return true;
    }
};

template <typename TData, size_t nDimension>
class TBinTree
{
    typedef void(*FScopeCallback)(TBinTree <TData, nDimension>&);
    static const size_t ms_nMaxBlock = 1 << nDimension;
    std::shared_ptr <TBinTreeNode<nDimension> > m_spkRoot;
    short m_nCurremtDepth;
    const short m_nDepth;

    static void EnterDepth(TBinTree <TData, nDimension>& _rkData)
    {
        _rkData.m_nCurremtDepth++;
    }

    static void LeaveDepth(TBinTree <TData, nDimension>& _rkData)
    {
        _rkData.m_nCurremtDepth--;
    }

    template <typename T>
    std::shared_ptr <T> NewSubBlock(std::shared_ptr<TBinTreeNode<nDimension> > spkNode, const TPoint<nDimension>& _rkInsertPoint, size_t nBlock)
    {
        const TPoint <nDimension>& rkRootCenter = spkNode->GetBlockCenter();
        TPoint<nDimension> kCenter = rkRootCenter;
        const TPoint <nDimension>& rkLength = spkNode->GetLength();
        TPoint <nDimension> kLength;
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            kLength[nIndex] = rkLength[nIndex] / 2.0f;
            float fT = float(1 - 2 * ((nBlock & (1 << nIndex)) != 0));
            kCenter[nIndex] += fT * kLength[nIndex] / 2;
        }
#if 1
        TPoint <nDimension> kSubMin = kCenter - kLength;
        TPoint <nDimension> kSubMax = kCenter + kLength;
        size_t nSubBlockSide = kCenter.GetBlockSide(_rkInsertPoint);
        if (nSubBlockSide == nBlock) {
            if (kSubMax < _rkInsertPoint)
                return nullptr;
            if (kSubMin > _rkInsertPoint)
                return nullptr;
        }
#endif
        return std::make_shared <T> (kCenter, kLength);
    }

    std::shared_ptr <TBinTreeNode <nDimension> > UpdateEndBlock(std::shared_ptr<TBinTreeNode <nDimension> > spkNode, const TPoint <nDimension>& _rkPoint)
    {
        const TPoint <nDimension>& rkRootCenter = spkNode->GetBlockCenter();
        const TPoint <nDimension>& rkLength = spkNode->GetLength();
        TPoint<nDimension> kCenter = rkRootCenter;
        size_t nBlock = kCenter.GetBlockSide(_rkPoint);
        TPoint<nDimension> kLength;
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            kLength[nIndex] = rkLength[nIndex] / 2.0f;
            float fT = 1 - 2 * ((nBlock & (1 << nIndex)) != 0);
            kCenter [nIndex] += fT * kLength [nIndex] / 2;
        }
#if 1
        TPoint <nDimension> kSubMin = kCenter - kLength;
        TPoint <nDimension> kSubMax = kCenter + kLength;
        size_t nSubBlockSide = kCenter.GetBlockSide(_rkPoint);
        if (nSubBlockSide == nBlock) {
            if (kSubMax < _rkPoint)
                return spkNode;
            if (kSubMin > _rkPoint)
                return spkNode;
        }
#endif
        std::shared_ptr<TBinTreeNode <nDimension> > spkSubNode = std::make_shared<TBinTreeNode <nDimension> >(rkRootCenter, rkLength);
        spkNode->SetBlockCenter(kCenter);
        spkNode->SetLength(kLength);
        TBinTreeNode <nDimension>* pkParent = spkNode->GetParent();
        spkSubNode->SetChild(nBlock, spkNode);
        LogNode (nBlock, *spkSubNode);
        nBlock = pkParent->GetBlockCenter().GetBlockSide(spkSubNode->GetBlockCenter());
        pkParent->SetChild(nBlock, spkSubNode);
        LogNode (nBlock, *spkNode);
        return spkSubNode;
    }

    bool Insert(const TData& _rkData, const TPoint<nDimension>& _rkPoint, std::shared_ptr<TBinTreeNode<nDimension> > spkNode)
    {
        TScopeHelperByFunctor<TBinTree <TData, nDimension>, FScopeCallback, FScopeCallback> kScopeHelper(*this, &TBinTree <TData, nDimension>::EnterDepth, &TBinTree <TData, nDimension>::LeaveDepth);
        bool bRet = false;
        const TPoint<nDimension>& rkCenter = spkNode->GetBlockCenter();
        size_t nSubBlock = rkCenter.GetBlockSide(_rkPoint);
        std::shared_ptr<TBinTreeNode <nDimension> > spkSubNode = spkNode->GetChild(nSubBlock);
        if (!spkSubNode) {
            if (!spkNode->IsEnd()) {
                std::shared_ptr<TBinTreeEndNode <TData, nDimension> > spkEndNode = NewSubBlock<TBinTreeEndNode <TData, nDimension> >(spkNode, _rkPoint, nSubBlock);
                if (!spkEndNode)    // point out of block
                    return true;
                bRet = spkEndNode->SetData(_rkData);
                spkNode->SetChild(nSubBlock, spkEndNode);
                LogNode(nSubBlock, *spkEndNode);
                return bRet;
            }
            UpdateEndBlock(spkNode, _rkPoint);
            TBinTreeEndNode <TData, nDimension>* pkEndNode = dynamic_cast <TBinTreeEndNode <TData, nDimension>*>(spkNode.get());
            pkEndNode->SetData(_rkData);
            return true;
        }

        if (m_nCurremtDepth >= m_nDepth) {
            TBinTreeEndNode <TData, nDimension>* pkEndNode = dynamic_cast <TBinTreeEndNode <TData, nDimension>*>(spkSubNode.get());
            return pkEndNode->SetData(_rkData);
        }

        return Insert(_rkData, _rkPoint, spkSubNode);
    }

    void InsertBoundBox(const TData& _rkData, std::shared_ptr <TBinTreeNode<nDimension> > spkNode, TPoint <nDimension> _akBoundBoxPoint [ms_nMaxBlock])
    {
        TScopeHelperByFunctor<TBinTree <TData, nDimension>, FScopeCallback, FScopeCallback> kScopeHelper(*this, &TBinTree <TData, nDimension>::EnterDepth, &TBinTree <TData, nDimension>::LeaveDepth);
        size_t nInsertedBlock = -1;
        for (unsigned int nIndex = 0; nIndex < ms_nMaxBlock; nIndex++) {
            const TPoint <nDimension>& rkRootCenter = spkNode->GetBlockCenter();
            size_t nSubBlock = rkRootCenter.GetBlockSide(_akBoundBoxPoint [nIndex]);
            std::shared_ptr<TBinTreeNode <nDimension> > spkSubNode = spkNode->GetChild(nSubBlock);
            if (!spkSubNode) {
                if (!spkNode->IsEnd()) {
                    std::shared_ptr<TBinTreeEndNode <TData, nDimension> > spkEndNode = NewSubBlock<TBinTreeEndNode <TData, nDimension> >(spkNode, _akBoundBoxPoint[nIndex], nSubBlock);
                    if (!spkEndNode) {
                        continue;
                    }
                    spkEndNode->SetData(_rkData);
                    spkNode->SetChild(nSubBlock, spkEndNode);
                    LogNode(nSubBlock, *spkEndNode);
                    continue;
                }
                if (nInsertedBlock != nSubBlock) {
                    TBinTreeEndNode <TData, nDimension>* pkEndNode = dynamic_cast <TBinTreeEndNode <TData, nDimension>*>(spkNode.get());
                    pkEndNode->SetData(_rkData);
                }
                if (m_nCurremtDepth >= m_nDepth) {
                    continue;
                }
                spkNode = UpdateEndBlock(spkNode, _akBoundBoxPoint [nIndex]);
                continue;
            }

            if (m_nCurremtDepth >= m_nDepth) {
                if (nInsertedBlock != nSubBlock) {
                    TBinTreeEndNode <TData, nDimension>* pkEndNode = dynamic_cast <TBinTreeEndNode <TData, nDimension>*>(spkSubNode.get());
                    pkEndNode->SetData(_rkData);
                }
            }

            InsertBoundBox(_rkData, spkSubNode, _akBoundBoxPoint);
        }
    }

    void LogNode(size_t nBlock, TBinTreeNode <nDimension>& rkNode)
    {
        bool bEnd = rkNode.IsEnd();
        const TPoint<nDimension>& rkCenter = rkNode.GetBlockCenter();
        const const TPoint <nDimension>& rkLength = rkNode.GetLength();
        std::string kData = "Center ";
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            kData += "D";
            kData += std::to_string(nIndex);
            kData += ":";
            kData += std::to_string(rkCenter[nIndex]);
            if (nIndex < (nDimension - 1))
                kData += ", ";
        }
        kData += " Length ";
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            kData += "D";
            kData += std::to_string(nIndex);
            kData += ":";
            kData += std::to_string(rkLength[nIndex]);
            if (nIndex < (nDimension - 1))
                kData += ", ";
        }
        ILogger::Log(eLL_Info, "Insert Depth:%d Block:%d IsEnd:%s %s\n", m_nCurremtDepth, nBlock, (bEnd) ? "true" : "false", kData.c_str());
    }

    template <typename Functor>
    void PreOrderTraveralRecusive(Functor& _fnFunctor, TBinTreeNode<nDimension>*  _pkNode)
    {
        if (!_fnFunctor(*_pkNode))
            return;
        for (size_t nIndex = 0; nIndex < ms_nMaxBlock; nIndex++) {
            std::shared_ptr <TBinTreeNode <nDimension> > spkChild = _pkNode->GetChild(nIndex);
            if (!spkChild)
                continue;
            PreOrderTraveralRecusive(_fnFunctor, spkChild.get());
        }
    }
public:
    TBinTree(short _nDepth)
        : m_nDepth(_nDepth)
        , m_nCurremtDepth(0)
    {
        m_spkRoot = std::make_shared <TBinTreeNode <nDimension> >(TPoint<nDimension>(), 50000.f);
    }

    TBinTree(const TPoint<nDimension>& _rkCenter, short _nDepth)
        : m_nDepth(_nDepth)
        , m_nCurremtDepth(0)
    {
        m_spkRoot = std::make_shared <TBinTreeNode <nDimension> >(_rkCenter, 50000.f);
    }

    TBinTree(const TPoint<nDimension>& _rkCenter, float _nLength, short _nDepth)
        : m_nDepth(_nDepth)
        , m_nCurremtDepth(0)
    {
        m_spkRoot = std::make_shared <TBinTreeNode <nDimension> >(_rkCenter, _nLength);
    }

    TBinTree(const TPoint<nDimension>& _rkCenter, float _nLength[nDimension], short _nDepth)
        : m_nDepth(_nDepth)
        , m_nCurremtDepth(0)
    {
        m_spkRoot = std::make_shared <TBinTreeNode <nDimension> >(_rkCenter, _nLength);
    }

    virtual ~TBinTree()
    {}

    bool InsertPoint(const TData& _rkData)
    {
        TPoint <nDimension> kPoint;
        for (size_t nIndex = 0; nIndex < nDimension; nIndex++) {
            kPoint [nIndex] = _rkData->getDimension(nIndex);
        }
        m_nCurremtDepth = 0;
        return Insert(_rkData, kPoint, m_spkRoot);
    }

    void InsertBoundBox(const TData& _rkData)
    {
        TPoint <nDimension> kCenter = _rkData->getCenter();
        TPoint <nDimension> kHalf = _rkData->getHalf();
        TPoint <nDimension> akBoundBoxPoint [ms_nMaxBlock];
        for (unsigned int nIndex = 0; nIndex < ms_nMaxBlock; nIndex++) {
            for (unsigned int nDim = 0; nDim < nDimension; nDim++) {
                akBoundBoxPoint [nIndex] [nDim] = kCenter [nDim] + ((nIndex & (1 << nDim)) ? kHalf [nDim] : -kHalf [nDim]);
            }
        }
        InsertBoundBox(_rkData, m_spkRoot, akBoundBoxPoint);
    }

    template <typename Functor>
    void PreOrderTraveral(Functor& _fnFunctor, bool _bNonrecusive = true)
    {
        if (_bNonrecusive) {
            struct TStackData : public std::pair <size_t, std::shared_ptr <TBinTreeNode <nDimension> > >
            {
                char m_nID;

                TStackData(size_t _nBlock, std::shared_ptr <TBinTreeNode <nDimension> > spkData)
                    :std::pair<size_t, std::shared_ptr <TBinTreeNode <nDimension> > >(_nBlock, spkData)
                {
                }

                static std::shared_ptr <TStackData> Create(size_t _nBlock, std::shared_ptr <TBinTreeNode <nDimension> > spkData)
                {
                    return std::make_shared <TStackData>(_nBlock, spkData);
                }

                static void SetData(std::shared_ptr <TStackData> spkStackData, size_t _nBlock, std::shared_ptr <TBinTreeNode <nDimension> > spkData)
                {
                    spkStackData->first = _nBlock;
                    spkStackData->second = spkData;
                }
            };
            // There will be multi thread issue
            static CRecyclePool <std::shared_ptr <TStackData>, char> kStackPairPool;
            static std::stack <char> kStack;
            std::shared_ptr <TBinTreeNode <nDimension> > spkCurrent = m_spkRoot;
            size_t nCurrentBlock = 0;
            bool bDone = false;
            while (!bDone) {
                if (spkCurrent) {
                    kStack.push(kStackPairPool.Allocate<TStackData>(nCurrentBlock, spkCurrent));
                    std::shared_ptr <TStackData> spkData = kStackPairPool.GetData(kStack.top());
                    spkData->m_nID = kStack.top();
                    bool bTest = true;
                    if (!nCurrentBlock)
                        bTest &= _fnFunctor(*spkCurrent);
                    if (!bTest) {
                        spkCurrent = nullptr;
                    }
                    else {
                        spkCurrent = spkCurrent->GetChild(nCurrentBlock);
                        if (spkCurrent)
                            nCurrentBlock = 0;
                    }
                }
                else {
                    if (!kStack.empty()) {
                        std::shared_ptr <TStackData> spkData = kStackPairPool.GetData(kStack.top());
                        nCurrentBlock = spkData->first + 1;
                        spkCurrent = (nCurrentBlock < ms_nMaxBlock) ? spkData->second : nullptr;
                        kStackPairPool.Recycle(spkData->m_nID);
                        kStack.pop();
                    }
                    else {
                        bDone = true;
                    }
                }
            }
            return;
        }
        PreOrderTraveralRecusive(_fnFunctor, m_spkRoot.get());
    }
};