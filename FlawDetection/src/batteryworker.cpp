#include "batteryworker.h"
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include "micros.h"
#include <QThread>

BatteryWorker::BatteryWorker()
{
    m_bat_timer = new QTimer(this);
    m_bat_timer->setInterval(3000);
    m_bat_timer->setSingleShot(false);
    connect(m_bat_timer, SIGNAL(timeout()), this, SLOT(getPwrSrc()), Qt::QueuedConnection);
    connect(m_bat_timer, SIGNAL(timeout()), this, SLOT(getChgStatus()), Qt::QueuedConnection);
    m_bat_timer->start();
    qDebug()<<__func__<<QThread::currentThreadId();
}

void BatteryWorker::getadc(int ch)
{
//    qDebug()<<__func__<<QThread::currentThreadId();
    QProcess adc_test;
    QStringList arg;
    arg << QString::number(ch);     // this is the adc channel argument
    adc_test.setProgram(ADC_TEST);
    adc_test.setArguments(arg);

    QObject::connect(&adc_test, &QProcess::readyReadStandardError, [&adc_test](){
       qDebug()<<adc_test.readAllStandardError();
    });
    QObject::connect(&adc_test, &QProcess::readyReadStandardOutput, [&adc_test, this](){
        QString result = adc_test.readAll();
        QStringList list = result.split("\n");
        foreach (QString e, list) {
            if(e.contains("=")){
                QString adc_val = e.split("=").at(1);
                qDebug().noquote()<<adc_val;
                emit adcReceived(adc_val);     //emit adc value to qml world

                // save to file
                saveAdcToFile("/opt/battery.txt", adc_val);
            }

        }

    });

    adc_test.start();
    adc_test.waitForFinished();
}

void BatteryWorker::getPwrSrc()
{
//    qDebug()<<__func__<<QThread::currentThreadId();
    QProcess gpio_test;
    QStringList arg;
    arg << "-s"<<"input"<<"-iGPIOE28";  // pwr src
    gpio_test.setProgram(GPIO_TEST);
    gpio_test.setArguments(arg);

    QObject::connect(&gpio_test, &QProcess::readyReadStandardError, [&gpio_test](){
       qDebug()<<gpio_test.readAllStandardError();
    });
    QObject::connect(&gpio_test, &QProcess::readyReadStandardOutput, [&gpio_test, this](){
        // GPIOE28:value 1
        // GPIOE28:value 0
        QString result = gpio_test.readAll();
        QStringList list = result.split(" ");
//        qDebug().noquote()<<list[1];
        emit pwrSrcChanged(list[1].toInt());
    });

    gpio_test.start();
    gpio_test.waitForFinished();
}

void BatteryWorker::getChgStatus()
{
//    qDebug()<<__func__<<QThread::currentThreadId();
    QProcess gpio_test;
    QStringList arg;
    arg << "-s"<<"input"<<"-iGPIOE27";  // charging status
    gpio_test.setProgram(GPIO_TEST);
    gpio_test.setArguments(arg);

    QObject::connect(&gpio_test, &QProcess::readyReadStandardError, [&gpio_test](){
       qDebug()<<gpio_test.readAllStandardError();
    });
    QObject::connect(&gpio_test, &QProcess::readyReadStandardOutput, [&gpio_test, this](){
        // GPIOE27:value 1
        // GPIOE27:value 0
        QString result = gpio_test.readAll();
        QStringList list = result.split(" ");
//        qDebug().noquote()<<list[1];
        emit chgStatusChanged(list[1].toInt());
    });

    gpio_test.start();
    gpio_test.waitForFinished();
}

void BatteryWorker::saveAdcToFile(QString filename, QString val)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        qDebug().noquote()<<filename<<"open failed";
        return;
    }

    file.write(val.toLatin1()+"\r\n");
    file.close();
}
