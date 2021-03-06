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

#ifndef BIOGUI_ADVANCEDSTREAMBOX_H
#define BIOGUI_ADVANCEDSTREAMBOX_H

#include <QListWidget>
#include <QTextCursor>
#include <QProcess>
#include <QAbstractButton>
#include <QFile>
#include <QFontDatabase>
#include <QTextStream>

#include <iostream>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/QSizeGrip>
#include <QtWidgets/QGridLayout>

#include "TCPExtendedBuffer.h"
#include "ExtendedThreadBuffer.h"
#include "ExtendedProcessBuffer.h"
#include "ExtendedStdBuffer.h"

#include <QMouseEvent>
#include <QtGui/QPainter>


class AdvancedListWidgetItem : public QListWidgetItem {

public:

    AdvancedListWidgetItem(QString sText, QString sStreamID)
            : QListWidgetItem(sText)
    {
        m_bFinished = false;
        this->setStreamID(sStreamID);

        const QFont oFixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        this->setFont(oFixedFont);


    }

    QString getStreamID()
    {
        return m_sStreamID;
    }

    void setStreamID(QString sStreamID)
    {
        m_sStreamID = sStreamID;
    }

    void setFinished(bool bValue)
    {
        m_bFinished = bValue;
    }

    void appendText(QString sText)
    {

        if (sText.at(sText.size()-1) == '\n')
        {
            m_bFinished = true;
        }

        QString sMyText = this->text();
        sMyText.append(sText);
        this->setText(sMyText);


    }

    void printStreamID()
    {
        std::cout << m_sStreamID.toStdString() << std::endl;
    }

    void setTextColor(QColor oColor)
    {
        this->setTextColor(oColor);
    }

    bool isFinished()
    {
        return m_bFinished;
    }

protected:

    QString m_sStreamID;
    bool m_bFinished;


};

class AdvancedStreamBox;

class AdvancedCornerWidget : public QWidget {
    Q_OBJECT

public:

    AdvancedCornerWidget(QWidget* pParentBox);

    void updateWidgetGeometry();

    void mousePressEvent(QMouseEvent* pEvent);

    void mouseMoveEvent(QMouseEvent * pEvent);

    void mouseReleaseEvent(QMouseEvent * pEvent);


signals:

    void sizeChanged();

protected:

    QPoint m_oMouseStart;
    QPoint m_oMouseEnd;
    QSize m_oCurrentSize;

    QWidget* m_pParentBox;

};

class AdvancedStreamBox : public QListWidget {
Q_OBJECT

public:

    AdvancedStreamBox()
            : QListWidget()
    {
        qRegisterMetaType<QTextCursor>("QTextCursor");
        qRegisterMetaType<QString>("QString");

        this->setResizeMode(QListWidget::Fixed);
        //this->setSizeAdjustPolicy(QListWidget::);
        this->setMinimumHeight(150);

        this->clear();

        this->setSelectionMode(QAbstractItemView::MultiSelection);

        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        AdvancedCornerWidget* pCornerWidget = new AdvancedCornerWidget(this);


        this->connect(pCornerWidget, &AdvancedCornerWidget::sizeChanged, this, &AdvancedStreamBox::sizeChanged);

        this->setCornerWidget( pCornerWidget );

    }

    ~AdvancedStreamBox()
    {
        std::cerr << "Deleting Advanced Stream Box" << std::endl;
    }



    virtual QSize sizeHint()
    {
        return QSize( this->minimumWidth(), this->minimumHeight() );
    }

    std::string getStreamContent(std::string* pStreamID)
    {
        std::string sReturn = "";

        size_t iCount = this->count();

        for (size_t i = 0; i < iCount; ++i)
        {

            AdvancedListWidgetItem* pItem = dynamic_cast<AdvancedListWidgetItem*>( this->item(i) );

            if (pItem == NULL)
                return NULL;

            if (pItem->getStreamID().compare( QString(pStreamID->c_str()), Qt::CaseInsensitive) == 0)
            {
                sReturn.append(pItem->text().toStdString());

                if (sReturn.size() > 0)
                    sReturn.append("\n");
            }

        }

        return sReturn;
    }

    void addProcBuffer(QProcess* pProcess, ExtendedProcessBuffer* pBuffer)
    {

        std::map<QProcess*, std::vector<ExtendedProcessBuffer*> >::iterator oIt = m_mProcToBuffer.find(pProcess);

        if (oIt != m_mProcToBuffer.end())
        {
            oIt->second.push_back(pBuffer);
        } else {
            std::pair<QProcess*, std::vector<ExtendedProcessBuffer*>> oPair(pProcess, std::vector<ExtendedProcessBuffer*>());

            oPair.second.push_back(pBuffer);
            m_mProcToBuffer.insert( oPair );

        }


    }

    void addThreadBuffer(ExecuteThread* pThread, ExtendedThreadBuffer* pBuffer)
    {

        std::map<ExecuteThread*, std::vector<ExtendedThreadBuffer*> >::iterator oIt = m_mThreadToBuffer.find(pThread);

        if (oIt != m_mThreadToBuffer.end())
        {
            oIt->second.push_back(pBuffer);
        } else {
            std::pair<ExecuteThread*, std::vector<ExtendedThreadBuffer*>> oPair(pThread, std::vector<ExtendedThreadBuffer*>());

            oPair.second.push_back(pBuffer);
            m_mThreadToBuffer.insert( oPair );

        }


    }

    void finishProcess(QProcess* pProcess)
    {
        std::map<QProcess*, std::vector<ExtendedProcessBuffer*> >::iterator oIt = m_mProcToBuffer.find(pProcess);

        if (oIt != m_mProcToBuffer.end())
        {

            for (size_t i = 0; i < oIt->second.size(); ++i)
            {

                ExtendedProcessBuffer* pBuffer = oIt->second.at(i);

                LOGERROR("Deleting PROC Buffer: " + std::to_string((uint64_t) pBuffer));

                pBuffer->transferText("\n");
                pBuffer->deleteLater();

            }

            m_mProcToBuffer.erase(oIt);


        }
    }

    void finishThread( ExecuteThread* pThread )
    {

        LOGERROR("Finishing thread: " + std::to_string((uint64_t) pThread));

        /*
        if (pThread == NULL)
            return;
        */

        std::map<ExecuteThread*, std::vector<ExtendedThreadBuffer*> >::iterator oJt = m_mThreadToBuffer.find(pThread);

        if (oJt != m_mThreadToBuffer.end())
        {

            for (size_t i = 0; i < oJt->second.size(); ++i)
            {

                ExtendedThreadBuffer* pBuffer = oJt->second.at(i);
                pBuffer->transferText("\n");
                pBuffer->stopTransmissions();
                pBuffer->deleteLater();

            }

            m_mThreadToBuffer.erase(oJt);

        }

    }

    void addTCPBuffer(ExecuteThread* pThread, std::string sHost, int iPort, QString sStreamID, QColor oTextColor)
    {

        TCPExtendedBuffer* pBuffer = new TCPExtendedBuffer(pThread, QString(sHost.c_str()), iPort);
        pBuffer->setTextColor( oTextColor );
        pBuffer->setStreamID( sStreamID );

        this->addThreadBuffer(pThread, pBuffer);

        QObject::connect(pBuffer, &TCPExtendedBuffer::sendText, this , &AdvancedStreamBox::receiveText, Qt::QueuedConnection );

    }

    void addThreadBuffer(ExecuteThread* pThread, QProcess::ProcessChannel eChannel, QString sStreamID, QColor oTextColor)
    {

        ExtendedThreadBuffer* pBuffer = new ExtendedThreadBuffer(pThread, eChannel);
        pBuffer->setTextColor( oTextColor );
        pBuffer->setStreamID( sStreamID );

        this->addThreadBuffer(pThread, pBuffer);

        switch (eChannel)
        {
            default:

            case QProcess::ProcessChannel::StandardOutput:

                QObject::connect( pThread, &ExecuteThread::readyReadStandardOutput, pBuffer, &ExtendedThreadBuffer::receiveThreadData );


                break;
            case QProcess::ProcessChannel::StandardError :

                QObject::connect( pThread, &ExecuteThread::readyReadStandardError, pBuffer, &ExtendedThreadBuffer::receiveThreadData );

                break;

        }

        QObject::connect(pBuffer, SIGNAL(sendText(QString,QColor, QString)), this , SLOT(receiveText(QString,QColor, QString)), Qt::QueuedConnection );

    }

    void addProcessBuffer(QProcess* pProcess, QProcess::ProcessChannel eChannel, QString sStreamID, QColor oTextColor)
    {

        ExtendedProcessBuffer* pBuffer = new ExtendedProcessBuffer(pProcess, eChannel);
        pBuffer->setTextColor( oTextColor );
        pBuffer->setStreamID( sStreamID );

        this->addProcBuffer(pProcess, pBuffer);

        switch (eChannel)
        {
            default:

            case QProcess::ProcessChannel::StandardOutput:

                QObject::connect( pProcess, &QProcess::readyReadStandardOutput, pBuffer, &ExtendedProcessBuffer::receiveProcData );


                break;
            case QProcess::ProcessChannel::StandardError :

                QObject::connect( pProcess, &QProcess::readyReadStandardError, pBuffer, &ExtendedProcessBuffer::receiveProcData );

                break;

        }

        QObject::connect(pBuffer, SIGNAL(sendText(QString,QColor, QString)), this , SLOT(receiveText(QString,QColor, QString)), Qt::QueuedConnection );

    }


    void addStream(std::string sStreamID, QCheckBox* pControl)
    {

        m_mStreamID2Button.insert( std::pair<std::string, QAbstractButton*>(sStreamID, pControl) );
        QObject::connect(pControl, SIGNAL(toggled(bool)), this , SLOT(filterText(bool)), Qt::QueuedConnection );

        // std::cout << "added stream " << sStreamID << std::endl;

    }

    void saveToFile(QString sFileName)
    {

        QFile oFile( sFileName );
        if ( oFile.open( QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text ) )
        {
            QTextStream stream( &oFile );

            size_t iCount = this->count();
            for (size_t i = 0; i < iCount; ++i) {

                size_t iIndex = iCount - 1 - i;

                AdvancedListWidgetItem *pItem = dynamic_cast<AdvancedListWidgetItem *>( this->item(iIndex));

                stream << pItem->text() << endl;
            }

            stream.flush();
            oFile.close();

        }


    }


signals:

    void sizeChanged();

protected slots:

    void filterText(bool bState)
    {

        std::map<std::string, QAbstractButton*>::iterator oIt = m_mStreamID2Button.begin();
        std::map<std::string, bool> mShowText;

        while( oIt != m_mStreamID2Button.end())
        {

            mShowText.insert( std::pair<std::string, bool>( oIt->first, oIt->second->isChecked() ));

            //std::cout << "set " << oIt->first << " " << oIt->second->isChecked() << std::endl;

            ++oIt;

        }

        size_t iCount = this->count();

        for (size_t i = 0; i < iCount; ++i)
        {

            size_t iIndex = iCount - 1 - i;

            AdvancedListWidgetItem* pItem = dynamic_cast<AdvancedListWidgetItem*>( this->item(iIndex) );

            if (pItem == NULL)
                continue;

            //pItem->printStreamID();

            std::map<std::string, bool>::iterator oDecide = mShowText.find( pItem->getStreamID().toStdString() );

            if (oDecide != mShowText.end())
            {

                //std::cout << pItem->text().toStdString() << " " << oDecide->second << std::endl;
                pItem->setHidden(!oDecide->second);

            }

        }

        this->scrollToBottom();

    }


    void receiveText(QString sString, QColor oColor, QString sStreamID)
    {

        LOGERROR("AdvancedStreamBox received: " + std::to_string((uint64_t) this) + " for STREAM: " + sStreamID.toStdString());
        LOGERROR( sString.toStdString() );
        LOGERROR("RECEIVE END");

        AdvancedListWidgetItem* pLastStream = this->getLastItemForStream(&sStreamID);

        bool bLastIsFinished = false;
        if ((sString.size() > 0) && (sString[sString.size()-1] == '\n'))
            bLastIsFinished = true;

        QStringList vList = sString.split("\n");

        if ((vList.size() == 0) || (vList.at(0).length() == 0))
        {

            if (pLastStream != NULL)
            {
                pLastStream->setFinished(true);
            }

        } else {

            if ((pLastStream == NULL) || (pLastStream->isFinished()))
            {
                pLastStream = new AdvancedListWidgetItem("", sStreamID);
                pLastStream->setForeground(oColor);
                this->addItem(pLastStream);
            }

            pLastStream->appendText( vList.at(0) );
        }


        for (size_t i = 1; i < vList.size(); ++i)
        {

            //std::cout << "more: '" << vList.at(i).toStdString() << "'" << std::endl;

            if (vList.at(i).length() > 0)
            {
                pLastStream = new AdvancedListWidgetItem( vList.at(i), sStreamID );
                pLastStream->setForeground(oColor);

                this->addItem(pLastStream);

            }

            if (pLastStream != NULL)
                if ((i < vList.size()-1) || (( i == vList.size()-1 ) && (bLastIsFinished)))
                    pLastStream->setFinished(true);

        }


        this->filterText(true);

    }


protected:


    AdvancedListWidgetItem* getLastItemForStream(QString* sStreamID)
    {

        size_t iCount = this->count();

        for (size_t i = 0; i < iCount; ++i)
        {

            size_t iIndex = iCount - 1 - i;

            AdvancedListWidgetItem* pItem = dynamic_cast<AdvancedListWidgetItem*>( this->item(iIndex) );

            if (pItem == NULL)
                return NULL;

            if (pItem->getStreamID().compare( QString(sStreamID->toStdString().c_str()), Qt::CaseInsensitive) == 0)
            {
                return pItem;
            }

        }

        return NULL;
    }


    std::map<QProcess*, std::vector<ExtendedProcessBuffer*>> m_mProcToBuffer;
    std::map<ExecuteThread*, std::vector<ExtendedThreadBuffer*>> m_mThreadToBuffer;

    std::map<std::string, QAbstractButton*> m_mStreamID2Button;


};


#endif //BIOGUI_ADVANCEDSTREAMBOX_H
