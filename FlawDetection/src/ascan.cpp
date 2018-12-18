#include "ascan.h"
#include <QBrush>
#include <QColor>
#include <QPen>

Ascan::Ascan(QQuickItem *parent)
    :QQuickPaintedItem (parent)
{
    w = 512;
    h = 400;
    m_rectificationType = 2; // default to plus rectification
    m_hardwareDraw = 0;      // default : disable hardware draw

    setRenderTarget(QQuickPaintedItem::Image);


//    m_timer = new QTimer(this);
//    m_timer->setTimerType(Qt::PreciseTimer);
//    m_timer->setInterval(16);
//    connect(m_timer, SIGNAL(timeout()), this, SLOT(test_timer()));
//    m_timer->start();
}

Ascan::~Ascan()
{

}

void Ascan::paint(QPainter *painter)
{
    // paint grid
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(Qt::gray));
    painter->setPen(pen);
    int x=0,y=0;
    while(y < h){
        while (x < w) {
            painter->drawPoint(x, y);
            x += 10;
        }
        x = 0;
        y += 40;
    }
    x = 0; y = 0;
    while(y < h){
        while (x < w + 1) {
            painter->drawPoint(x, y);
            x += 50;
        }
        x = 0;
        y += 10;
    }

//    if(m_hardwareDraw) return;

    // paint A scan
    painter->setPen(QPen(QColor(Qt::yellow)));
////    geneFakeData();
    if(m_rectificationType>0){
        painter->drawLines(m_points, w);


        if(1){
            QFile file("/opt/data-in-paint.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
                return;

            QTextStream out(&file);

            out << QString::number(m_points[1024].x(), 16).rightJustified(2).toUpper()<< " "<<QString::number(m_points[1024].y(), 16).rightJustified(2).toUpper();

            out<<"\r\n";    // packet seperate

            file.close();
        }


    }else if(m_rectificationType == 0){
        painter->drawPolyline(m_points, w);
    }


    // calculate and display fps
    static int frame_cnt=0;
    static int fps = 0;
    frame_cnt ++;
    static int time1,time2,time_1s = 0;
    time1 = QTime::currentTime().msecsSinceStartOfDay();


    time_1s += time1 - time2;
    if(time_1s > 1000){
        fps = frame_cnt * 1000 / time_1s;
        time_1s = 0;
        frame_cnt = 0;
//        qDebug()<<__func__<<"time = ";
    }

    time2 = QTime::currentTime().msecsSinceStartOfDay();


    static int f_max=1, f_min=1000;
    if(fps > 0){     // filter invalid data
        if(fps > f_max) f_max = fps;
        if(fps < f_min) f_min = fps;
    }

    painter->drawText(20,20,QString("FPS: ")+QString::number(fps)+QString(" max: ")+QString::number(f_max)+QString(" min: ")+QString::number(f_min));
//    t.restart();
    // end fps
}

void Ascan::recvData(QByteArray data)
{
//    qDebug()<<__func__<<data.mid(512, 16);
//    m_localData = data;
    combinePoints(data);
}

void Ascan::setrectificationType(int val)
{
    if(m_rectificationType != val){
        m_rectificationType = val;
    }
}

void Ascan::sethardwareDraw(int val)
{
    if(m_hardwareDraw != val){
        m_hardwareDraw = val;
    }
}


void Ascan::test_timer()
{
//    static int cnt=0;
//    QTime time = QTime::currentTime();
//    qsrand((uint)time.msec());
//    int k = randInt(10,100);
//    qDebug()<<__func__<<cnt++<<"   "<<k;
//    geneFakeData();

    update();

}

#include <math.h>
#define PI 3.141592
void Ascan::geneFakeData()
{

    int k;
    for(int i=1;i<=512;i++){

        m_points[i].setX(i);
        time = QTime::currentTime();
        qsrand((uint)time.msec());
        k = randInt(10,100);
        m_points[i].setY(k*sin(8*PI*((double)i/512))+180);
    }
}

void Ascan::combinePoints(QByteArray data)
{
    // ATTENTION: 400 is the window height[top left is the origin]
    if(m_rectificationType > 0) // 1 2 3
    {
        QVector<unsigned int> finalArr = generateFinalArray(data);
        // prepare the X values [p0.x = p1.x] [p2.x=p3.x]
        for(int i = 0; i< 1024; i+=2){
            m_points[i].setX(i/2);
            m_points[i+1].setX(i/2);
        }

        for(int i = 0; i< 1024; i++){

            if(finalArr.at(i) > 400){  // shape the peak and
                m_points[i].setY(0);
            }
            else {
                unsigned int tmp = finalArr.at(i);
                m_points[i].setY(399 - tmp);
            }
        }

        // test show the incremental data
        m_points[1024].setX(data[524]);
        m_points[1024].setY(data[525]);

    }
    else{ //radio rectification
        for(int i = 0; i< 512; i++){
            m_points[i].setX(i);
            if((unsigned char)data.at(i) <= 28){  // shape the peak and
                m_points[i].setY(399);
            }
            else if((unsigned char)data.at(i) > 228){  // shape the peak and
                m_points[i].setY(0);
            }
            else {
                unsigned int tmp = (unsigned char)data.at(i) - 28;
                tmp <<= 1;
                m_points[i].setY(399 - tmp);  // scale 2
            }
        }
    }
    update();
}

int Ascan::randInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

QVector<unsigned int> Ascan::generateFinalArray(QByteArray arr)
{
    QVector<unsigned int> array;
    QVector<unsigned int> finalArray;
    unsigned int tmp;
    for(int i = 0;i<512;i++){
        tmp = (unsigned int)arr.at(i);
        array.push_back(tmp<<1);
    }
    unsigned int goal;
    unsigned int cur, pre, next;
    for(int i = 0;i<512;i++){
        // append the peak value

        // borrow the idea from fpga-way
        if(i == 0){
            pre = 0;
            cur = array.at(i);
            next = array.at(i+1);
        }else if(i == 511){
            pre = array.at(i-1);
            cur = array.at(i);
            next = 0;
        }else{
            pre = array.at(i-1);
            cur = array.at(i);
            next = array.at(i+1);
        }

        // calc the downside value
        if(pre <= cur && cur <= next){
            goal = (pre == cur) ? pre : pre + 1;
        }else if(pre <= cur && cur >= next){
            if(pre <= next){
                goal = (pre == cur)?pre:pre+1;
            }else{
                goal = (next == cur)?next:next+1;
            }
        }else if(pre >= cur && cur <= next){
            goal = cur;
        }else{
            goal = (next == cur)?next:next+1;
        }

        // append the downside value
        finalArray.push_back(cur);
        finalArray.push_back(goal);

    }


    return finalArray;
}

