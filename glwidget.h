#ifndef GLWIDGET_H
#define GLWIDGET_H
#ifdef __APPLE__
#include <QtOpenGL/QGLWidget>
#else
#include <QGLWidget>
#endif
#include "common.h"
class DrawEngine;
class QTimer;
class QElapsedTimer;

class GLWidget : public QGLWidget {
    Q_OBJECT
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void render_text();
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QTimer *timer_;
    QElapsedTimer *time_;
    DrawEngine *draw_engine_;
    float prev_fps_;
    float2 mouse_pos_prev_;
};

#endif // GLWIDGET_H
