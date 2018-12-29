#ifndef FT2232HWRAPPER_H
#define FT2232HWRAPPER_H

#include <QObject>
#include <QThread>
#include "ftd2xx.h"
#include "ft2232hworker.h"
#include "ft2232hreadworker.h"

class Ft2232HWrapper : public QObject
{
    Q_OBJECT
public:
    explicit Ft2232HWrapper(QObject *parent = nullptr);
    ~Ft2232HWrapper();
    Q_INVOKABLE void testFun(int num);
    Q_INVOKABLE void closeFt2232H();

    Q_INVOKABLE void wrGain(float val);
    Q_INVOKABLE void wrRange(int val);
    Q_INVOKABLE void wrHighVol(int val);
    Q_INVOKABLE void wrEmitDamp(int val);
    Q_INVOKABLE void wrFilterBand(int val);

    Q_INVOKABLE void wrrectificationType(int val);
    Q_INVOKABLE void wrtransducerType(int val);
    Q_INVOKABLE void wrtcgGain(int val);
    Q_INVOKABLE void wrkeyBoardBacklight(int val);
    Q_INVOKABLE void wrvga(int val);

    Q_INVOKABLE void wrrepeatFreq(int val);
    Q_INVOKABLE void wrhardwareDraw(int val);
    Q_INVOKABLE void wrechoDisplayMode(int val);
    Q_INVOKABLE void wrechoFreeze(int val);
    Q_INVOKABLE void wrEchoColor(int val);
signals:
    void initWorker();
    void callWorker(int num);
    void dataReady(QByteArray data);
    // menu 1
    void wrGainSig(float val);
    void wrRangeSig(int val);
    void wrHighVolSig(int val);
    void wrEmitDampSig(int val);
    void wrFilterBandSig(int val);
    // menu 2
    void wrrectificationTypeSig(int val);
    void wrtransducerTypeSig(int val);
    void wrtcgGainSig(int val);
    void wrkeyBoardBacklightSig(int val);
    void wrvgaSig(int val);
    // menu 3
    void wrrepeatFreqSig(int val);
    void wrhardwareDrawSig(int val);
    void wrechoDisplayModeSig(int val);
    void wrechoFreezeSig(int val);
    void wrEchoColorSig(int val);

public slots:
       void catchData(QByteArray data);
private:
    bool probeFt2232H();

private:
    FT_HANDLE       ftHandle;
    int             portNumber = 0;
    bool            m_isFt2232hWorking;

    QThread *m_thread;
    QThread *m_readThread;
    Ft2232HWorker *m_worker;
    Ft2232HReadWorker *m_readWorker;
    int m_repeatFreq;
};

#endif // FT2232HWRAPPER_H
