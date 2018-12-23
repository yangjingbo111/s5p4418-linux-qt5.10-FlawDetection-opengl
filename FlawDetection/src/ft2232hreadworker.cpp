#include "ft2232hreadworker.h"
#include <QDebug>
#include <QString>
#include <iostream>


Ft2232HReadWorker::Ft2232HReadWorker(FT_HANDLE ftHandle, QObject *parent)
    :QObject (parent)
{
    m_ftHandle = ftHandle;
    m_isWorking = true;//probeFt2232H();   // probe ft2232h function

}

Ft2232HReadWorker::~Ft2232HReadWorker()
{

}

void Ft2232HReadWorker::readLoop()
{
    m_timer = new QTimer;
    m_timer->setInterval(16);
    m_timer->start();

    // Attention: this loop never exit!!
    // This sub-thread will not run other slots[timerout slots for example]
    while (m_isWorking) {
        readFifo();

        // use timer to dispatch packets
        if(m_timer->remainingTime() == 0){
            m_timer->start();   // restart timer
            if(!m_list.isEmpty()){
                emit dataReady(m_list.takeAt(0));
            }
        }
    }
}

void Ft2232HReadWorker::readFifo()
{
    FT_STATUS   ftStatus = FT_OK;
    DWORD EventDWord;
    DWORD RxBytes;
    DWORD TxBytes;
    DWORD       bytesReaded = 0;
    ftStatus = FT_GetStatus(m_ftHandle, &RxBytes, &TxBytes, &EventDWord);
    if(ftStatus == FT_OK && RxBytes >= PACKET_SIZE){
        ftStatus = FT_Read(m_ftHandle, m_dataPool, PACKET_SIZE/*RxBytes*/, &bytesReaded);
        if (ftStatus != FT_OK)
        {
            qDebug()<<__func__<<"FT_Read failed (error %d) "<<(int)ftStatus;
            (void)FT_Close(m_ftHandle);
        }

        QByteArray array((const char *)m_dataPool, PACKET_SIZE);
//            emit dataReady(array);
        // keep MAX list size to 16 by dropping packet when repeat_freq > 60
        if(m_list.size()< 16){
            m_list.append(array);
        }

    }

}

void Ft2232HReadWorker::wrrepeatFreq(int val)
{
    if(m_repeatFreq != val){
        m_repeatFreq = val;
    }
}

void Ft2232HReadWorker::initWorker()
{
    readLoop();
}

