#include "ECSFrameworkPCH.h"
#include "XmlParser.h"


CRapidXMLDocument::CRapidXMLDocument(IXMLHandler& _rkHandler, const char* _pcSource, size_t _nSize)
    : m_rkHandler(_rkHandler)
{
    char* pcBuffer = new char[_nSize + 2]();
    memcpy(pcBuffer, _pcSource, _nSize);

    pcBuffer [_nSize] = '\n';
    pcBuffer [_nSize + 1] = '\0';

    rapidxml::xml_document <> kDoc;
    kDoc.parse<0>(pcBuffer);

    rapidxml::xml_node <>* pkCurrElement = kDoc.first_node();
    if (pkCurrElement) {
        ProcessElement(pkCurrElement);
    }

    delete [] pcBuffer;
}

void CRapidXMLDocument::ProcessElement(const rapidxml::xml_node<>* _pkElement)
{
    rapidxml::xml_attribute<>* pkCurrAttr = _pkElement->first_attribute(0);

    m_rkHandler.OnElementStart(_pkElement->name(), *pkCurrAttr);

    rapidxml::xml_node<>* pkChildNode = _pkElement->first_node();

    while (pkChildNode) {
        switch (pkChildNode->type())
        {
            case rapidxml::node_element:
                ProcessElement(pkChildNode);
                break;

            case rapidxml::node_data:
                if (pkChildNode->value() != '\0')
                    m_rkHandler.OnText(pkChildNode->value());

                break;
        };

        pkChildNode = pkChildNode->next_sibling();
    }

    // end element
    m_rkHandler.OnElementEnd(_pkElement->name());
}