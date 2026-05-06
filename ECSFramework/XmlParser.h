#pragma once

#include "rapidxml.hpp"
#include <map>
#include <string>



class ECS_API IXMLHandler
{
public:

    virtual ~IXMLHandler()
    {
    }

    virtual const char* GetSchemaName() const = 0;

    virtual void OnElementStart(const std::string& _rkElement, const rapidxml::xml_attribute<>& _rkAttr) = 0;

    virtual void OnElementEnd(const std::string& _rkElement) = 0;

    virtual void OnText(const std::string& _rkText) = 0;
};

class ECS_API CRapidXMLDocument : public rapidxml::xml_document<>
{
    IXMLHandler& m_rkHandler;
public:
    CRapidXMLDocument(IXMLHandler& _rkHandler, const char* _pcSource, size_t _nSize);

    virtual ~CRapidXMLDocument()
    {
    }

protected:
    void ProcessElement(const rapidxml::xml_node<>* _rkElement);
};