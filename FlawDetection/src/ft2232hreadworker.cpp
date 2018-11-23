#include "ft2232hreadworker.h"
#include <QDebug>

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
//        if(!m_list.isEmpty()){
//            QByteArray arr = m_list.takeFirst();
//            emit dataReady(arr);
//        }

    }
}

void Ft2232HReadWorker::readFifo()
{
    static int time1,time2,time_16ms = 0;
    time1 = QTime::currentTime().msecsSinceStartOfDay();
    time_16ms += time1 - time2;
    time2 = QTime::currentTime().msecsSinceStartOfDay();


    FT_STATUS   ftStatus = FT_OK;
    DWORD       bytesReaded = 0;
    ftStatus = FT_Read(m_ftHandle, m_rdBuf, 1020, &bytesReaded);
    if (ftStatus != FT_OK)
    {
            printf("FT_Read failed (error %d).\n", (int)ftStatus);
            (void)FT_Close(m_ftHandle);
    }

    m_packetIndex++;
    if(m_packetIndex == 16){
        m_packetIndex = 0;
    }

    memcpy(m_dataPool[m_packetIndex], m_rdBuf, bytesReaded);
    if(time_16ms >= 16){
        time_16ms = 0;
//        QByteArray array((const char *)m_dataPool[(m_packetIndex > 0) ? m_packetIndex - 1 : 15], 1020);
        QByteArray array((const char *)m_dataPool[m_packetIndex], 1020);
        emit dataReady(array);
    }
//    printf("read: %d data.\n", bytesReaded);
//    for(int i=512;i<512+16/*bytesReaded*/;i++){
//            printf("%4x", m_rdBuf[i]);
//            if(i != 0 && i% 16 == 0)printf("\n");
//    }
//    QByteArray array((const char *)m_rdBuf, (int)bytesReaded);
//    emit dataReady(array);
//    if(m_repeatFreq <= 60){
////        m_list.append(array);
//        QByteArray array((const char *)m_rdBuf, (int)bytesReaded);
//        emit dataReady(array);
//    }else{
//        m_capCnt++;
//        if(m_capCnt >= m_capNum){
//            m_capCnt = 0;
////            m_list.append(array);
//            QByteArray array((const char *)m_rdBuf, (int)bytesReaded);
//            emit dataReady(array);
//        }
//    }

//    // calc "fps" : read as fast as possible[up to 1k]
//    static int frame_cnt=0;
//    static int fps = 0;
//    static int f_max=1, f_min=1000;
//    frame_cnt ++;
//    static int time1,time2,time_1s = 0;
//    time1 = QTime::currentTime().msecsSinceStartOfDay();
//    qDebug()<<__func__<<"time="<<time1;
//    time_1s += time1 - time2;
//    time2 = QTime::currentTime().msecsSinceStartOfDay();
////    if(time_1s > 1000){
////        fps = frame_cnt * 1000 / time_1s;
////        time_1s = 0;
////        frame_cnt = 0;
////        qDebug()<<__func__<<"fps="<<fps<<"min="<<f_min<<"max="<<f_max;
////    }



//    if(fps > 0){     // filter invalid data
//        if(fps > f_max) f_max = fps;
//        if(fps < f_min) f_min = fps;
//    }

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

