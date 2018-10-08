#include "appmanager.h"
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include "micros.h"


AppManager::AppManager(QObject *parent) : QObject(parent)
{
//    connect()
    m_thread = new QThread;
    m_batteryWorker = new BatteryWorker;
    m_batteryWorker->moveToThread(m_thread);
    connect(this, SIGNAL(getadcSignal(int)), m_batteryWorker, SLOT(getadc(int)), Qt::QueuedConnection);
    connect(m_batteryWorker, SIGNAL(adcReceived(QString)), this, SIGNAL(adcReceived(QString)), Qt::QueuedConnection);

    m_thread->start();

}

AppManager::~AppManager()
{

}

void AppManager::startApp(QString appname)
{
//    QString program = "/path/to/Qt/examples/widgets/analogclock";
    QStringList arguments;
//    arguments << "-style" << "fusion";

//    QProcess myProcess;
//    myProcess.start(appname, arguments, QIODevice::ReadWrite);
    QProcess::startDetached(appname);
    qApp->quit();
}

void AppManager::startApp(QString appname, bool running)
{
    QStringList arguments;
//    arguments << "HUAWEIP20Pro" << "yangjingbo";
    arguments << "/opt/connect-wifi.sh";
    qDebug()<<"startApp is called: "+appname;
//    QProcess myProcess;
//    myProcess.start(appname, arguments, QIODevice::ReadWrite);
//    int res = QProcess::execute(appname);
    int res = QProcess::execute(appname,arguments);
    qDebug()<<"startApp is called: "+ QString(res);

}

void AppManager::startSearchWifi()
{
    QString appname(WPA_CLI);
    QList<QByteArray> commands = {"scan", "scan_results", "quit"};
    QListIterator<QByteArray> itr (commands);

    QStringList arg0, arg1, arg2;
    QProcess wpa_cli;
    wpa_cli.setProgram(appname);
    wpa_cli.setArguments(arg0);

    QObject::connect(&wpa_cli, &QProcess::readyReadStandardError, [&wpa_cli](){
       qDebug()<<wpa_cli.readAllStandardError();
    });
    QObject::connect(&wpa_cli, &QProcess::readyReadStandardOutput, [&wpa_cli, &itr, this](){
        QString result = wpa_cli.readAll();
        QStringList list = result.split("\n");
        foreach (QString e, list) {

            if(e.contains("[")){
    //            qDebug()<<e;
    //            QStringList l = e.split("\t");
    //            foreach (QString i, l) {
    //                qDebug()<<i;
    //            }
                emit searchResultChanged(e);
            }

        }

        qDebug().noquote()<<"Results:\n"<<result;
        if(itr.hasNext()){
            const QByteArray & command = itr.next();
            wpa_cli.write(command+"\n");
            qDebug()<< "command: " << command;
        }

    });


    wpa_cli.start();

    wpa_cli.waitForFinished();

    // test

//    listNetworks();
//    listNetworks_();
//    qDebug().noquote()<< getNetworkId("GULF-PC")<<getNetworkId("HUAWEIP20Pro");
//    getCurrentNetworkStatus();



}

void AppManager::connectToWifi(QString ssid)
{
    int networkid = getNetworkId(ssid);
    qDebug().noquote()<<networkid;
    selectNetwork(networkid);

    enableNetwork(networkid);

    dhcp();
}

void AppManager::connectToWifi(QString ssid, QString psk)
{
    int networkid = addNetwork();
    setNetwork(ssid, psk, networkid);
    selectNetwork(networkid);
    enableNetwork(networkid);
    dhcp();
    getip();
}

void AppManager::getip()
{
    QProcess ifconfig;
    QStringList arg;
    arg << "wlan0";
    ifconfig.setProgram(IFCONFIG);
    ifconfig.setArguments(arg);

    QObject::connect(&ifconfig, &QProcess::readyReadStandardError, [&ifconfig](){
       qDebug()<<ifconfig.readAllStandardError();
    });
    QObject::connect(&ifconfig, &QProcess::readyReadStandardOutput, [&ifconfig, this](){
        QString result = ifconfig.readAll();
        QStringList list = result.split("\n");
        foreach (QString e, list) {
            if(e.contains("inet addr:")){
                QString ip = e.split("Bcast").at(0).split(":").at(1).trimmed();

                emit ipChanged(ip);
            }

        }

    });

    ifconfig.start();
    ifconfig.waitForFinished();
}

void AppManager::getadc(int ch)
{
    emit getadcSignal(ch);
}

//void AppManager::getadc(int ch)
//{
//    QProcess adc_test;
//    QStringList arg;
//    arg << QString::number(ch);     // this is the adc channel argument
//    adc_test.setProgram(ADC_TEST);
//    adc_test.setArguments(arg);

//    QObject::connect(&adc_test, &QProcess::readyReadStandardError, [&adc_test](){
//       qDebug()<<adc_test.readAllStandardError();
//    });
//    QObject::connect(&adc_test, &QProcess::readyReadStandardOutput, [&adc_test, this](){
//        QString result = adc_test.readAll();
//        QStringList list = result.split("\n");
//        foreach (QString e, list) {
//            if(e.contains("=")){
//                QString adc_val = e.split("=").at(1);
//                qDebug().noquote()<<adc_val;
//                emit adcReceived(adc_val);     //emit adc value to qml world

//                // save to file
//                saveAdcToFile("/opt/battery.txt", adc_val);
//            }

//        }

//    });

//    adc_test.start();
//    adc_test.waitForFinished();
//}

QList<QMap<QString, int>> AppManager::listNetworks()
{
    QList<QMap<QString, int> > networkList;
    QProcess wpa_cli;
    QStringList arg;
    arg << "list_networks";
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

    foreach (QString e, list) {
//        qDebug().noquote()<<e;
        /*
        network id / ssid / bssid / flags
         0	GULF-PC	any	[CURRENT]
        */

        if(e.contains("[")){
            QMap<QString, int> m;
            m[e.split("\t").at(1)] = e.split("\t").at(0).toInt();
            networkList.append(m);

        }
    }

    for (int i =0; i < networkList.size();i++) {
        qDebug().noquote()<<networkList.at(i).firstKey()<<networkList.at(i).first();
    }

    return networkList;
}

QMap<QString, int> AppManager::listNetworks_()
{
    QMap<QString, int> networkMap;
    QProcess wpa_cli;
    QStringList arg;
    arg << "list_networks";
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

    foreach (QString e, list) {
//        qDebug().noquote()<<e;
        /*
        network id / ssid / bssid / flags
         0	GULF-PC	any	[CURRENT]
        */

        if(e.contains("[")){
            networkMap[e.split("\t").at(1)] = e.split("\t").at(0).toInt();
        }
    }

    QMapIterator<QString, int> i(networkMap);
    while (i.hasNext()) {
        i.next();
        qDebug().noquote() << i.key() << ": " << i.value();
    }



    return networkMap;
}

QMap<QString, QString> AppManager::getCurrentNetworkStatus()
{
    QMap<QString, QString> status;
    QProcess wpa_cli;
    QStringList arg;
    arg << "status";
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

    foreach (QString e, list) {
//        qDebug().noquote()<<e;
        /*
Selected interface 'wlan0'
bssid=08:10:79:18:00:42
freq=0
ssid=GULF-PC
id=0
mode=station
pairwise_cipher=CCMP
group_cipher=CCMP
key_mgmt=WPA2-PSK
wpa_state=COMPLETED
ip_address=192.168.137.81
address=cc:b8:a8:27:1c:66
uuid=05a45e4b-419e-50ac-b042-1cfafc3a1786
        */

        if(e.contains("=")){
            status[e.split("=").at(0)] = e.split("=").at(1);
        }
    }

    QMapIterator<QString, QString> i(status);

    // traverse
//    while (i.hasNext()) {
//        i.next();
//        qDebug().noquote() << i.key() << ": " << i.value();
//    }

    return status;
}

int AppManager::getNetworkId(QString ssid)
{
    QMap<QString, int> networks = listNetworks_();
    int value = networks.value(ssid, -1);
    return value;
}

void AppManager::selectNetwork(int networkid)
{
    QMap<QString, QString> status;
    QProcess wpa_cli;
    QStringList arg;
    arg << "select_network"<<QString::number(networkid);
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

    foreach (QString e, list) {
        qDebug().noquote()<<e;
        /*
        network id / ssid / bssid / flags
         0	GULF-PC	any	[CURRENT]
        */
    }


}

void AppManager::enableNetwork(int networkid)
{
    QMap<QString, QString> status;
    QProcess wpa_cli;
    QStringList arg;
    arg << "enable_network"<<QString::number(networkid);
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

    foreach (QString e, list) {
        qDebug().noquote()<<e;
        /*
        network id / ssid / bssid / flags
         0	GULF-PC	any	[CURRENT]
        */
    }
}

int AppManager::addNetwork()
{
    QProcess wpa_cli;
    QStringList arg;
    arg << "add_network";
    wpa_cli.setProgram(WPA_CLI);
    wpa_cli.setArguments(arg);
    wpa_cli.start();
    wpa_cli.waitForStarted();
    wpa_cli.waitForReadyRead();
    QString res = wpa_cli.readAll();
    QStringList list = res.split("\n");
    wpa_cli.waitForFinished();

//    qDebug().noquote()<<"add network id:"<<list.at(1);
    return list.at(1).toInt();
}

void AppManager::testQString(QString ssid, QString psk, int networkid)
{
    QString a = QString("set_network %1 ssid \"%2\"").arg(QString::number(networkid), QString(ssid));
    QString b = QString("set_network %1 psk \"%2\"").arg(QString::number(networkid), QString(psk));
    qDebug().noquote()<<a;
    qDebug().noquote()<<b;
}




void AppManager::setNetwork(QString ssid, QString psk, int networkid)
{
    QString appname(WPA_CLI);
    QList<QString> commands = {QString("set_network %1 ssid \"%2\"\n").arg(QString::number(networkid), QString(ssid)),
                              QString("set_network %1 psk \"%2\"\n").arg(QString::number(networkid), QString(psk)),
                              QString("quit\n")};
    QListIterator<QString> itr (commands);

    QStringList arg0;
    QProcess wpa_cli;
    wpa_cli.setProgram(appname);
    wpa_cli.setArguments(arg0); // interactive mode, start wpa_cli with none argument

    QObject::connect(&wpa_cli, &QProcess::readyReadStandardError, [&wpa_cli](){
       qDebug()<<wpa_cli.readAllStandardError();
    });
    QObject::connect(&wpa_cli, &QProcess::readyReadStandardOutput, [&wpa_cli, &itr, this](){
        QString result = wpa_cli.readAll();
        QStringList list = result.split("\n");


        qDebug().noquote()<<"Results:\n"<<result;
        if(itr.hasNext()){
            const QString & command = itr.next();

            wpa_cli.write(command.toLocal8Bit());
            qDebug()<< "command: " << command;
        }

    });


    wpa_cli.start();

    wpa_cli.waitForFinished();
}

/**
 * @brief AppManager::dhcp
 * @details use udhcpc -iwlan0 to get dynamic ip address
 */
void AppManager::dhcp()
{
    QProcess udhcpc;
    QStringList arg;
    arg << "-iwlan0";
    udhcpc.setProgram(UDHCPC);
    udhcpc.setArguments(arg);
    udhcpc.start();
    udhcpc.waitForStarted();
    udhcpc.waitForReadyRead();
    QString res = udhcpc.readAll();
    QStringList list = res.split("\n");

    udhcpc.waitForFinished();

    foreach (QString e, list) {
        qDebug().noquote()<<e;
        /*
        network id / ssid / bssid / flags
         0	GULF-PC	any	[CURRENT]
        */
    }
}

void AppManager::writeHeader(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        qDebug().noquote()<<filename<<"open failed";
        return;
    }

    file.write("-----------------start-----------------\r\n");
    file.close();
}

void AppManager::writeFooter(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        qDebug().noquote()<<filename<<"open failed";
        return;
    }

    file.write("-----------------stop-----------------\r\n");
    file.close();
}

/**
 * @brief AppManager::downloadFileToUsbDisk
 * @param srcfilename
 * @details usbdisk will be enumrate at /dev/sda1
 */
void AppManager::downloadFileToUsbDisk(QString usbnode, QString srcfilename)
{
//    qDebug().noquote()<<usbnode<<srcfilename;
//    return;
    //mount /dev/sdb1 (sometime /dev/sda1) to /mnt
    QProcess mount;
    QStringList arg;
    arg << "/dev/"+usbnode<<"/mnt";     // this is the adc channel argument
    mount.setProgram(MOUNT);
    mount.setArguments(arg);

    mount.start();
    mount.waitForStarted();
    mount.waitForFinished();

    //download file to the root directory of usbdisk
    QProcess cp;
    QStringList arg1;
    arg1 << srcfilename<<"/mnt/";
    cp.setProgram(CP);
    cp.setArguments(arg1);
    cp.start();
    cp.waitForStarted();
    cp.waitForFinished();


    //umount /mnt
    QProcess umount;
    QStringList arg2;
    arg2 << "/mnt";
    umount.setProgram(UMOUNT);
    umount.setArguments(arg2);
    umount.start();
    umount.waitForStarted();
    umount.waitForFinished();
}


bool AppManager::checkUsbDisk()
{

    //"No such file"
}

/**
 * @brief AppManager::getUsbDiskNode
 * @return /dev/sd*
 */
QString AppManager::getUsbDiskNode()
{
    QString res = "no usb";

    QProcess ls;
    QStringList arg;
    arg << "/dev";
    ls.setProgram(LS);
    ls.setArguments(arg);

    QObject::connect(&ls, &QProcess::readyReadStandardError, [&ls](){
       qDebug()<<ls.readAllStandardError();
    });
    QObject::connect(&ls, &QProcess::readyReadStandardOutput, [&ls, this,&res](){
        QString result = ls.readAll();
        QStringList list = result.split("\n");
        foreach (QString e, list) {
            if(e.contains("sd")){
                qDebug().noquote()<<e;
                res = e;
//                emit ipChanged(ip);
            }

        }

    });

    ls.start();
    ls.waitForFinished();

    return res;
}

void AppManager::saveAdcToFile(QString filename, QString val)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        qDebug().noquote()<<filename<<"open failed";
        return;
    }

    file.write(val.toLatin1()+"\r\n");
    file.close();
}

//void AppManager::getIp()
//{

//}

// worked version
//void AppManager::startSearchWifi()
//{
//    QString appname("/usr/sbin/wpa_cli");
//    QStringList arg1, arg2;
//    arg1 << "scan";
//    QProcess wpa_cli;
//    wpa_cli.start(appname, arg1, QIODevice::ReadWrite);
//    wpa_cli.waitForStarted();
//    wpa_cli.waitForReadyRead();

//    QString res = wpa_cli.readAll();
//    QStringList list = res.split("\n");
//    wpa_cli.waitForFinished();

//    foreach (QString e, list) {
//        qDebug()<<e;
//    }
//    arg2 << "scan_results";
//    wpa_cli.start(appname, arg2, QIODevice::ReadWrite);
//    wpa_cli.waitForStarted();
//    wpa_cli.waitForReadyRead();
//    res = wpa_cli.readAll();
//    list = res.split("\n");

//    struct WifiEntry wifinode;

//    foreach (QString e, list) {

//        if(e.contains("[")){
////            qDebug()<<e;
////            QStringList l = e.split("\t");
////            foreach (QString i, l) {
////                qDebug()<<i;
////            }
//            emit searchResultChanged(e);
//        }

//    }
////    emit searchResultChanged(res);
//    wpa_cli.waitForFinished();
//}

void AppManager::appStarted()
{

}

void AppManager::appGotError()
{

}
