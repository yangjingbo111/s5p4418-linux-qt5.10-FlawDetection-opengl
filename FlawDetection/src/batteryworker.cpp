#include "batteryworker.h"
#include <QProcess>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include "micros.h"

BatteryWorker::BatteryWorker()
{

}

void BatteryWorker::getadc(int ch)
{
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
