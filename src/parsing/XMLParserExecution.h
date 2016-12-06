//
// Created by joppich on 3/30/16.
//

#ifndef BIOGUI_XMLPARSEREXECUTION_H
#define BIOGUI_XMLPARSEREXECUTION_H

#include <src/parsing/nodes/ExecutionFileNode.h>
#include <src/parsing/nodes/ExecutionStringReplaceNode.h>
#include <src/parsing/nodes/ExecutionPathRelocateNode.h>
#include <src/parsing/nodes/ExecutionHTTPExecuteNode.h>
#include <src/parsing/nodes/ExecutionScriptNode.h>
#include "nodes/ExecutionOrderedAddNode.h"
#include "nodes/ExecutionPlaceholderNode.h"
#include "nodes/ExecutionIfNode.h"
#include "nodes/ExecutionMathNode.h"
#include "nodes/ExecutionValueNode.h"
#include "nodes/ExecutionConstNode.h"
#include "nodes/ExecutionAddNode.h"
#include "nodes/ExecutionExecuteNode.h"
#include "nodes/ExecutionOutputNode.h"
#include "nodes/ExecutionEnvNode.h"
#include "nodes/ExecutionNode.h"
#include "nodes/ExecutionScriptNode.h"
#include "nodes/ExecutionNetwork.h"
#include "XMLParser.h"


#include <iostream>
#include <ctype.h>
#include <sstream>

class XMLParserExecution : public XMLParser {

    Q_OBJECT

public:

    XMLParserExecution()
    : XMLParser()
    {

        this->insertNodeType("EXECUTE", [] (QDomElement* pElement) {
            ExecutionExecuteNode* pNode = new ExecutionExecuteNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("HTTPEXECUTE", [] (QDomElement* pElement) {
            ExecutionHTTPExecuteNode* pNode = new ExecutionHTTPExecuteNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("FILE", [] (QDomElement* pElement) {
            ExecutionFileNode* pNode = new ExecutionFileNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("CONST", [] (QDomElement* pElement) {
            ExecutionConstNode* pNode = new ExecutionConstNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("VALUE", [] (QDomElement* pElement) {
            ExecutionValueNode* pNode = new ExecutionValueNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("ADD", [] (QDomElement* pElement) {
            ExecutionAddNode* pNode = new ExecutionAddNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("REPLACE", [] (QDomElement* pElement) {
            ExecutionStringReplaceNode* pNode = new ExecutionStringReplaceNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("ORDEREDADD", [] (QDomElement* pElement) {
            ExecutionOrderedAddNode* pNode = new ExecutionOrderedAddNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("MATH", [] (QDomElement* pElement) {
            ExecutionMathNode* pNode = new ExecutionMathNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("OUTPUT", [] (QDomElement* pElement) {
            ExecutionOutputNode* pNode = new ExecutionOutputNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("IF", [] (QDomElement* pElement) {
            ExecutionIfNode* pNode = new ExecutionIfNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("ELSE", [] (QDomElement* pElement) {
            ExecutionPlaceholderNode* pNode = new ExecutionPlaceholderNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("RELOCATE", [] (QDomElement* pElement) {
            ExecutionPathRelocateNode* pNode = new ExecutionPathRelocateNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("ENV", [] (QDomElement* pElement) {
            ExecutionEnvNode* pNode = new ExecutionEnvNode( pElement );
            return (ExecutionNode*) pNode;
        });

        this->insertNodeType("SCRIPT", [] (QDomElement* pElement) {
            ExecutionScriptNode* pNode = new ExecutionScriptNode( pElement );
            return (ExecutionNode*) pNode;
        });


        if (m_pKnownTags != NULL)
            delete m_pKnownTags;

        m_pKnownTags = this->getKnownTags();

    }

    void printAvailableNodes()
    {
        QDomDocument oDoc;

        for (size_t i = 0; i < m_pKnownTags->size(); ++i)
        {
            QDomElement oElement = oDoc.createElement( QString(m_pKnownTags->at(i).c_str()) );

            ExecutionNode* pNode = NULL;

            pNode = this->createExecutionNode(&oElement);

            if (pNode == NULL)
            {
                LOGLVL("Failed to create Execution Node for: " + oElement.tagName().toStdString() + " in: " + m_sCurrentDocument, Logging::ERROR);
            }

            this->handleAttributeNode(m_pKnownTags->at(i), pNode);

        }
    }

    void handleAttributeNode(std::string& sTag, ExecutionNode* pNode)
    {
        if (pNode == NULL)
            return;

        std::vector<std::string> vAttributes = pNode->getAcceptedAttributes();

        std::stringstream oSS;
        for (size_t j = 0; j < vAttributes.size(); ++j)
            oSS << ", " << vAttributes[j];

        std::string sJoined = oSS.str();

        if (sJoined.size() > 2)
            sJoined = sJoined.substr(2, -1);

        std::cout << "<" << sTag << ">" << "\t[" << sJoined << "]" << std::endl;

        if (pNode != NULL)
            delete pNode;
    }

    ~XMLParserExecution()
    {

        for (size_t i = 0; i < m_vCreatedNodes.size(); ++i)
        {
            delete m_vCreatedNodes.at(i);
        }

    }

    ExecutionNetwork * getExecutionNetwork();

protected:

    void insertNodeType(std::string sType, std::function< ExecutionNode* ( QDomElement* )> oFunc)
    {
        std::pair<std::string, std::function< ExecutionNode*( QDomElement*)> > oPair;

        oPair = std::pair<std::string, std::function< ExecutionNode*( QDomElement*)>>( sType, oFunc);
        m_mCreateNodeMap.insert(oPair);

    }


    std::vector<std::string>* getKnownTags()
    {
        std::vector<std::string>* pTags = new std::vector<std::string>();

        std::map<std::string, std::function< ExecutionNode*( QDomElement*)> >::iterator oIt = m_mCreateNodeMap.begin();

        while (oIt != m_mCreateNodeMap.end())
        {
            std::string sTag = oIt->first;

            for (size_t i = 0; i < sTag.size(); ++i)
                sTag[i] = std::toupper((char) sTag[i]);

            pTags->push_back( sTag );
            ++oIt;
        }

        // manually add master node!
        pTags->push_back("EXECUTION");

        return pTags;
    }

    ExecutionNode* createNode(QDomElement* pElement, std::string sNodeType)
    {

        for (size_t i = 0; i < sNodeType.size(); ++i)
            sNodeType[i] = std::toupper(sNodeType[i]);

        std::map<std::string, std::function< ExecutionNode*( QDomElement*)> >::iterator oIt = m_mCreateNodeMap.find( sNodeType );

        if (oIt != m_mCreateNodeMap.end())
        {
            return oIt->second( pElement );
        } else {
            return NULL;
        }


    }

    ExecutionNode* createExecutionNode(QDomElement* pElement)
    {


        QString sTagName = pElement->tagName();

        ExecutionNode* pReturn = this->createNode(pElement, sTagName.toStdString());

        if (pReturn == NULL)
        {

            std::cout << sTagName.toStdString() << std::endl;
            std::cout << sTagName.toStdString() << std::endl;

        }


        return pReturn;
    }


    QDomElement * getRoot(QDomDocument* pDocument);
    ExecutionNetwork * createNetwork(QDomElement* pElement);
    ExecutionNode * getExecutionNodes(QDomElement* pElement );

    std::map< std::string, std::function< ExecutionNode*( QDomElement*)> > m_mCreateNodeMap;
    std::vector<ExecutionNode*> m_vCreatedNodes;

};


#endif //BIOGUI_XMLPARSEREXECUTION_H
