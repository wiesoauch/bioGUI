/*
 *  bioGUI - a new universal GUI for bioinformatic applications
 *  Copyright (C) 2016  Markus Joppich
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BIOGUI_QEXCLUSIVEGROUPBOX_H
#define BIOGUI_QEXCLUSIVEGROUPBOX_H

#include <QGroupBox>
#include <QAbstractButton>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include "QOrderedLayout.h"
#include "QExtGridLayout.h"

class QExclusiveGroupBox : public QGroupBox {

    Q_OBJECT

public:

    QExclusiveGroupBox(QString sTitle, QWidget* pParent = 0)
    :QGroupBox(sTitle, pParent)
    {


    }

    void setOrdered(bool bValue)
    {
        m_bOrdering = bValue;
    }

    bool getOrdered()
    {
        return m_bOrdering;
    }

    void setExclusive(bool bValue)
    {
        m_bExclusive = bValue;
    }

    bool getExclusive()
    {
        return m_bExclusive;
    }

    void addChild(QWidget* pChild);
    void elementChangedBox(QExclusiveGroupBox* pElement);
    void elementChangedAButton(QAbstractButton* pElement);

    QWidget* addNextWidget(QWidget* pWidget)
    {
        QWidget* pAddWidget = pWidget;

        if (m_bOrdering)
        {

            pAddWidget = new QWidget();

            QGridLayout* pLayout = new QGridLayout();
            pLayout->addWidget(new QLabel("Position"), 0,0);
            QComboBox* pOrdering = new QComboBox();
            pLayout->addWidget(pOrdering, 0, 1);
            pLayout->addWidget(pWidget, 1, 0, 1, 2);

            pAddWidget->setLayout(pLayout);

            m_mWidgetToSelBox.insert(std::pair<QWidget*, QComboBox*>(pAddWidget, pOrdering));

            //connect(pOrdering,static_cast< void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [] (int) {});

            QObject::connect(pOrdering,static_cast< void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [pAddWidget, this] (int iNewPos) {

                this->switchOrdering(pAddWidget, iNewPos);

            });
        }

        m_vWidgets.push_back(pAddWidget);
        this->addChild(pAddWidget);

        m_mAddedToOriginalWidget.insert(std::pair<QWidget*, QWidget*>(pAddWidget, pWidget));

        this->updateLayout();

        if (m_bOrdering)
        {

            m_bSwitchOrdering = false;
            this->updateOrderBoxes();
            m_bSwitchOrdering = true;
        }

        return pAddWidget;

    }

    std::vector<std::string> getOrderedIDs(std::map<QWidget*, std::string>* pMap, bool bOnlySelected)
    {
        std::vector<std::string> vReturn;

        for (size_t i = 0; i < m_vWidgets.size(); ++i)
        {
            std::map<QWidget*, std::string>::iterator oIt = pMap->find( m_vWidgets.at(i) );

            if (oIt != pMap->end())
            {

                if (bOnlySelected && !this->isSelected(oIt->first))
                    continue;

                vReturn.push_back( oIt->second );
            }
        }

        return vReturn;
    }

    void getConsistent()
    {

        if (!m_bExclusive)
            return;

        m_bExclusive = false;

        int iSelected = 0;

        for (size_t i = 0; i < m_vChildren.size(); ++i)
        {

            if ( QAbstractButton* pButton = dynamic_cast<QAbstractButton*> (m_vChildren.at(i)))
            {

                pButton->toggled(pButton->isChecked());

                if (pButton->isChecked())
                    ++iSelected;

            }

            if ( QExclusiveGroupBox* pGroup = dynamic_cast<QExclusiveGroupBox*> (m_vChildren.at(i)))
            {

                pGroup->toggled(pGroup->isChecked());

                if (pGroup->isChecked())
                    ++iSelected;

            }
        }

        if (iSelected != 1)
        {
            iSelected = 0;

            for (size_t i = 0; i < m_vChildren.size(); ++i)
            {

                if ( QAbstractButton* pButton = dynamic_cast<QAbstractButton*> (m_vChildren.at(i)))
                {

                    if (((pButton->isChecked()) && (iSelected == 0)))
                    {
                        ++iSelected;
                    } else {
                        pButton->setChecked( false );
                    }

                }

                if ( QExclusiveGroupBox* pGroup = dynamic_cast<QExclusiveGroupBox*> (m_vChildren.at(i)))
                {

                    if (((pGroup->isChecked()) && (iSelected == 0)))
                    {
                        ++iSelected;
                    } else {
                        pGroup->setChecked( false );
                    }
                }
            }

            if (iSelected == 0)
            {
                if (m_vChildren.size() > 0)
                {

                    if ( QAbstractButton* pButton = dynamic_cast<QAbstractButton*> (m_vChildren.at(0)))
                    {

                        pButton->setChecked( true );


                    } else if ( QExclusiveGroupBox* pGroup = dynamic_cast<QExclusiveGroupBox*> (m_vChildren.at(0)))
                    {

                        pGroup->setChecked( true );
                    }

                }
            }
        }

        m_bExclusive = true;

    }

protected:

    bool isSelected(QWidget* pShownWidget)
    {

        std::map<QWidget*, QWidget*>::iterator oIt = m_mAddedToOriginalWidget.find(pShownWidget);

        QWidget* pWidget = NULL;

        if (oIt != m_mAddedToOriginalWidget.end())
            pWidget = oIt->second;

        if ( QAbstractButton* pButton = dynamic_cast<QAbstractButton*> (pWidget))
        {

            return ((pButton->isCheckable() == false) || (pButton->isChecked()));

        }

        if ( QExclusiveGroupBox* pGroup = dynamic_cast<QExclusiveGroupBox*> (pWidget))
        {

            return ((pGroup->isCheckable() == false) || (pGroup->isChecked()));

        }

        return true;
    }

    void switchOrdering(QWidget* pWidget, int iNewPos)
    {

        if (!m_bSwitchOrdering)
            return;

        std::vector<QWidget*>::iterator oIt = std::find(m_vWidgets.begin(), m_vWidgets.end(), pWidget);

        QWidget* pPutBefore = NULL;
        pPutBefore = m_vWidgets.at(iNewPos);

        if (oIt != m_vWidgets.end())
        {

            m_vWidgets.erase(oIt);

            if (pPutBefore != NULL)
            {

                oIt = std::find(m_vWidgets.begin(), m_vWidgets.end(), pPutBefore);

                int iDist = std::distance(m_vWidgets.begin(), oIt);

                if (iDist < iNewPos)
                    ++oIt;


                m_vWidgets.insert(oIt, pWidget);

            } else {

                m_vWidgets.push_back(pWidget);

            }



        }

        m_bSwitchOrdering = false;
        this->updateOrderBoxes();
        this->updateLayout();
        m_bSwitchOrdering = true;
    }

    void updateLayout()
    {
        QLayout* pLayout = this->layout();

        if (QExtGridLayout* pOrdered = dynamic_cast<QExtGridLayout*>(pLayout))
        {
            pOrdered->updateWidgets(&m_vWidgets);
        }

    }

    void updateOrderBoxes()
    {

        std::map<QWidget*, QComboBox*>::iterator oIt = m_mWidgetToSelBox.begin();

        while (oIt != m_mWidgetToSelBox.end())
        {

            QComboBox* pBox = oIt->second;

            pBox->clear();

            for (size_t i = 0; i < m_vWidgets.size(); ++i)
            {

                QString sItemNum = QString(std::to_string(i+1).c_str());

                pBox->addItem( sItemNum, (int)i );

                if (m_vWidgets.at(i) == oIt->first)
                {
                    pBox->setCurrentIndex(i);
                }

            }

            ++oIt;

        }



    }

    int currentElements()
    {
        return m_vWidgets.size();
    }

    std::map<QWidget*, QWidget*> m_mAddedToOriginalWidget;
    std::map<QWidget*, QComboBox*> m_mWidgetToSelBox;
    std::vector<QWidget*> m_vWidgets;
    bool m_bOrdering = false;
    bool m_bSwitchOrdering = true;
    bool m_bExclusive = false;

    std::vector<QWidget*> m_vChildren;

};


#endif //BIOGUI_QEXCLUSIVEGROUPBOX_H
