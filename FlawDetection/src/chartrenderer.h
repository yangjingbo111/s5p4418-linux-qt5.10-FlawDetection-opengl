#ifndef CHARTRENDERER_H
#define CHARTRENDERER_H



#define DATA_LENGTH 100  //暂定画100个数据
#define PI 3.1415926

#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>

#include <QTime>
#include <QVector>

//class QOpenGLBuffer;
//class QOpenGLTexture;
//class QOpenGLShaderProgram;
//class QOpenGLVertexArrayObject;

class ChartRenderer : protected QOpenGLFunctions
{
public:
    ChartRenderer();
    ~ChartRenderer();

    void initialize();
    void render();
//    void invalidate();

//    void setNewData(float newData);
//    void setIsDoubleLines(bool isDoubleLines);
//    void setDataLength(int dataLength);

//public slots:
//    void genNewData();

//private:
//    void init();
//    int randInt(int low, int high);



//    QScopedPointer<QOpenGLShaderProgram> m_shaderProgram;
//    QScopedPointer<QOpenGLVertexArrayObject> m_vao;
//    QScopedPointer<QOpenGLBuffer> m_vvbo; // Our vertice Vertex Buffer Object
//    QScopedPointer<QOpenGLBuffer> m_data_vbo; // Our vertice Vertex Buffer Object
//    QScopedPointer<QOpenGLBuffer> m_data_vbo2; // Our vertice Vertex Buffer Object
//    QScopedPointer<QOpenGLBuffer> m_vcbo; // Our color Vertex Buffer Object



//    QTimer m_data_gen_timer;
//    int xCount = 0;

//    bool m_isDoubleLines;
//    int m_dataLength;
//    QVector3D m_data_vetices[DATA_LENGTH];
//    QVector3D m_data_vetices2[DATA_LENGTH];



private:

    qreal   m_fAngle;
    qreal   m_fScale;

    void paintQtLogo();
    void createGeometry();
    void quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4);
    void extrude(qreal x1, qreal y1, qreal x2, qreal y2);

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QOpenGLShaderProgram program1;
    int vertexAttr1;
    int normalAttr1;
    int matrixUniform1;
};

#endif // CHARTRENDERER_H
