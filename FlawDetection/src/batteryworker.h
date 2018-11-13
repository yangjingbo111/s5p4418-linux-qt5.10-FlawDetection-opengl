#ifndef BATTERYWORKER_H
#define BATTERYWORKER_H

#include <QObject>
#include <QTimer>

class BatteryWorker : public QObject
{
Q_OBJECT
public:
    BatteryWorker();

signals:
    void adcReceived(QString val);
    void pwrSrcChanged(int src);    // emit when pwr src changed
    void chgStatusChanged(int chg); // emit when chg status changed

public slots:
    void getadc(int ch);
    void getPwrSrc();
    void getChgStatus();

private:
    void saveAdcToFile(QString filename, QString val);

private:
    QTimer *m_bat_timer;  // check pwr src and charging status timer
};

#endif // BATTERYWORKER_H
