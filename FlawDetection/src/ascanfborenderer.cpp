#include "ascanfborenderer.h"
#include "ascanfbo.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

AscanFboRenderer::AscanFboRenderer()
{
    m_render.initialize();

}

AscanFboRenderer::~AscanFboRenderer()
{

}

void AscanFboRenderer::synchronize(QQuickFramebufferObject *item) {
    m_window = item->window();
//    connect(m_window, SIGNAL(frameSwapped()), this, SLOT(updateManual()));
    AscanFbo *i = static_cast<AscanFbo *>(item);
    m_render.setNewData(i->getNewData(), i->getType());
    m_render.setHardwareDraw(i->hardwareDraw());

}

void AscanFboRenderer::render() {
    m_render.render();
    m_window->resetOpenGLState();
//        update();
//        m_window->update();
}

QOpenGLFramebufferObject *AscanFboRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setSamples(4);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void AscanFboRenderer::updateManual()
{
//    update();
//    qDebug()<<__func__;
}
