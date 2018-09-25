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

    void synchronize(QQuickFramebufferObject *item) Q_DECL_OVERRIDE{
        m_window = item->window();
        AscanFbo *i = static_cast<AscanFbo *>(item);
        m_render.setNewData(i->getNewData(), i->getType());
//        m_render.setDataLength(i->dataLength());
    }

    void render() Q_DECL_OVERRIDE{
        m_render.render();
//        m_window->resetOpenGLState();
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) Q_DECL_OVERRIDE{
        QOpenGLFramebufferObjectFormat format;
//        format.setSamples(2);
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

void AscanFbo::recvData(QByteArray data)
{
    m_data = data;
    combinePoints(data);
}

void AscanFbo::setrectificationType(int val)
{
    if(m_rectificationType != val){
        m_rectificationType = val;
    }
}

QByteArray AscanFbo::getNewData()
{
    return m_data;
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

int AscanFbo::getType()
{
    return m_rectificationType;
}

void AscanFbo::updateManual()
{
    update();
}

void AscanFbo::combinePoints(QByteArray data)
{
    // ATTENTION: 400 is the window height[top left is the origin]
    if(m_rectificationType > 0) // 1 2 3
    {
        for(int i = 0; i< 512; i++){
            if((unsigned char)data.at(i) > 200){  // shape the peak and
                m_data[i] = (char)200;
            }
        }
    }
    else{ //radio rectification
        for(int i = 0; i< 512; i++){
            if((unsigned char)data.at(i) <= 28){  // shape the peak and
                m_data[i] = 0;
            }
            else if((unsigned char)data.at(i) > 228){  // shape the peak and
                m_data[i] = 200;
            }
            else {
                m_data[i] = m_data[i]-28;;
            }
        }
    }

    update();
}
