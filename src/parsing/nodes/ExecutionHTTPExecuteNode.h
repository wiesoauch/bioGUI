#ifndef EXECUTIONHTTPEXECUTENODE_H
#define EXECUTIONHTTPEXECUTENODE_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QThread>
#include <src/app/ExecuteThread.h>

#include "ExecutionExecuteNode.h"


class HTTPExecuteThread : public ExecuteThread
{
public:
    HTTPExecuteThread(QNetworkAccessManager* pNetworkAccessManager, QNetworkRequest* pRequest, QUrlQuery* pParams)
    : ExecuteThread()
    {
        m_pNetworkAccessManager = pNetworkAccessManager;
        m_pRequest = pRequest;
        m_pParams = pParams;


        m_pNetworkAccessManager->connect(m_pNetworkAccessManager, &QNetworkAccessManager::finished, [this](QNetworkReply* pReply){
            this->getExecutionResponse(pReply);
        });


    }

    void getExecutionResponse(QNetworkReply* pReply)
    {

        m_vReplies.push_back(pReply);

        emit readyReadStandardOutput();
        emit executionFinished();
    }

    QByteArray readAllStandardOutput()
    {

        QByteArray oReturn;

        std::vector<QNetworkReply*>::iterator oIt = m_vReplies.begin();

        while( oIt != m_vReplies.end() )
        {

            QNetworkReply* pReply = *oIt;

            QByteArray oReplyData = pReply->readAll();

            oReturn.append(oReplyData);

            oIt = m_vReplies.erase(oIt);

        }

        return oReturn;

    }

    QByteArray readAllStandardError()
    {
        return QByteArray();
    }

    void execute()
    {
        m_pNetworkAccessManager->post(*m_pRequest, m_pParams->toString(QUrl::FullyEncoded).toUtf8());
    }

protected:


    QNetworkAccessManager* m_pNetworkAccessManager;
    QNetworkRequest* m_pRequest;
    QUrlQuery* m_pParams;

    std::vector<QNetworkReply*> m_vReplies;

};

class ExecutionHTTPExecuteNode : public ExecutionExecuteNode
{
public:

    ExecutionHTTPExecuteNode(QDomElement* pElement)
        : ExecutionExecuteNode(pElement)
    {


        m_iPort = this->getDomElementAttribute(pElement, "port", "-1").toInt();

    }


    virtual std::string evaluate( std::map< std::string, ExecutionNode*>* pID2Node,
                                  std::map<std::string, std::string>* pInputID2Value,
                                  std::map<std::string, QWidget*>* pInputID2Widget)
    {

        QNetworkAccessManager *pNetworkManager = new QNetworkAccessManager();

        std::string sURL = m_sExecLocation;

        if (m_iPort != -1)
        {
            sURL = sURL + ":" + std::to_string(m_iPort);
        }

        sURL = sURL + m_sExecutable;

        QString qsURL( sURL.c_str() );

        QUrlQuery postData;
        std::string sCLArg = this->getCLArgs(pID2Node, pInputID2Value, pInputID2Widget);
        QString qsCLArg(sCLArg.c_str());

        QStringList oArgs = ProcessLauncher::stringToArguments(qsCLArg, '\"');

        for (int i = 0; i < oArgs.size(); ++i)
        {

            QString sArg = oArgs.at(i);

            if (sArg.startsWith("--"))
            {
                postData.addQueryItem(sArg , "");
            } else if (sArg.startsWith("-"))
            {
                if (i+1 < oArgs.size())
                {
                    postData.addQueryItem(sArg , oArgs.at(i+1));
                    ++i;
                }
            } else {
                throw "error parsing post data";
            }

        }

        QNetworkRequest oNetRequest( qsURL );
        oNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        HTTPExecuteThread* pThread = new HTTPExecuteThread(pNetworkManager, &oNetRequest, &postData);

        this->evaluateChildren(pID2Node, pInputID2Value, pInputID2Widget, NULL, pThread, false);

        pThread->start();

        QObject::connect(pThread, &ExecuteThread::executionFinished, [pThread, pNetworkManager, pID2Node, pInputID2Value, pInputID2Widget, this](){

            this->evaluateChildren(pID2Node, pInputID2Value, pInputID2Widget, NULL, pThread, true);
            pNetworkManager->deleteLater();

        });

        return "";

    }

protected:

    int m_iPort = -1;

};

#endif // EXECUTIONHTTPEXECUTENODE_H
