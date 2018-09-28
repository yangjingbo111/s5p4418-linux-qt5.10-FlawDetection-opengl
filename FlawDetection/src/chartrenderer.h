#ifndef CHARTRENDERER_H
#define CHARTRENDERER_H



#define DATA_LENGTH 512  //暂定画512个数据
#define PI 3.1415926

#include <QObject>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>

#include <QTime>
#include <QVector>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QQuickWindow>

//class QOpenGLBuffer;
//class QOpenGLTexture;
//class QOpenGLShaderProgram;
//class QOpenGLVertexArrayObject;

class ChartRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ChartRenderer(QObject *parent=nullptr);
    ~ChartRenderer();

    void initialize();
    void render();
//    void invalidate();

    void setNewData(QByteArray newData, int type);
    void setHardwareDraw(int val);
//    void setIsDoubleLines(bool isDoubleLines);
//    void setDataLength(int dataLength);

public slots:
    void genNewData();

private:
    void init();
    int randInt(int low, int high);



//    QScopedPointer<QOpenGLShaderProgram> m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vvbo; // Our vertice Vertex Buffer Object
    QOpenGLBuffer m_data_vbo; // Our vertice Vertex Buffer Object
//    QScopedPointer<QOpenGLBuffer> m_data_vbo2; // Our vertice Vertex Buffer Object
//    QScopedPointer<QOpenGLBuffer> m_vcbo; // Our color Vertex Buffer Object



//    QTimer m_data_gen_timer;
//    int xCount = 0;

//    bool m_isDoubleLines;
//    int m_dataLength;
    QVector3D m_data_vetices[DATA_LENGTH];

    QQuickWindow *m_window;
    int m_hardwareDraw;

private:

    qreal   m_fAngle;
    qreal   m_fScale;


    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QOpenGLShaderProgram program1;
    int vertexAttr1;
    int normalAttr1;
    int matrixUniform1;
};

#endif // CHARTRENDERER_H
