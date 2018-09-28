#ifndef ASCANFBORENDERER_H
#define ASCANFBORENDERER_H

#include <QQuickFramebufferObject>
#include <QQuickWindow>
#include "chartrenderer.h"
#include <QObject>


class AscanFboRenderer: public QObject, public QQuickFramebufferObject::Renderer
{
    Q_OBJECT
public:
    explicit AscanFboRenderer();
    ~AscanFboRenderer();
    void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE;

public slots:
    void updateManual();
private:
    QQuickWindow *m_window;
    ChartRenderer m_render;


};

#endif // ASCANFBORENDERER_H
