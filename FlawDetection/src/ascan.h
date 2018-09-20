#ifndef ASCAN_H
#define ASCAN_H

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QPoint>
#include <QTime>
#include <QTimer>

class Ascan : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit Ascan(QQuickItem *parent = nullptr);
    ~Ascan();
    void paint(QPainter *painter);
    Q_INVOKABLE void recvData(QByteArray data);
    Q_INVOKABLE void setrectificationType(int val);
public slots:
    void test_timer();
private:
    void geneFakeData();
    void combinePoints(QByteArray data);
    int randInt(int low, int high);
private:
    int w;
    int h;
    QPoint m_points[512];
    QByteArray m_localData;
    QTime time;
    int m_rectificationType;

    QTimer *m_timer;
};

#endif // ASCAN_H
