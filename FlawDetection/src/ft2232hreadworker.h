#ifndef FT2232HREADWORKER_H
#define FT2232HREADWORKER_H

#include <QObject>
#include "ftd2xx.h"
#include <QTime>
#include <QTimer>
#include <QList>

#define MAX_BUF_SIZE 16
#define PACKET_SIZE 1020    // each packet has 1020 bytes, among them, 512 bytes are echoing data

class Ft2232HReadWorker : public QObject
{
    Q_OBJECT
public:
    explicit Ft2232HReadWorker(FT_HANDLE ftHandle, QObject *parent = nullptr);
    ~Ft2232HReadWorker();

    void readLoop();
    void readFifo();
    void dispatchPacket();
    void wrrepeatFreq(int val);

    // special case [cooperate with while loop]


signals:
    void dataReady(QByteArray data);

public slots:
    void initWorker();
private:
    FT_HANDLE       m_ftHandle;
    int             portNumber = 0;
    bool m_isWorking;   // indicate ft2232h working status, in order to control the main loop

    UCHAR m_rdBuf[PACKET_SIZE];
    QTime time;
    QTimer *m_timer;
    int m_repeatFreq;     // repeat frequency
    QList<QByteArray> m_list;
    UCHAR m_dataPool[PACKET_SIZE];

};

#endif // FT2232HREADWORKER_H
