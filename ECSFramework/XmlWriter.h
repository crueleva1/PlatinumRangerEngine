#pragma once



#include "rapidxml_print.hpp"
#include <stack>

class ECS_API CXmlWriter
{
    rapidxml::xml_document<> m_kDocument;
    std::stack <rapidxml::xml_node<>*> m_kElementStack;
    rapidxml::xml_node<>* m_pkCurrent;
    std::string m_kFile;
public:
    CXmlWriter(const char* _pkFile);

    virtual ~CXmlWriter();

    void PushElement(const char* _pcElement, rapidxml::node_type type = rapidxml::node_element);

    void PopElement();

    void AddAttr(const char* _pkKey, const char* _pkValue);

    void GetStream(std::ostream& _rkOut);
};

class ECS_API CXmlWriterChainer
{
    CXmlWriter& m_rkWriter;
    int m_nPushCount;
public:
    CXmlWriterChainer(CXmlWriter& _rkWriter, const char* _pkElement, rapidxml::node_type type = rapidxml::node_element)
        :m_rkWriter(_rkWriter)
        ,m_nPushCount(1)
    {
        m_rkWriter.PushElement(_pkElement, type);
    }

    virtual ~CXmlWriterChainer()
    {
        while (m_nPushCount > 0)
        {
            m_rkWriter.PopElement();
            m_nPushCount--;
        }
    }


    CXmlWriterChainer& AddAttr(const char* _pkKey, const char* _pkValue)
    {
        m_rkWriter.AddAttr(_pkKey, _pkValue);
        return *this;
    }

    CXmlWriterChainer& PushElement(const char* _pkElement, rapidxml::node_type type = rapidxml::node_element)
    {
        m_rkWriter.PushElement(_pkElement, type);
        m_nPushCount++;
        return *this;
    }

    CXmlWriterChainer& PopElement()
    {
        m_rkWriter.PopElement();
        m_nPushCount--;
        return *this;
    }
};