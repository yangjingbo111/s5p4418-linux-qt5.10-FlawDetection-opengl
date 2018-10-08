#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QFile>
#include <QThread>
#include "batteryworker.h"

class AppManager : public QObject
{
    Q_OBJECT
public:
    explicit AppManager(QObject *parent = nullptr);
    ~AppManager();

    Q_INVOKABLE void startApp(QString appname);
    Q_INVOKABLE void startApp(QString appname, bool running);
    Q_INVOKABLE void startSearchWifi();
    Q_INVOKABLE void connectToWifi(QString ssid);
    Q_INVOKABLE void connectToWifi(QString ssid, QString psk);
    Q_INVOKABLE void getip();
    Q_INVOKABLE void getadc(int ch);
    Q_INVOKABLE void writeHeader(QString filename);
    Q_INVOKABLE void writeFooter(QString filename);
    Q_INVOKABLE void downloadFileToUsbDisk(QString usbnode, QString srcfilename);
    Q_INVOKABLE bool checkUsbDisk();
    Q_INVOKABLE QString getUsbDiskNode();

    QList<QMap<QString, int>> listNetworks();
    QMap<QString, int> listNetworks_();
    QMap<QString, QString> getCurrentNetworkStatus();
    int getNetworkId(QString ssid);
    void selectNetwork(int networkid);
    void enableNetwork(int networkid);
    Q_INVOKABLE int  addNetwork();
    Q_INVOKABLE void  testQString(QString ssid, QString psk, int networkid);
    Q_INVOKABLE void setNetwork(QString ssid, QString psk, int networkid);

    void dhcp();

    void saveAdcToFile(QString filename, QString val);
//    void getIp();

signals:
    void searchResultChanged(QString res);
    void ipChanged(QString ip);
    void connectionStatusChanged(); //1 connected; 0 unconnected
    void adcReceived(QString val);

    void getadcSignal(int ch);
public slots:
    void appStarted();
    void appGotError();


private:
    QPROCESSORDETECTION_H

    QThread *m_thread;
    BatteryWorker *m_batteryWorker;
};

#endif // APPMANAGER_H
