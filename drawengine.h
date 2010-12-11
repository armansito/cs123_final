#ifndef DRAWENGINE_H
#define DRAWENGINE_H

#include <QHash>
#include <QString>
#ifdef __APPLE__
#include <QtOpenGL/qgl.h>
#else
#include <qgl.h>
#endif
#include "glm.h"
#include "common.h"
#include <QVector3D>
#include <vector>
#include <QTime>

class QGLContext;
class QGLShaderProgram;
class QFile;
class QGLFramebufferObject;
class QKeyEvent;

struct Model {
    GLMmodel *model;
    GLuint idx;
};

struct Camera {
    float3 eye, center, up;
    float fovy, near, far;
};

struct Ripple {
    float3 _position;
    QTime *_time;
};

class DrawEngine {
public:

    //ctor and dtor
    DrawEngine(const QGLContext *context, int w, int h);
    ~DrawEngine();

    //methods
    void draw_frame(float time, int w, int h);
    void resize_frame(int w, int h);
    void mouse_wheel_event(int dx);
    void mouse_drag_event(float2 p0, float2 p1);
    void mouse_press_event(float2 point);
    void key_press_event(QKeyEvent *event);
    //getters and setters
    float fps() { return fps_; }

    //member variables

protected:

    //methods
    void perspective_camera(int w, int h);
    void orthogonal_camera(int w, int h);
    void textured_quad(int w, int h, bool flip);
    void render_scene(float time, int w, int h);
    void realloc_framebuffers(int w, int h);
    void render_blur(float width, float height);
    void load_models();
    void load_textures();
    void load_shaders();
    GLuint load_cube_map(QList<QFile *> files);
    void create_fbos(int w, int h);
    void create_blur_kernel(int radius,int w,int h,GLfloat* kernel,GLfloat* offsets);

    // shoots a ray from the screen to the object space
    float3 getMouseRay(const float2 &mouse, const Camera &camera);
    void updateRipples();
    void addRipple(float3 p);

    //member variables
    QHash<QString, QGLShaderProgram *>          shader_programs_; ///hash map of all shader programs
    QHash<QString, QGLFramebufferObject *>      framebuffer_objects_; ///hash map of all framebuffer objects
    QHash<QString, Model>                       models_; ///hashmap of all models
    QHash<QString, GLuint>                      textures_; ///hashmap of all textures
    const QGLContext                            *context_; ///the current OpenGL context to render to
    float                                       previous_time_, fps_; ///the previous time and the fps counter
    Camera                                      camera_; ///a simple camera struct
    double modelviewMatrix[16], projectionMatrix[16]; // maintains the modelview and projection matrices
    std::vector<Ripple> _ripples; // holds a list of ripples and their elapsed time
};

#endif // DRAWENGINE_H
