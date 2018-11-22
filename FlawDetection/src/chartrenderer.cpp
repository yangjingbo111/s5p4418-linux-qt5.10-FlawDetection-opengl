#include "chartrenderer.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>



#include <QDebug>
#include <QImage>
#include <QString>
#include <QtMath>

#define MAX_CHAR        128//最大字符数


ChartRenderer::ChartRenderer(QObject *parent)
    :QObject(parent),
//      m_vao(new QOpenGLVertexArrayObject),
      m_vvbo(QOpenGLBuffer::VertexBuffer),
      m_data_vbo(QOpenGLBuffer::VertexBuffer)
//      m_data_vbo2(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
//      m_vcbo(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
{
    m_hardwareDraw = 0;
    init();
//    m_isDoubleLines = false;
//    m_dataLength = 100;
//    m_data_gen_timer.setInterval(200);
//    connect(&m_data_gen_timer, SIGNAL(timeout()), this, SLOT(genNewData()));
//    m_data_gen_timer.start();

//    QTime time = QTime::currentTime();
//    qsrand((uint)time.msec());

    m_vvbo.create();
    m_data_vbo.create();
}

ChartRenderer::~ChartRenderer()
{
//    invalidate();
    m_vvbo.destroy();
    m_data_vbo.destroy();
}

/**
  背景栅格 5条横线[10个点]
  **/
QVector3D c_grids_vetices[20] = {
    //vertex position
    //横线
    QVector3D(-1.0f, -1.0f, .0f),
    QVector3D(1.0f, -1.0f, .0f),

    QVector3D(-1.0f, -0.5f, .0f),
    QVector3D(1.0f, -0.5f, .0f),

    QVector3D(-1.0f, 0.0f,  .0f),
    QVector3D(1.0f, 0.0f,  .0f),

    QVector3D(-1.0f, 0.5f,  .0f),
    QVector3D(1.0f, 0.5f,  .0f),

    QVector3D(-1.0f, 1.0f,  .0f),
    QVector3D(1.0f, 1.0f,  .0f),



    //竖线
    QVector3D(-1.0f, 1.0f,  .0f),
    QVector3D(-1.0f, -1.0f,  .0f),

    QVector3D(-0.5f, 1.0f,  .0f),
    QVector3D(-0.5f, -1.0f,  .0f),

    QVector3D(0.0f, 1.0f,  .0f),
    QVector3D(0.0f, -1.0f,  .0f),

    QVector3D(0.5f, 1.0f,  .0f),
    QVector3D(0.5f, -1.0f,  .0f),

    QVector3D(1.0f, 1.0f,  .0f),
    QVector3D(1.0f, -1.0f,  .0f),

};


void ChartRenderer::initialize()
{
//    m_window =
//    createGeometry();

///////////////////////////////////////////////////////////////////////////

    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    m_vao.bind();

    if (!program1.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/shaders/vshader.glsl"))
        qFatal("Vertex shader compilation failed");
    if (!program1.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/shaders/fshader.glsl"))
        qFatal("Fragment shader compilation failed");
    if (!program1.link())
        qFatal("Shader program not linked");

    program1.bind();

    m_vvbo.bind();
    m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo.allocate(c_grids_vetices, (20)*sizeof(QVector3D));
    program1.enableAttributeArray("inPosition");
    program1.setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);

//    m_data_vbo->create();
//    m_data_vbo2->create();

//    m_vcbo->create();
//    m_vcbo->bind();
//    m_vcbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);

    m_vao.release();
}

void ChartRenderer::render()
{

    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();


    GLint viewportSize[4];
    functions->glGetIntegerv(GL_VIEWPORT, viewportSize);

    functions->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    functions->glEnable(GL_TEXTURE_2D);
    functions->glDisable(GL_DEPTH_TEST);
    functions->glEnable(GL_BLEND);
    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program1.bind();


    float alphaUniform=1.0f;
    float rUniform=0.4f;
    float gUniform=0.4f;
    float bUniform=0.4f;

    program1.setUniformValue("alpha", alphaUniform);
    program1.setUniformValue("r", rUniform);
    program1.setUniformValue("g", gUniform);
    program1.setUniformValue("b", bUniform);


    m_vvbo.bind();
    m_vao.bind();
    program1.enableAttributeArray("inPosition");
    program1.setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);
    functions->glDrawArrays(GL_LINES, 0, 20);



    //画线1
//    genNewData();//产生数据
    if(m_hardwareDraw){
        return;
    }
    m_data_vbo.bind();
    m_data_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_data_vbo.allocate(m_data_vetices, (DATA_LENGTH)*sizeof(QVector3D));

    program1.setUniformValue("r", 0.0f);
    program1.setUniformValue("g", 1.0f);
    program1.setUniformValue("b", 0.0f);
    program1.enableAttributeArray("inPosition");
    program1.setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);
    if(m_rectificationType > 0){
        functions->glDrawArrays(GL_LINES, 0, DATA_LENGTH);
    }else if(m_rectificationType == 0){
        functions->glDrawArrays(GL_LINE_STRIP, 0, DATA_LENGTH/2);
    }


    m_data_vbo.release();


}


//void ChartRenderer::invalidate()
//{
//    m_shaderProgram.reset();
//    m_vvbo->destroy();
//    m_vcbo->destroy();
//    m_vao->destroy();
//    m_data_vbo->destroy();
//    m_data_vbo2->destroy();
//}

void ChartRenderer::setNewData(QByteArray newData, int type)//note:newData 在绘制之前必须要映射到（-1，1）上，或直接在qml端进行映射
{
    m_rectificationType = type;

    if(newData.length() == 0)return;

    initVertices(type);
    if(type > 0){
        QVector<unsigned int> finalArr = generateFinalArray(newData);

        for(int i = 0; i< DATA_LENGTH; i++){

            if(finalArr.at(i) >= 400){  // shape the peak and
                m_data_vetices[i].setY(-1.0f);
            }
            else {
                //int tmp = finalArr.at(i) - 100;
                //m_data_vetices[i].setY(/*399 - tmp*/ -2.0f*(tmp)/201);  /*// scale 2 !! 201 is important
                unsigned int tmp = finalArr.at(i);
                m_data_vetices[i].setY(/*399 - tmp*/ (200-tmp)/200.0f);
            }
        }
    }
    else{
        for(int i=0;i<DATA_LENGTH/2;i++)
        {
            if((unsigned char)newData.at(i) <= 28){  // shape the peak and
                m_data_vetices[i].setY(1.0f);
            }
            else if((unsigned char)newData.at(i) > 228){  // shape the peak and
                m_data_vetices[i].setY(-1.0f);
            }
            else {
//                int tmp = newData.at(i) - 128; // wrong
                int tmp = newData.at(i) - 100;
                m_data_vetices[i].setY(/*399 - tmp*/ -2.0f*tmp/201);  // scale 2
            }
        }
    }


}

void ChartRenderer::setHardwareDraw(int val)
{
    if(m_hardwareDraw != val){
        m_hardwareDraw = val;
    }
}



//void ChartRenderer::setDataLength(int dataLength)
//{
//    m_dataLength = dataLength;
//}



void ChartRenderer::genNewData()
{
    QVector3D data_tmp[DATA_LENGTH];
    static float x =0.0f;
    static float distance = 2.0f * PI / DATA_LENGTH;
    float newData = (float)randInt(-10,10)/10;
    float y = newData*sin(x);
    float tx;
    tx = x;
    tx -= PI;
    tx /= PI;
    data_tmp[0].setX(tx);
    data_tmp[0].setY(y);
    data_tmp[0].setZ(.0f);
    x += distance;
    if(x>2.0f * PI)x=0.0f;
    memcpy(&data_tmp[1], m_data_vetices, (DATA_LENGTH-1)*sizeof(QVector3D));
    memcpy(m_data_vetices, data_tmp, (DATA_LENGTH)*sizeof(QVector3D));


}



/**
 * @brief ChartRenderer::init
 * @note 初始化数组
 */
void ChartRenderer::init()
{
    int i;

    //wang
    float x = -1.0f;
    static float distance = 2.0f/512;  // distribute 512 points to x axis[length is 2]
    float tx;
    for(i=0; i<DATA_LENGTH; i++, x+=distance)
    {
        tx = x;
        m_data_vetices[i].setX(tx);
        m_data_vetices[i].setY(0.0f);
        m_data_vetices[i].setZ(0.0f);
    }

    //wang


}

void ChartRenderer::initVertices(int type)
{
    float x = -1.0f;
    static float distance = 2.0f/511;  // distribute 512 points to x axis[length is 2]
    float tx;
    if(type > 0){
        for(int i=0; i<DATA_LENGTH; i+=2, x+=distance)
        {
            tx = x;
            m_data_vetices[i].setX(tx);
            m_data_vetices[i+1].setX(tx);
            m_data_vetices[i].setY(0.0f);
            m_data_vetices[i+1].setY(0.0f);
            m_data_vetices[i].setZ(0.0f);
            m_data_vetices[i+1].setZ(0.0f);
        }
    }else if(type == 0){
        for(int i=0; i<DATA_LENGTH/2; i++, x+=distance)
        {
            tx = x;
            m_data_vetices[i].setX(tx);
            m_data_vetices[i].setY(0.0f);
            m_data_vetices[i].setZ(0.0f);
        }
    }


}

int ChartRenderer::randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

QVector<unsigned int> ChartRenderer::generateFinalArray(QByteArray arr)
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


