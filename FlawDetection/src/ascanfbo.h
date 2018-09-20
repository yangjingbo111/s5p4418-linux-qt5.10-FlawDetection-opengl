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

    Q_INVOKABLE void deliverNewData(double data);
    float getNewData();

    Q_PROPERTY(int dataLength READ dataLength WRITE setDataLength NOTIFY dataLengthChanged)
    int dataLength();
    void setDataLength(int dataLength);

signals:
    void dataLengthChanged(int dataLength);

public slots:
    void updateManual();

private:
    QTimer timer;
    float m_newData;
    int m_dataLength;

};

#endif // ASCANFBO_H
