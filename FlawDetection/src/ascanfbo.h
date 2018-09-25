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
    QByteArray getNewData();

    Q_PROPERTY(int dataLength READ dataLength WRITE setDataLength NOTIFY dataLengthChanged)
    int dataLength();
    void setDataLength(int dataLength);
    int getType();

signals:
    void dataLengthChanged(int dataLength);

public slots:
    void updateManual();


private:
    void combinePoints(QByteArray data);

private:
    QTimer timer;
    float m_newData;
    int m_dataLength;

    QByteArray m_data;
    int m_rectificationType;

};

#endif // ASCANFBO_H
