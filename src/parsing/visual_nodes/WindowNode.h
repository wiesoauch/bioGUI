//
// Created by joppich on 10/21/16.
//

#ifndef BIOGUI_WINDOWNODE_H
#define BIOGUI_WINDOWNODE_H

#include <exception>
#include <string>
#include <QDomElement>
#include <functional>

class WindowNodeException : public std::exception
{
public:

    WindowNodeException(std::string sMessage)
            : std::exception(), m_sMessage(sMessage)
    {

    }

    char const* what() const throw()
    {
        return m_sMessage.c_str();
    }

protected:

    const std::string m_sMessage;

};

struct Retriever {

    std::string sElementID;
    std::function<std::string()> oRetriever;

};

class WindowComponentFactory;

template <class T>
class WindowNode {
public:

    WindowNode(WindowComponentFactory* pFactory)
    : m_pFactory(pFactory)
    {
    }

    virtual ~WindowNode()
    {

    }

    struct CreatedElement {
        T* pElement = NULL;
        std::vector< Retriever > vRetriever;
        bool hasRetriever()
        {
            return vRetriever.size() > 0;
        }

        void addRetriever(std::string sID,std::function<std::string()> oFunc )
        {
            vRetriever.push_back( {sID, oFunc} );
        }

        bool bHasChildrenFinished = false;
    };

    std::vector<std::string> getAcceptedAttributes()
    {
        std::vector<std::string> vReturnAttribs;

        this->addNodeAttributes(vReturnAttribs);

        std::sort(vReturnAttribs.begin(), vReturnAttribs.end());

        return vReturnAttribs;
    }

    virtual typename WindowNode<T>::CreatedElement getWindowElement( QDomElement* pDOMElement ) = 0;

protected:

    virtual void addNodeAttributes(std::vector<std::string>& vAttributes) = 0;

    bool hasAttribute(QDomElement* pElement, QString sAttribName)
    {
        if (pElement == NULL)
            return false;

        QDomNamedNodeMap mAttributes = pElement->attributes();

        for (size_t i = 0; i < mAttributes.length(); ++i)
        {

            QDomAttr oItem = mAttributes.item(i).toAttr();

            if (oItem.name().compare( sAttribName, Qt::CaseInsensitive ) == 0)
            {
                return true;
            }

        }

        return false;
    }

    QString getQAttribute(QDomElement* pElement, QString sAttribName, QString sDefault)
    {

        if (pElement == NULL)
            return sDefault;

        QDomNamedNodeMap mAttributes = pElement->attributes();

        for (size_t i = 0; i < mAttributes.length(); ++i)
        {

            QDomAttr oItem = mAttributes.item(i).toAttr();

            if (oItem.name().compare( sAttribName, Qt::CaseInsensitive ) == 0)
            {
                return oItem.value();
            }

        }

        return sDefault;

    }

    std::string getAttribute(QDomElement* pElement, QString sAttribName, QString sDefault)
    {

        QString sQAttrib = this->getQAttribute(pElement, sAttribName, sDefault);

        return sQAttrib.toStdString();
    }

    std::string getDomID(QDomElement* pDOMElement)
    {
        return this->getAttribute(pDOMElement, "id", "");
    }

    WindowComponentFactory* m_pFactory = NULL;

};

#endif //BIOGUI_WINDOWNODE_H
