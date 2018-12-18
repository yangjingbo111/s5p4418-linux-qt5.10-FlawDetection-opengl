#include "ft2232hwrapper.h"

#include <QDebug>



Ft2232HWrapper::Ft2232HWrapper(QObject *parent)
    :QObject (parent)
{
    qDebug()<<__func__<<QThread::currentThreadId();
    // probe if ft2232h function well [ftHandle and working status are returned]
    m_isFt2232hWorking = probeFt2232H();

    if(m_isFt2232hWorking){
        m_worker = new Ft2232HWorker(ftHandle);
        m_thread = new QThread;
        connect(this, SIGNAL(callWorker(int)), m_worker, SLOT(testFun(int)),Qt::QueuedConnection);

        // control API
        // menu 1
        connect(this, SIGNAL(wrGainSig(int)), m_worker, SLOT(wrGain(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrRangeSig(int)), m_worker, SLOT(wrRange(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrHighVolSig(int)), m_worker, SLOT(wrHighVol(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrEmitDampSig(int)), m_worker, SLOT(wrEmitDamp(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrFilterBandSig(int)), m_worker, SLOT(wrFilterBand(int)), Qt::QueuedConnection);
        // menu 2
        connect(this, SIGNAL(wrrectificationTypeSig(int)), m_worker, SLOT(wrrectificationType(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrtransducerTypeSig(int)), m_worker, SLOT(wrtransducerType(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrtcgGainSig(int)), m_worker, SLOT(wrtcgGain(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrkeyBoardBacklightSig(int)), m_worker, SLOT(wrkeyBoardBacklight(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrvgaSig(int)), m_worker, SLOT(wrvga(int)), Qt::QueuedConnection);
        // menu 3
        connect(this, SIGNAL(wrrepeatFreqSig(int)), m_worker, SLOT(wrrepeatFreq(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrhardwareDrawSig(int)), m_worker, SLOT(wrhardwareDraw(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrechoDisplayModeSig(int)), m_worker, SLOT(wrechoDisplayMode(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrechoFreezeSig(int)), m_worker, SLOT(wrechoFreeze(int)), Qt::QueuedConnection);
        connect(this, SIGNAL(wrEchoColorSig(int)), m_worker, SLOT(wrEchoColor(int)), Qt::QueuedConnection);



        m_worker->moveToThread(m_thread);

        m_thread->start();

        m_readWorker = new Ft2232HReadWorker(ftHandle);
        m_readThread = new QThread;
        connect(this, SIGNAL(initWorker()), m_readWorker, SLOT(initWorker()), Qt::QueuedConnection);
        connect(m_readWorker, SIGNAL(dataReady(QByteArray)), this, SIGNAL(dataReady(QByteArray)), Qt::QueuedConnection);
//        connect(m_readWorker, SIGNAL(dataReady(QByteArray)), this, SLOT(catchData(QByteArray)), Qt::QueuedConnection);
        m_readWorker->moveToThread(m_readThread);
//        m_readThread->start(QThread::TimeCriticalPriority);
        m_readThread->start();
        emit initWorker();
    }

}

Ft2232HWrapper::~Ft2232HWrapper()
{

}


void Ft2232HWrapper::testFun(int num)
{
    emit callWorker(num);
}

void Ft2232HWrapper::closeFt2232H()
{
    if(ftHandle){
        (void)FT_Close(ftHandle);
    }
}

void Ft2232HWrapper::wrGain(int val)
{
    emit wrGainSig(val);
}

void Ft2232HWrapper::wrRange(int val)
{
    emit wrRangeSig(val);
}

void Ft2232HWrapper::wrHighVol(int val)
{

    emit wrHighVolSig(val);
}

void Ft2232HWrapper::wrEmitDamp(int val)
{
    emit wrEmitDampSig(val);
}

void Ft2232HWrapper::wrFilterBand(int val)
{
    emit wrFilterBandSig(val);
}

void Ft2232HWrapper::wrrectificationType(int val)
{
    emit wrrectificationTypeSig(val);
}

void Ft2232HWrapper::wrtransducerType(int val)
{
    emit wrtransducerTypeSig(val);
}

void Ft2232HWrapper::wrtcgGain(int val)
{
    emit wrtcgGainSig(val);
}

void Ft2232HWrapper::wrkeyBoardBacklight(int val)
{
    emit wrkeyBoardBacklightSig(val);
}

void Ft2232HWrapper::wrvga(int val)
{
    emit wrvgaSig(val);
}

void Ft2232HWrapper::wrrepeatFreq(int val)
{
    emit wrrepeatFreqSig(val);

    // [as there is a loop in that worker, can't use signal-slot]
    m_readWorker->wrrepeatFreq(val);


}

void Ft2232HWrapper::wrhardwareDraw(int val)
{
    emit wrhardwareDrawSig(val);
}

void Ft2232HWrapper::wrechoDisplayMode(int val)
{
    emit wrechoDisplayModeSig(val);
}

void Ft2232HWrapper::wrechoFreeze(int val)
{
    emit wrechoFreezeSig(val);
}

void Ft2232HWrapper::wrEchoColor(int val)
{
    emit wrEchoColorSig(val);
}

void Ft2232HWrapper::catchData(QByteArray data)
{
    qDebug()<<data.mid(512, 16);
}

bool Ft2232HWrapper::probeFt2232H()
{
    FT_STATUS       ftStatus = FT_OK;
    ftStatus = FT_Open(portNumber, &ftHandle);
    if (ftStatus != FT_OK)
    {
        /* FT_Open can fail if the ftdi_sio module is already loaded. */
        printf("FT_Open(%d) failed (error %d).\n", portNumber, (int)ftStatus);
        printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
        printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
        return false;
    }
    printf("Selecting sync fifo mode.\n");
    ftStatus = FT_SetBitMode(ftHandle,
                             0xFF, /* sets all 8 pins as outputs */
                             FT_BITMODE_RESET);
    if (ftStatus != FT_OK)
    {
            printf("FT_SetBitMode to RESET failed (error %d).\n", (int)ftStatus);
            goto exit;
    }
    ftStatus = FT_SetBitMode(ftHandle,
                             0xFF, /* sets all 8 pins as outputs */
                             FT_BITMODE_SYNC_FIFO);
    if (ftStatus != FT_OK)
    {
            printf("FT_SetBitMode to SYNC FIFO failed (error %d).\n", (int)ftStatus);
            goto exit;
    }

    ftStatus = FT_SetLatencyTimer(ftHandle, 16/*2*/);
    if (ftStatus != FT_OK)
    {
            printf("FT_SetLatencyTimer failed (error %d).\n", (int)ftStatus);
            goto exit;
    }
//    UCHAR latency;
//    ftStatus = FT_GetLatencyTimer(ftHandle, &latency);
//    if (ftStatus != FT_OK)
//    {
//            printf("FT_GetLatencyTimer failed (error %d).\n", (int)ftStatus);
//            goto exit;
//    }
//    qDebug()<<__func__<<"FT_GetLatencyTimer"<<latency;

    // test start 20181126

//    ftStatus = FT_SetUSBParameters (ftHandle, 1020*16, 512);
    ftStatus = FT_SetUSBParameters (ftHandle, 1024, /*512*/1024);
    if (ftStatus != FT_OK)
    {
            printf("FT_SetUSBParameters failed (error %d).\n", (int)ftStatus);
            goto exit;
    }
//    ftStatus = FT_SetTimeouts (ftHandle, 1000, 200);
//    if (ftStatus != FT_OK)
//    {
//            printf("FT_SetTimeouts failed (error %d).\n", (int)ftStatus);
//            goto exit;
//    }
    // test end 20181126


    ftStatus = FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0, 0);
    if (ftStatus != FT_OK)
    {
            printf("FT_SetFlowControl failed (error %d).\n", (int)ftStatus);
            goto exit;
    }
    ftStatus = FT_Purge(ftHandle, FT_PURGE_TX | FT_PURGE_RX);
    if (ftStatus != FT_OK)
    {
            printf("FT_Purge failed (error %d).\n", (int)ftStatus);
            goto exit;
    }


//    (void)FT_Close(ftHandle);
    return true;    // init ft2232h ok

exit:
    (void)FT_Close(ftHandle);
    return false;   // init ft2232h failed

}



