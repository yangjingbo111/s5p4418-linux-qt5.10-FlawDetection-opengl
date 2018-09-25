#include "ascanfbo.h"
#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include "chartrenderer.h"

class AscanFboRenderer: public QQuickFramebufferObject::Renderer
{
public:
    AscanFboRenderer(){
        m_render.initialize();
    }

//    void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE{
//        m_window = item->window();
//        AscanFbo *i = static_cast<AscanFbo *>(item);
//        m_render.setNewData(i->getNewData());
//        m_render.setDataLength(i->dataLength());
//    }

    void render() Q_DECL_OVERRIDE{
        m_render.render();
//        m_window->resetOpenGLState();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE{
        QOpenGLFramebufferObjectFormat format;
        format.setSamples(4);
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        return new QOpenGLFramebufferObject(size, format);
    }

private:
    QQuickWindow *m_window;
    ChartRenderer m_render;
};

AscanFbo::AscanFbo(QQuickItem *parent)
    :QQuickFramebufferObject(parent)
{
    m_dataLength = 100;
//    timer.setInterval(40);
//    connect(&timer, SIGNAL(timeout()), this, SLOT(updateManual()));
}

QQuickFramebufferObject::Renderer *AscanFbo::createRenderer() const
{
    return new AscanFboRenderer();
}

void AscanFbo::deliverNewData(double data)
{
    m_newData = (float)data;
    updateManual();
}

float AscanFbo::getNewData()
{
    return m_newData;
}

int AscanFbo::dataLength()
{
    return m_dataLength;
}

void AscanFbo::setDataLength(int dataLength)
{
    if(m_dataLength != dataLength){
        m_dataLength = dataLength;
        update();
    }
}

void AscanFbo::updateManual()
{
    update();
}
