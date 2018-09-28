#include "ascanfbo.h"

#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include "chartrenderer.h"

#include <QObject>
#include "ascanfborenderer.h"

AscanFbo::AscanFbo(QQuickItem *parent)
    :QQuickFramebufferObject(parent)
{
    m_rectificationType = 2; // default to plus rectification
    m_hardwareDraw = 0;      // default : disable hardware draw
}

QQuickFramebufferObject::Renderer *AscanFbo::createRenderer() const
{
    return new AscanFboRenderer();
}

void AscanFbo::recvData(QByteArray data)
{
//    if(m_hardwareDraw)return;

    m_data = data;
    combinePoints(data);
}

void AscanFbo::setrectificationType(int val)
{
    if(m_rectificationType != val){
        m_rectificationType = val;
    }
}

void AscanFbo::sethardwareDraw(int val)
{
    if(m_hardwareDraw != val){
        m_hardwareDraw = val;
    }
}

QByteArray AscanFbo::getNewData()
{
    return m_data;
}

int AscanFbo::hardwareDraw()
{
    return m_hardwareDraw;
}


int AscanFbo::getType()
{
    return m_rectificationType;
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
