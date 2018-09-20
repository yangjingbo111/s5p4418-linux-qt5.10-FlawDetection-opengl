#ifndef CHARTRENDERER_H
#define CHARTRENDERER_H

#include <QObject>
#include <QScopedPointer>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QTimer>
#include <QTime>
#define DATA_LENGTH 100  //暂定画100个数据
#define PI 3.1415926




class QOpenGLBuffer;
class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLVertexArrayObject;

class ChartRenderer : public QObject
{
    Q_OBJECT
public:
    ChartRenderer(QObject *parent=0);
    ~ChartRenderer();

    void initialize();
    void render();
    void invalidate();

    void setNewData(float newData);
    void setIsDoubleLines(bool isDoubleLines);
    void setDataLength(int dataLength);

public slots:
    void genNewData();

private:
    void init();
    int randInt(int low, int high);



    QScopedPointer<QOpenGLShaderProgram> m_shaderProgram;
    QScopedPointer<QOpenGLVertexArrayObject> m_vao;
    QScopedPointer<QOpenGLBuffer> m_vvbo; // Our vertice Vertex Buffer Object
    QScopedPointer<QOpenGLBuffer> m_data_vbo; // Our vertice Vertex Buffer Object
    QScopedPointer<QOpenGLBuffer> m_data_vbo2; // Our vertice Vertex Buffer Object
    QScopedPointer<QOpenGLBuffer> m_vcbo; // Our color Vertex Buffer Object



    QTimer m_data_gen_timer;
    int xCount = 0;

    bool m_isDoubleLines;
    int m_dataLength;
    QVector3D m_data_vetices[DATA_LENGTH];
    QVector3D m_data_vetices2[DATA_LENGTH];
};

#endif // CHARTRENDERER_H
