//
// Created by joppich on 3/30/16.
//

#ifndef BIOGUI_EXECUTIONOUTPUTNODE_H
#define BIOGUI_EXECUTIONOUTPUTNODE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include "ExecutionNode.h"
#include "../../app/StreamTextEdit.h"

class ExecutionOutputNode : public ExecutionNode {

public:
    ExecutionOutputNode(QDomElement* pElement)
    : ExecutionNode(pElement)
    {

        m_sType = this->getDomElementAttribute(pElement, "TYPE", "STD").toUpper().toStdString();
        m_sColor = this->getDomElementAttribute(pElement, "COLOR", "black").toStdString();
        m_sTo = this->getDomElementAttribute(pElement, "TO", "").toStdString();
        m_sLocation = this->getDomElementAttribute(pElement, "location", "").toStdString();

        if (m_sTo.size() == 0)
        {
            throw "no attribute TO given for node " + m_sID;
        }


    }

    std::string evaluate( std::map< std::string, ExecutionNode*>* pID2Node,
                          std::map<std::string, std::string>* pInputID2Value,
                          std::map<std::string, QWidget*>* pInputID2Widget)
    {

        if (!(pInputID2Widget->find( m_sTo ) != pInputID2Widget->end()))
        {
            throw "invalid TO id given for node id " + m_sID;
        }

        QWidget* pWidget = pInputID2Widget->find( m_sTo )->second;

        if (StreamTextEdit* pTextEdit = dynamic_cast<StreamTextEdit*>( pWidget ))
        {

            QColor oColor = QColor(QString(m_sColor.c_str()));

            if (m_sType.compare("STD") == 0)
            {
                pTextEdit->addBuffer( std::cout, oColor );
                pTextEdit->addBuffer( std::cerr, oColor );
            }

            if (m_sType.compare("COUT") == 0)
            {
                pTextEdit->addBuffer( std::cout, oColor );
            }

            if (m_sType.compare("CERR") == 0)
            {
                pTextEdit->addBuffer( std::cerr, oColor );
            }


        }

        if (QGraphicsView* pImageView = dynamic_cast<QGraphicsView*>( pWidget ))
        {

            pImageView->scene()->clear();

            QGraphicsPixmapItem* pItem = new QGraphicsPixmapItem(QPixmap( QString(m_sLocation.c_str()) ));

            pImageView->scene()->addItem( pItem );

        }


    }

protected:

    std::string m_sType;
    std::string m_sTo;
    std::string m_sColor;
    std::string m_sLocation;


};


#endif //BIOGUI_EXECUTIONOUTPUTNODE_H
