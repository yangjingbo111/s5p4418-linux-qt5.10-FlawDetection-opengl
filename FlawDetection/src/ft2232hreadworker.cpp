#include "ft2232hreadworker.h"
#include <QDebug>
#include <QString>
#include <iostream>


Ft2232HReadWorker::Ft2232HReadWorker(FT_HANDLE ftHandle, QObject *parent)
    :QObject (parent)
{
    m_ftHandle = ftHandle;
    m_isWorking = true;//probeFt2232H();   // probe ft2232h function
    m_needRead = true;      // read forever
    m_repeatFreq = 30;

}

Ft2232HReadWorker::~Ft2232HReadWorker()
{

}

void Ft2232HReadWorker::readLoop()
{
    while (m_isWorking) {
        if(m_needRead){
            readFifo();
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

        if(0){
            QFile file("/opt/raw-data.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
                return;


            QTextStream out(&file);

            for (int i=0;i<bytesReaded;i++){
                out << QString::number(m_dataPool[i], 16).rightJustified(2).toUpper()<<" ";
            }

            out<<"\r\n";    // packet seperate

            file.close();
        }


        // packet loss test[result: no loss 20181218]
//        // 0x20C[7:0] 0x20D 0X20E 0X20F is packet counter
//        static int packet_cnt = (int)m_dataPool[0x20C];

//        if(-255 != (int)m_dataPool[0x20c] - packet_cnt && 1 != (int)m_dataPool[0x20c] - packet_cnt){
//            qDebug()<<"loss packet: "<<(int)m_dataPool[0x20c]-packet_cnt;

//        }
//        packet_cnt = (int)m_dataPool[0x20C];

        if(1 || (m_dataPool[0x209] == 0XA5 && m_dataPool[0x20B] == 0X5A)){
            QByteArray array((const char *)m_dataPool, PACKET_SIZE);
            emit dataReady(array);
        }

        if(0 && bytesReaded != PACKET_SIZE){
            QFile file("/opt/packet-size.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
                return;

            QTextStream out(&file);

            out << bytesReaded;

            out<<"\r\n";    // packet seperate

            file.close();
        }
    }

}

void Ft2232HReadWorker::wrrepeatFreq(int val)
{
    if(m_repeatFreq != val){
        m_repeatFreq = val;
        m_capCnt = 0;
        m_capNum = m_repeatFreq / 60;
    }
}

void Ft2232HReadWorker::initWorker()
{
    readLoop();
}

void Ft2232HReadWorker::test_timer()
{

    if(m_needRead){
        readFifo();
    }

}

