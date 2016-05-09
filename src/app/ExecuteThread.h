//
// Created by mjopp on 09.05.2016.
//

#ifndef BIOGUI_EXECUTETHREAD_H
#define BIOGUI_EXECUTETHREAD_H

#include <QThread>

#include <iostream>


class ExecuteThread : public QThread
{
    Q_OBJECT

public:

    ExecuteThread() : QThread()
    {
        this->connect(this, &QThread::started, this, &ExecuteThread::startExecution);

    }


    virtual QByteArray readAllStandardOutput() = 0;
    virtual QByteArray readAllStandardError() = 0;

public slots:

    virtual void startExecution() //QPrivateSignal* pSignal
    {

        std::cout.rdbuf();
        std::cerr.rdbuf();

        this->execute();
    }

    signals:

    void executionFinished();

    void readyReadStandardOutput();
    void readyReadStandardError();


protected:

    virtual void execute() = 0;

    //ExtendedThreadBuffer* m_pSTDBuffer;
    //ExtendedThreadBuffer* m_pERRBuffer;


};


#endif //BIOGUI_EXECUTETHREAD_H