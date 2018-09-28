#ifndef ASCANFBO_H
#define ASCANFBO_H

#include <QQuickFramebufferObject>
#include <QTimer>

class AscanFbo : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    AscanFbo(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const Q_DECL_OVERRIDE;

    Q_INVOKABLE void recvData(QByteArray data);
    Q_INVOKABLE void setrectificationType(int val);
    Q_INVOKABLE void sethardwareDraw(int val);
    QByteArray getNewData();
    int hardwareDraw();
    int getType();

private:
    void combinePoints(QByteArray data);

private:

    float m_newData;

    QByteArray m_data;
    int m_rectificationType;
    int m_hardwareDraw;

};

#endif // ASCANFBO_H
