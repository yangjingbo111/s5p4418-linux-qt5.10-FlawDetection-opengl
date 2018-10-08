#ifndef BATTERYWORKER_H
#define BATTERYWORKER_H

#include <QObject>

class BatteryWorker : public QObject
{
Q_OBJECT
public:
    BatteryWorker();

signals:
    void adcReceived(QString val);

public slots:
    void getadc(int ch);

private:
    void saveAdcToFile(QString filename, QString val);
};

#endif // BATTERYWORKER_H
