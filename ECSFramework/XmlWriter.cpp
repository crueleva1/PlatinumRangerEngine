#include "ECSFrameworkPCH.h"
#include "XmlWriter.h"



CXmlWriter::CXmlWriter(const char* _pkFile)
    :m_pkCurrent(nullptr)
{

}

CXmlWriter::~CXmlWriter()
{

}

void CXmlWriter::PushElement(const char* _pcElement, rapidxml::node_type type)
{
    rapidxml::xml_node<>* pkNode = m_kDocument.allocate_node(type, _pcElement);
    if (pkNode) {
        if (m_pkCurrent) {
            m_kElementStack.push(m_pkCurrent);
        }
        m_pkCurrent = pkNode;
    }
}

void CXmlWriter::PopElement()
{
    if (!m_pkCurrent)
        return;
    
    if (m_kElementStack.empty()) {
        m_kDocument.append_node(m_pkCurrent);
        m_pkCurrent = nullptr;
    }
    else {
        m_kElementStack.top()->append_node(m_pkCurrent);
        m_pkCurrent = m_kElementStack.top();
        m_kElementStack.pop();
    }
}

void CXmlWriter::AddAttr(const char* _pkKey, const char* _pkValue)
{
    if (!m_pkCurrent)
        return;

    m_pkCurrent->append_attribute(m_kDocument.allocate_attribute(_pkKey, _pkValue));
}

void CXmlWriter::GetStream(std::ostream& _rkOut)
{
    _rkOut.clear();
    _rkOut << m_kDocument;
}

