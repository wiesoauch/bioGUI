//
// Created by joppich on 10/21/16.
//

#ifndef BIOGUI_WINDOWWIDGETGENERALGROUP_H
#define BIOGUI_WINDOWWIDGETGENERALGROUP_H

#include <src/app/QExclusiveGroupBox.h>
#include <iostream>
#include "WindowWidgetNode.h"

class WindowWidgetGeneralGroupNode : public WindowWidgetNode {

public:

    WindowWidgetGeneralGroupNode()
    : WindowWidgetNode()
    {

    }

protected:

    QWidget* createGeneralGroup(QDomElement* pElement,
                                bool* pChildrenFinished,
                                std::function<QLayout* (QDomElement*, QDomNodeList*)> oLayoutFunc,
                                std::function<QList<QWidget*> (QDomElement*, bool*, int)> oPreProcFunc = [] (QDomElement* pChildNode, bool* pBoolean, int iElement) {return QList<QWidget*>();},
                                std::function<void (QExclusiveGroupBox*, QLayout*, QWidget*, QWidget*, QStringList*, int)> oPostProcFunc = [] (QExclusiveGroupBox* pBox, QLayout* pLay, QWidget* pWid1, QWidget* pWid2, QStringList* pList, int iElement) {})
    {
        (*pChildrenFinished) = true;

        QExclusiveGroupBox* pGroupBox = new QExclusiveGroupBox("");

        /*
         * Window Title
         */
        QString sTitle = this->getAttribute(pElement, "title", "");
        pGroupBox->setTitle( sTitle );

        /*
         * checkable?
         */
        std::string sCheckable = this->getAttribute(pElement, "checkable", "false").toUpper().toStdString();
        if (sCheckable.compare("TRUE") == 0)
        {
            pGroupBox->setCheckable(true);
            pGroupBox->setChecked(false);

            std::string sCheckedValue = this->getAttribute(pElement, "checked_value", "true").toStdString();
            std::string sUncheckedValue = this->getAttribute(pElement, "unchecked_value", "false").toStdString();

            this->addValueFetcher(pElement, [pGroupBox, sCheckedValue, sUncheckedValue] () {

                if (pGroupBox->isChecked())
                {
                    return sCheckedValue;
                } else {
                    return sUncheckedValue;
                }

            });

        }

        /*
         * exclusive?
         */
        QString sExclusive = this->getAttribute(pElement, "exclusive", "false");
        if (sExclusive.compare("TRUE", Qt::CaseInsensitive) == 0)
        {
            pGroupBox->setExclusive(true);
        } else {
            pGroupBox->setExclusive(false);
        }

        /*
         * ordered
         *
         */

        bool bOrdered = (this->getAttribute(pElement, "ordered", "false").compare("true", Qt::CaseInsensitive) == 0);
        pGroupBox->setOrdered(bOrdered);

        /*
         * state?
         *
         * selected = true/false may indicate that group is selected.
         *
         * selected = [ids] may indicate that group is selected and children are selected
         *
         */
        QString sSelected = this->getAttribute(pElement, "selected", "");
        QStringList vSelected;

        if (pGroupBox->isCheckable())
        {
            if (sSelected.compare("TRUE", Qt::CaseInsensitive) == 0)
            {
                pGroupBox->setChecked(true);
            } else if (sSelected.compare("FALSE", Qt::CaseInsensitive) == 0)
            {
                pGroupBox->setChecked(false);
            }

        } else {

            pGroupBox->setChecked(true);

            if (sSelected.length() > 0)
            {
                vSelected = sSelected.split(";");
            }

        }

        /*
         * LAYOUT
         */

        QDomNodeList oChildren = pElement->childNodes();
        QLayout* pLayout = oLayoutFunc(pElement, &oChildren);

        /*
         *
         * CREATE AND ADD CHILDREN
         *
         */

        bool bBoolean = false;
        int iAdded = 0;

        for (size_t i = 0; i < oChildren.size(); ++i)
        {
            bBoolean = false;

            QDomElement oChildNode = oChildren.at(i).toElement();
            QList<QWidget*> vChildren = oPreProcFunc(&oChildNode, &bBoolean, iAdded);

            if (vChildren.size() == 0)
            {
                QWidget* pChildElement = this->createComponent(&oChildNode, &bBoolean);

                if (pChildElement != NULL)
                    vChildren.append(pChildElement);
            }


            if (vChildren.size() == 0)
            {
                std::cout << "error in creating groupbox components: " + pElement->text().toStdString() << std::endl;
                std::cout << "error in creating groupbox components: " + oChildNode.text().toStdString() << std::endl;
                //throw "error in creating groupbox components: " + oChildNode.text().toStdString();

                continue;
            }

            for (size_t i = 0; i < vChildren.size(); ++i)
            {

                QWidget* pChildElement = vChildren.at(i);

                // TODO this is not going to work with ordered radiobuttons ...
                QWidget* pTransformedChildElement = pGroupBox->addNextWidget(pChildElement);
                this->addToLayout(pLayout, pTransformedChildElement);
                this->setID(pTransformedChildElement, &oChildNode, true);

                oPostProcFunc(pGroupBox, pLayout, pChildElement, pTransformedChildElement, &vSelected, iAdded);
            }


            ++iAdded;

        }

        // Finally set Layout
        QWidget* pComponentWidget = new QWidget();

        pComponentWidget->setLayout(pLayout);

        QLayout* pComponentLayout = new QHBoxLayout();
        pComponentLayout->addWidget(pComponentWidget);

        pGroupBox->setLayout(pComponentLayout);

        /*
         * AFTER SETTING LAYOUT CARE FOR VISIBILITY
         */

        if (pGroupBox->isCheckable() == true)
        {


            std::string sVisible = this->getAttribute(pElement, "visible", "ALWAYS").toUpper().toStdString();

            if (sVisible.compare("ALWAYS") != 0)
            {
                QObject::connect( pGroupBox, &QExclusiveGroupBox::toggled, [pGroupBox, pComponentWidget] () {

                    if (pGroupBox->isCheckable())
                    {

                        if (pGroupBox->isChecked())
                        {
                            pComponentWidget->setVisible(true);
                        } else {
                            pComponentWidget->setVisible(false);
                        }

                    }

                } );

                if (sVisible.compare("FALSE") == 0)
                {
                    pGroupBox->setChecked(false);
                } else if (sVisible.compare("TRUE"))
                {
                    pGroupBox->setChecked(true);
                }

            }

        }

        pGroupBox->getConsistent();


        return pGroupBox;
    }

};


#endif //BIOGUI_WINDOWWIDGETGENERALGROUP_H
