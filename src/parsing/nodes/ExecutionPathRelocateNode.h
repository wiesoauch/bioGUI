//
// Created by joppich on 4/25/16.
//

#ifndef BIOGUI_EXECUTIONWSLTRANSFORMNODE_H
#define BIOGUI_EXECUTIONWSLTRANSFORMNODE_H


#include <QFile>
#include <QTextStream>
#include <QtCore/qdir.h>
#include "ExecutionNode.h"
#include "ExecutionValueNode.h"

class ExecutionPathRelocateNode : public ExecutionNode {

public:

    ExecutionPathRelocateNode(QDomElement* pElement)
            : ExecutionNode(pElement)
    {

        m_sFrom = this->getDomElementAttribute(pElement, "from", "").toStdString();
        m_sTo = this->getDomElementAttribute(pElement, "to", "").toStdString();
        m_bMakeUnix = (this->getDomElementAttribute(pElement, "unix", "false").compare("TRUE", Qt::CaseInsensitive) == 0);
        m_bToWSL = (this->getDomElementAttribute(pElement, "wsl", "false").compare("TRUE", Qt::CaseInsensitive) == 0);

        if (m_bToWSL)
            m_bMakeUnix = true;



    }

    std::string evaluate( std::map< std::string, ExecutionNode*>* pID2Node,
                          std::map<std::string, std::string>* pInputID2Value,
                          std::map<std::string, QWidget*>* pInputID2Widget)
    {

        std::string sChildren = "";

        if (m_sFrom.size() == 0)
        {

            sChildren = this->evaluateChildren(pID2Node, pInputID2Value, pInputID2Widget);

        } else {

            sChildren = ExecutionValueNode::getFromID(m_sFrom, pID2Node, pInputID2Value, pInputID2Widget);

        }

        QString qsChildren(sChildren.c_str());

        // replace from with to
        if (qsChildren.startsWith(QString(m_sFrom.c_str()), Qt::CaseInsensitive))
        {

            qsChildren.remove(0, m_sFrom.size());
            qsChildren.prepend(QString(m_sTo.c_str()));

        }

        // absolute path
        if (qsChildren.at(1) != ':')
        {
            QDir oPath(qsChildren);
            qsChildren = oPath.absolutePath();
        }

        if (m_bMakeUnix)
        {
            // assumes that we have a windows string
            QChar cDrive = qsChildren.at(0);
            qsChildren.remove(0,2);
            qsChildren.prepend(QString("/") + cDrive.toLower());

            qsChildren.replace("\\", "/");
        }

        if (m_bToWSL)
        {
            qsChildren.prepend("/mnt/");
        }


        return qsChildren.toStdString();

    }

protected:

    std::string m_sFrom;
    std::string m_sTo;

    bool m_bMakeUnix = true;
    bool m_bToWSL = true;


};


#endif //BIOGUI_EXECUTIONWSLTRANSFORMNODE_H