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


ChartRenderer::ChartRenderer()
//    :QObject(parent),
//      m_vao(new QOpenGLVertexArrayObject),
//      m_vvbo(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
//      m_data_vbo(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
//      m_data_vbo2(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
//      m_vcbo(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
{
//    init();
//    m_isDoubleLines = false;
//    m_dataLength = 100;
//    m_data_gen_timer.setInterval(200);
//    connect(&m_data_gen_timer, SIGNAL(timeout()), this, SLOT(genNewData()));
//    m_data_gen_timer.start();

//    QTime time = QTime::currentTime();
//    qsrand((uint)time.msec());
}

ChartRenderer::~ChartRenderer()
{
//    invalidate();
}

float c_vertices[] = {
    //vertex position
    -0.5f, -0.5f, .0f,
     0.5f,-0.5f, .0f,
    0.5f, 0.5f,  .0f,
    -0.5f, -0.5f, .0f,
    0.5f, 0.5f,  .0f,
    -0.5f,0.5f,  .0f,
};

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





//    QVector3D(-1.1f,0.9f,0.0f),
//    QVector3D(-1.0f,1.1f,0.0f),

};


void ChartRenderer::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";



    if (!program1.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/assets/vshader.glsl"))
        qFatal("Vertex shader compilation failed");
    if (!program1.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/assets/fshader.glsl"))
        qFatal("Fragment shader compilation failed");
    if (!program1.link())
        qFatal("Shader program not linked");



//    program1.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc1);
//    program1.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc1);
//    program1.link();

    vertexAttr1 = program1.attributeLocation("vertex");
    normalAttr1 = program1.attributeLocation("normal");
    matrixUniform1 = program1.uniformLocation("matrix");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_fAngle = 0;
    m_fScale = 1;
    createGeometry();






//    if(m_vao->isCreated())
//        return;

//    if(!m_vao->create())
//        qFatal("unable to create vao");

//    m_vao->bind();
//    m_shaderProgram.reset(new QOpenGLShaderProgram);

//    if (!m_shaderProgram->create())
//        qFatal("Unable to create shader program");
//    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/assets/chart.vert"))
//        qFatal("Vertex shader compilation failed");
//    if (!m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/assets/chart.frag"))
//        qFatal("Fragment shader compilation failed");
//    if (!m_shaderProgram->link())
//        qFatal("Shader program not linked");

//    m_shaderProgram->bind();

//    m_vvbo->create();
//    m_vvbo->bind();
//    m_vvbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
//    m_vvbo->allocate(c_grids_vetices, (20)*sizeof(QVector3D));
//    m_shaderProgram->enableAttributeArray("inPosition");
//    m_shaderProgram->setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);

//    m_data_vbo->create();
//    m_data_vbo2->create();
////    m_data_vbo->bind();
////    m_data_vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
////    m_data_vbo->allocate(m_data_vetices, (DATA_LENGTH)*sizeof(QVector3D));

//    m_vcbo->create();
//    m_vcbo->bind();
//    m_vcbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
////    m_vcbo->allocate(texCoord, (12)*sizeof(float));
////    m_shaderProgram->enableAttributeArray("inCoord");
////    m_shaderProgram->setAttributeBuffer("inCoord", GL_FLOAT, 0, 2);

//    m_vao->release();
}

void ChartRenderer::render()
{
    glDepthMask(true);

    glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 modelview;
    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(m_fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);

    program1.bind();
    program1.setUniformValue(matrixUniform1, modelview);
    paintQtLogo();
    program1.release();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_fAngle += 1.0f;











//    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();



//    QMatrix4x4 modelMatrix;
//    QMatrix4x4 viewMatrix;
//    QMatrix4x4 projectionMatrix;



//    modelMatrix.rotate(0, 0, 1, 0);
//    const QVector3D eyePosition(0,0,1);

//    QVector3D upVector = QVector3D(0, 1, 0);

//    viewMatrix.lookAt(eyePosition * /*m_distance*/4, //the position of your camera, in world space
//                      QVector3D(0, 0, 0),       //where you want to look at, in world space
//                      upVector);                //probably vec3(0,1,0), but (0,-1,0)would make you looking upside-down

//    GLint viewportSize[4];
//    functions->glGetIntegerv(GL_VIEWPORT, viewportSize);


//    projectionMatrix.perspective(45, // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
//                                 float(viewportSize[2]) / viewportSize[3], // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
//                                 0.01,  // Near clipping plane. Keep as big as possible, or you'll get precision issues.
//                                 1000);        // Far clipping plane. Keep as little as possible.

//    functions->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//    functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////    functions->glDepthFunc(GL_LESS);//
//    functions->glEnable(GL_TEXTURE_2D);
//    //functions->glDisable(GL_LIGHTING);
//    functions->glDisable(GL_DEPTH_TEST);
//    functions->glEnable(GL_BLEND);
//    functions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    m_shaderProgram->bind();


//    modelMatrix.scale(1.0,0.6,1.0);

//    float alphaUniform=1.0f;
//    float rUniform=0.4f;
//    float gUniform=0.4f;
//    float bUniform=0.4f;
//    m_shaderProgram->setUniformValue("modelMatrix", modelMatrix);
//    m_shaderProgram->setUniformValue("viewMatrix", viewMatrix);
//    m_shaderProgram->setUniformValue("projectionMatrix", projectionMatrix);
//    m_shaderProgram->setUniformValue("alpha", alphaUniform);
//    m_shaderProgram->setUniformValue("r", rUniform);
//    m_shaderProgram->setUniformValue("g", gUniform);
//    m_shaderProgram->setUniformValue("b", bUniform);


//    m_vvbo->bind();
//    m_vao->bind();
//    m_shaderProgram->enableAttributeArray("inPosition");
//    m_shaderProgram->setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);
//    functions->glDrawArrays(GL_LINES, 0, 20);



//    //画线1
////    genNewData();//产生数据
//    m_data_vbo->bind();
//    m_data_vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
//    m_data_vbo->allocate(m_data_vetices, (DATA_LENGTH)*sizeof(QVector3D));

//    m_shaderProgram->setUniformValue("r", 1.0f);
//    m_shaderProgram->setUniformValue("g", 1.0f);
//    m_shaderProgram->setUniformValue("b", 0.0f);
//    m_shaderProgram->enableAttributeArray("inPosition");
//    m_shaderProgram->setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);
//    functions->glDrawArrays(GL_LINE_STRIP, 0, 100);


//    //画线2
//    m_data_vbo2->bind();
//    m_data_vbo2->setUsagePattern(QOpenGLBuffer::DynamicDraw);
//    m_data_vbo2->allocate(m_data_vetices2, (DATA_LENGTH)*sizeof(QVector3D));

//    m_shaderProgram->setUniformValue("r", 0.0f);
//    m_shaderProgram->setUniformValue("g", 0.0f);
//    m_shaderProgram->setUniformValue("b", 1.0f);
//    m_shaderProgram->enableAttributeArray("inPosition");
//    m_shaderProgram->setAttributeBuffer("inPosition", GL_FLOAT, 0, 3);


//    if(m_isDoubleLines)
//    {
//        functions->glDrawArrays(GL_LINE_STRIP, 0, 100);
//    }
//    m_data_vbo->release();
    //    m_data_vbo2->release();
}

void ChartRenderer::paintQtLogo()
{
    program1.enableAttributeArray(normalAttr1);
    program1.enableAttributeArray(vertexAttr1);
    program1.setAttributeArray(vertexAttr1, vertices.constData());
    program1.setAttributeArray(normalAttr1, normals.constData());
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    program1.disableAttributeArray(normalAttr1);
    program1.disableAttributeArray(vertexAttr1);
}

void ChartRenderer::createGeometry()
{
    vertices.clear();
    normals.clear();

    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = M_PI;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    for (int i = 0;i < vertices.size();i++)
        vertices[i] *= 2.0f;
}

void ChartRenderer::quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    vertices << QVector3D(x1, y1, -0.05f);
    vertices << QVector3D(x2, y2, -0.05f);
    vertices << QVector3D(x4, y4, -0.05f);

    vertices << QVector3D(x3, y3, -0.05f);
    vertices << QVector3D(x4, y4, -0.05f);
    vertices << QVector3D(x2, y2, -0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;

    vertices << QVector3D(x4, y4, 0.05f);
    vertices << QVector3D(x2, y2, 0.05f);
    vertices << QVector3D(x1, y1, 0.05f);

    vertices << QVector3D(x2, y2, 0.05f);
    vertices << QVector3D(x4, y4, 0.05f);
    vertices << QVector3D(x3, y3, 0.05f);

    n = QVector3D::normal
        (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
}

void ChartRenderer::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    vertices << QVector3D(x1, y1, +0.05f);
    vertices << QVector3D(x2, y2, +0.05f);
    vertices << QVector3D(x1, y1, -0.05f);

    vertices << QVector3D(x2, y2, -0.05f);
    vertices << QVector3D(x1, y1, -0.05f);
    vertices << QVector3D(x2, y2, +0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    normals << n;
    normals << n;
    normals << n;

    normals << n;
    normals << n;
    normals << n;
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

//void ChartRenderer::setNewData(float newData)//note:newData 在绘制之前必须要映射到（-1，1）上，或直接在qml端进行映射
//{
//    qDebug()<<"newData in opengl = "<<newData;
//    qDebug()<<"m_data_vetices[0] 1111111 = "<<m_data_vetices[99];

//    float x = -1.0f;
//    static float distance = 0.02f;
//    float tx;
//    for(int i=0;i<DATA_LENGTH-1;i++,x+=distance)
//    {
//        tx = x;
//        m_data_vetices[i].setX(tx);
//        m_data_vetices[i].setY(m_data_vetices[i+1].y());
//        m_data_vetices[i].setZ(0.0f);
//        m_data_vetices2[i].setX(tx);
//        m_data_vetices2[i].setY(m_data_vetices2[i+1].y());
//        m_data_vetices2[i].setZ(0.0f);
//    }
//    m_data_vetices[DATA_LENGTH-1].setX(1.0);
//    m_data_vetices[DATA_LENGTH-1].setY(newData);
//    m_data_vetices[DATA_LENGTH-1].setZ(0.0);

//    m_data_vetices2[DATA_LENGTH-1].setX(1.0);
//    m_data_vetices2[DATA_LENGTH-1].setY(newData+0.3);
//    m_data_vetices2[DATA_LENGTH-1].setZ(0.0);
//}

//void ChartRenderer::setIsDoubleLines(bool isDoubleLines)
//{
//    m_isDoubleLines = isDoubleLines;
//}

//void ChartRenderer::setDataLength(int dataLength)
//{
//    m_dataLength = dataLength;
//}



//void ChartRenderer::genNewData()
//{
////    QVector3D data_tmp[100];
////    static float x =0.0f;
////    static float distance = 2.0f * PI / DATA_LENGTH;
////    float newData = (float)randInt(-10,10)/10;
////    float y = newData*sin(x);
////    float tx;
////    tx = x;
////    tx -= PI;
////    tx /= PI;
////    data_tmp[0].setX(tx);
////    data_tmp[0].setY(y);
////    data_tmp[0].setZ(.0f);
////    x += distance;
////    if(x>2.0f * PI)x=0.0f;
////    memcpy(&data_tmp[1], m_data_vetices, (DATA_LENGTH-1)*sizeof(QVector3D));
////    memcpy(m_data_vetices, data_tmp, (DATA_LENGTH)*sizeof(QVector3D));





//    QVector3D data_tmp[DATA_LENGTH];


//    float x = -1.0f;
//    static float distance = 1.0f;
//    float newData = (float)randInt(-10.0,10.0)/20;
//    float y = newData;
////    float tx;
////    tx = x;
////    tx -= 100.0*((int)tx/100)+50;
////    tx /= 50.0;
////    data_tmp[0].setX(tx);
////    data_tmp[0].setY(y);
////    data_tmp[0].setZ(.0f);
////    x += distance;


//    for(int i=0;i<DATA_LENGTH-1;i++,x+=distance)
//    {
//        m_data_vetices[i].setX(x);
//        m_data_vetices[i].setY(m_data_vetices[i+1].y());
//        m_data_vetices2[i].setX(x);
//        m_data_vetices2[i].setY(m_data_vetices2[i+1].y());
//    }
//    m_data_vetices[DATA_LENGTH-1].setX(1.0);
//    m_data_vetices[DATA_LENGTH-1].setY(y);
//    m_data_vetices[DATA_LENGTH-1].setZ(0.0);

//    m_data_vetices2[DATA_LENGTH-1].setX(1.0);
//    m_data_vetices2[DATA_LENGTH-1].setY(y);
//    m_data_vetices2[DATA_LENGTH-1].setZ(0.0);




//}



///**
// * @brief ChartRenderer::init
// * @note 初始化数组
// */
//void ChartRenderer::init()
//{
//    int i;


//        //yang
////    double x = 0.0f;
////    double distance = 2.0f * PI / DATA_LENGTH;
////    float tx,ty;
////    for(i=0; i<DATA_LENGTH; i++, x+=distance)
////    {

////        m_data_vetices[i].setX(0);
////        m_data_vetices[i].setY(0);
////        m_data_vetices[i].setZ(0.0f);
////        tx = m_data_vetices[i].x();
////        tx -= PI;
////        tx /= PI;
////        m_data_vetices[i].setX(tx);

////        ty = m_data_vetices[i].y();
////        m_data_vetices[i].setY(ty);

////    }
////        yang







//    //wang
//    /*static*/ float x = -1.0f;
//    static float distance = 0.02f;
//    float tx;
//    for(i=0; i<DATA_LENGTH; i++, x+=distance)
//    {
//        tx = x;
////        m_data_vetices2[i].setX(tx);
//        m_data_vetices2[i].setY(0.0f);
//        m_data_vetices2[i].setZ(0.0f);

////        m_data_vetices[i].setX(tx);
//        m_data_vetices[i].setY(0.0f);
//        m_data_vetices[i].setZ(0.0f);
//    }

//    //wang


//}

//int ChartRenderer::randInt(int low, int high)
//{
//    return qrand() % ((high + 1) - low) + low;
//}


