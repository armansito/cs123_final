/**
  @author psastras
**/

#include "drawengine.h"
#include "glm.h"
#include <QKeyEvent>
#include <QHash>
#include <QQuaternion>
#include <QVector3D>
#include <QString>
#include <iostream>
#include <QFile>
#include <QTime>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#define APIENTRY
#define ROOT_PATH "/Users/armanuguray/Documents/courses/cs123/final/cs123_final/"
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLFramebufferObject>
#else
#define ROOT_PATH "../cs123_final/"
#include <qgl.h>
#include <QGLContext>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <GL/glu.h>
#include <GL/glext.h>
#endif
#include "Settings.h"
using std::cout;
using std::endl;

extern "C"{
    extern void APIENTRY glActiveTexture (GLenum);
}

// this is the width of the big water square.
#define EXTENT 100
#define WATER_WIDTH (100)
#define VERTICES_PER_SIDE (70)

//#define LOG(s) cout << s << endl;
#define LOG(s)

#define MAX_RIPPLES 50
#define RIPPLE_DEATH_TIME 4000

#define RIPPLE_DELAY 250


/**
  @paragraph DrawEngine ctor.  Expects a Valid OpenGL context and the viewport's current
  width and height.  Initializes the draw engine.  Loads models,textures,shaders,
  and allocates framebuffers.  Also sets up OpenGL to begin drawing.

  @param context The current OpenGL context this drawing engine is associated with.
  Probably should be the context from the QGLWidget.

  @param w The viewport width used to allocate the correct framebuffer size.
  @param h The viewport heigh used to alloacte the correct framebuffer size.

**/
DrawEngine::DrawEngine(const QGLContext *context,int w,int h) : context_(context) {
    //initialize ogl settings
    glEnable(GL_TEXTURE_2D);
    glFrontFace(GL_CCW);
    glDisable(GL_DITHER);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    //init member variables
    previous_time_ = 0.0f;
    camera_.center.x = 0.f,camera_.center.y = 0.f,camera_.center.z = 0.f;
    camera_.eye.x = 20.f,camera_.eye.y = 10.f,camera_.eye.z = -20.f;
    camera_.up.x = 0.f,camera_.up.y = 1.f,camera_.up.z = 0.f;
    camera_.near = 0.1f,camera_.far = sqrt(3)*2*EXTENT;
    camera_.fovy = 60.f;

    //init resources - so i heard you like colored text?
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;
    //ideally we would now check to make sure all the OGL functions we use are supported
    //by the video card.  but that's a pain to do so we're not going to.
    cout << "\033[31;1mLoading Resources\033[0m" << endl;
    load_models();
    load_shaders();
    load_textures();
    create_fbos(w,h);
    cout << "\033[31;1mRenderingi...\033[0m" << endl;
}

/**
  @paragraph Dtor
**/
DrawEngine::~DrawEngine() {
    foreach(QGLShaderProgram *sp,shader_programs_)
        delete sp;
    foreach(QGLFramebufferObject *fbo,framebuffer_objects_)
        delete fbo;
    foreach(GLuint id,textures_)
        ((QGLContext *)(context_))->deleteTexture(id);
    foreach(Model m,models_)
        glmDelete(m.model);
}

/**
  @paragraph Loads models used by the program.  Caleed by the ctor once upon
  initialization.
**/
void DrawEngine::load_models() {
    cout << "\033[1mLoading models...\033[0m" << endl;
    models_["yacht"].model = glmReadOBJ(ROOT_PATH "models/yacht/yacht.obj");
    glmUnitize(models_["yacht"].model);
    glEnable(GL_LIGHTING);
    glNewList(models_["yacht"].idx = glGenLists(1),GL_COMPILE);
    glmFacetNormals(models_["yacht"].model);
    glmVertexNormals(models_["yacht"].model, 90.0);
    glmDraw(models_["yacht"].model,GLM_SMOOTH | GLM_TEXTURE);
    glEndList();
    glDisable(GL_LIGHTING);
    cout << "\t \033[32m/course/cs123/data/mesh/yacht.obj\033[0m" << endl;

    //Create grid
    models_["grid"].idx = glGenLists(1);
    glNewList(models_["grid"].idx,GL_COMPILE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float r = WATER_WIDTH / 2.f, dim = VERTICES_PER_SIDE, delta = WATER_WIDTH / dim;
    for(int y = 0; y < dim; ++y) {
        glBegin(GL_QUAD_STRIP);
        for(int x = 0; x <= dim; ++x) {
            glNormal3f(0, 1, 0);
            glVertex3f(x * delta - r, 0, y * delta - r);
            glVertex3f(x * delta - r, 0, (y + 1) * delta - r);
        }
        glEnd();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEndList();

    cout << "\t \033[32mgrid compiled\033[0m" << endl;
    models_["skybox"].idx = glGenLists(1);
    glNewList(models_["skybox"].idx,GL_COMPILE);
    //Be glad we wrote this for you...ugh.
    glBegin(GL_QUADS);
    float fExtent = EXTENT;//WATER_WIDTH / 2;
    glTexCoord3f(1.0f,-1.0f,-1.0f); glVertex3f(fExtent,-fExtent,-fExtent);
    glTexCoord3f(-1.0f,-1.0f,-1.0f);glVertex3f(-fExtent,-fExtent,-fExtent);
    glTexCoord3f(-1.0f,1.0f,-1.0f);glVertex3f(-fExtent,fExtent,-fExtent);
    glTexCoord3f(1.0f,1.0f,-1.0f); glVertex3f(fExtent,fExtent,-fExtent);
    glTexCoord3f(1.0f,-1.0f,1.0f);glVertex3f(fExtent,-fExtent,fExtent);
    glTexCoord3f(1.0f,-1.0f,-1.0f); glVertex3f(fExtent,-fExtent,-fExtent);
    glTexCoord3f(1.0f,1.0f,-1.0f);  glVertex3f(fExtent,fExtent,-fExtent);
    glTexCoord3f(1.0f,1.0f,1.0f); glVertex3f(fExtent,fExtent,fExtent);
    glTexCoord3f(-1.0f,-1.0f,1.0f);  glVertex3f(-fExtent,-fExtent,fExtent);
    glTexCoord3f(1.0f,-1.0f,1.0f); glVertex3f(fExtent,-fExtent,fExtent);
    glTexCoord3f(1.0f,1.0f,1.0f);  glVertex3f(fExtent,fExtent,fExtent);
    glTexCoord3f(-1.0f,1.0f,1.0f); glVertex3f(-fExtent,fExtent,fExtent);
    glTexCoord3f(-1.0f,-1.0f,-1.0f); glVertex3f(-fExtent,-fExtent,-fExtent);
    glTexCoord3f(-1.0f,-1.0f,1.0f);glVertex3f(-fExtent,-fExtent,fExtent);
    glTexCoord3f(-1.0f,1.0f,1.0f); glVertex3f(-fExtent,fExtent,fExtent);
    glTexCoord3f(-1.0f,1.0f,-1.0f);glVertex3f(-fExtent,fExtent,-fExtent);
    glTexCoord3f(-1.0f,1.0f,-1.0f);glVertex3f(-fExtent,fExtent,-fExtent);
    glTexCoord3f(-1.0f,1.0f,1.0f);glVertex3f(-fExtent,fExtent,fExtent);
    glTexCoord3f(1.0f,1.0f,1.0f);glVertex3f(fExtent,fExtent,fExtent);
    glTexCoord3f(1.0f,1.0f,-1.0f);glVertex3f(fExtent,fExtent,-fExtent);
    glTexCoord3f(-1.0f,-1.0f,-1.0f);glVertex3f(-fExtent,-fExtent,-fExtent);
    glTexCoord3f(-1.0f,-1.0f,1.0f);glVertex3f(-fExtent,-fExtent,fExtent);
    glTexCoord3f(1.0f,-1.0f,1.0f); glVertex3f(fExtent,-fExtent,fExtent);
    glTexCoord3f(1.0f,-1.0f,-1.0f);glVertex3f(fExtent,-fExtent,-fExtent);
    glEnd();
    glEndList();
    cout << "\t \033[32mskybox compiled\033[0m" << endl;
}
/**
  @paragraph Loads shaders used by the program.  Caleed by the ctor once upon
  initialization.
**/
void DrawEngine::load_shaders() {
    cout << "\033[1mLoading shaders...\033[0m" << endl;
    shader_programs_["reflect"] = new QGLShaderProgram(context_);
    shader_programs_["reflect"]->addShaderFromSourceFile(QGLShader::Vertex,
                                                         ROOT_PATH "shaders/reflect.vert");
    shader_programs_["reflect"]->addShaderFromSourceFile(QGLShader::Fragment,
                                                         ROOT_PATH "shaders/reflect.frag");
    shader_programs_["reflect"]->link();
    cout << "\t \033[32mshaders/reflect\033[0m" << endl;
    shader_programs_["water"] = new QGLShaderProgram(context_);
    shader_programs_["water"]->addShaderFromSourceFile(QGLShader::Vertex,
                                                       ROOT_PATH "shaders/water.vert");
    shader_programs_["water"]->addShaderFromSourceFile(QGLShader::Fragment,
                                                       ROOT_PATH "shaders/water.frag");
    shader_programs_["water"]->link();
    cout << "\t \033[32mshaders/water\033[0m" << endl;
    shader_programs_["brightpass"] = new QGLShaderProgram(context_);
    shader_programs_["brightpass"]->addShaderFromSourceFile(QGLShader::Fragment,
                                                            ROOT_PATH "shaders/brightpass.frag");
    shader_programs_["brightpass"]->link();
    cout << "\t \033[32mshaders/brightpass\033[0m" << endl;

    shader_programs_["blur"] = new QGLShaderProgram(context_);
    shader_programs_["blur"]->addShaderFromSourceFile(QGLShader::Fragment,
                                                      ROOT_PATH "shaders/blur.frag");
    shader_programs_["blur"]->link();
    cout << "\t \033[32mshaders/blur\033[0m" << endl;
}
/**
  @paragraph Loads textures used by the program.  Caleed by the ctor once upon
  initialization.
**/
void DrawEngine::load_textures() {
    cout << "\033[1mLoading textures...\033[0m" << endl;
    QList<QFile *> fileList;
    fileList.append(new QFile(ROOT_PATH "textures/football/posx.jpg"));
    fileList.append(new QFile(ROOT_PATH "textures/football/negx.jpg"));
    fileList.append(new QFile(ROOT_PATH "textures/football/posy.jpg"));
    fileList.append(new QFile(ROOT_PATH "textures/football/negy.jpg"));
    fileList.append(new QFile(ROOT_PATH "textures/football/posz.jpg"));
    fileList.append(new QFile(ROOT_PATH "textures/football/negz.jpg"));
    textures_["cube_map_1"] = load_cube_map(fileList);
}
/**
  @paragraph Creates the intial framebuffers for drawing.  Called by the ctor once
  upon initialization.

  @todo Finish filling this in.

  @param w:    the viewport width
  @param h:    the viewport height
**/
void DrawEngine::create_fbos(int w,int h) {

    //Allocate the main framebuffer object for rendering the scene to
    //This needs a depth attachment.
    framebuffer_objects_["fbo_0"] = new QGLFramebufferObject(w,h,QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D,GL_RGB16F_ARB);
    framebuffer_objects_["fbo_0"]->format().setSamples(16);
    //Allocate the secondary framebuffer obejcts for rendering textures to (post process effects)
    //These do not require depth attachments.kybo
    framebuffer_objects_["fbo_1"] = new QGLFramebufferObject(w,h,QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D,GL_RGB16F_ARB);
    //You need to create another framebuffer here.  Look up two lines to see how to do this... =.=
    framebuffer_objects_["fbo_2"] = new QGLFramebufferObject(w,h,QGLFramebufferObject::NoAttachment,
                                                             GL_TEXTURE_2D,GL_RGB16F_ARB);
}
/**
  @paragraph Reallocates all the framebuffers.  Called when the viewport is
  resized.

  @param w:    the viewport width
  @param h:    the viewport height
**/
void DrawEngine::realloc_framebuffers(int w,int h) {
    foreach(QGLFramebufferObject *fbo,framebuffer_objects_)  {
        const QString &key = framebuffer_objects_.key(fbo);
        QGLFramebufferObjectFormat format = fbo->format();
        delete fbo;
        framebuffer_objects_[key] = new QGLFramebufferObject(w,h,format);
    }
}

/**
  @paragraph Should render one frame at the given elapsed time in the program.
  Assumes that the GL context is valid when this method is called.

  @todo Finish filling this in

  @param time: the current program time in milliseconds
  @param w:    the viewport width
  @param h:    the viewport height

**/
void DrawEngine::draw_frame(float time,int w,int h) {
    fps_ = 1000.f / (time - previous_time_),previous_time_ = time;
    //Render the scene to a framebuffer
    framebuffer_objects_["fbo_0"]->bind();
    perspective_camera(w,h);
    render_scene(time,w,h);
    framebuffer_objects_["fbo_0"]->release();

    //copy the rendered scene into framebuffer 1
    framebuffer_objects_["fbo_0"]->blitFramebuffer(framebuffer_objects_["fbo_1"],
                                                   QRect(0,0,w,h),framebuffer_objects_["fbo_0"],
                                                   QRect(0,0,w,h),GL_COLOR_BUFFER_BIT,GL_NEAREST);
    orthogonal_camera(w,h);
    glBindTexture(GL_TEXTURE_2D, framebuffer_objects_["fbo_1"]->texture());
    textured_quad(w,h,true);
    glBindTexture(GL_TEXTURE_2D, 0);


    //you may want to add code here
    // step 1
    framebuffer_objects_["fbo_2"]->bind();
    shader_programs_["brightpass"]->bind();
    glBindTexture(GL_TEXTURE_2D, framebuffer_objects_["fbo_1"]->texture());
    textured_quad(w,h,true);
    shader_programs_["brightpass"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    framebuffer_objects_["fbo_2"]->release();
    // step 2
    //Uncomment this section in step 2 of the lab...
    float scales[] = {4.f,8.f,16.f,32.f};
    for(int i = 0; i < 4; ++i) {
        render_blur(w /scales[i],h /scales[i]);
        glBindTexture(GL_TEXTURE_2D,framebuffer_objects_["fbo_1"]->texture());
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE);
        glTranslatef(0.f,(scales[i] - 1)* -h,0.f);
        textured_quad(w * scales[i],h * scales[i],false);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,0);
    }

}

/**
  @paragraph Updates each ripples elapsed time. If the ripple has been around for a
             certain time, it will be decl
        cout << i << endl;
        cout << it << endl;ared dead and removed from the list
 **/
void DrawEngine::updateRipples()
{
    if (_ripples.size() > 0) {
        Ripple r;
        std::vector<Ripple>::iterator it = _ripples.begin();
        while (it != _ripples.end()) {
            r = (*it);
            LOG(r._time->elapsed());
            if (r._time->elapsed() >= RIPPLE_DEATH_TIME) {
                delete r._time;
                it = _ripples.erase(it);
            } else ++it;
        }
    }
}

void DrawEngine::addRipple(float3 p)
{
    static QTime timer = QTime::currentTime();
    static int milliseconds = 0;

    milliseconds += timer.restart();
    if (milliseconds >= RIPPLE_DELAY) {
        if (_ripples.size() < MAX_RIPPLES) {
            Ripple r;
            r._position = p;
            r._amplitude = settings.ripple_amplitude;
            r._energy = settings.ripple_energy;
            r._speed = settings.ripple_speed / 100.f;
            r._time = new QTime();
            _ripples.push_back(r);
            r._time->start();
        }
    }
}

/**
  @paragraph Should run a gaussian blur on the texture stored in
  fbo 2 and put the result in fbo 1.  The blur should have a radius of 2.

  @todo Finish filling this in.

  @param time: the current program time in milliseconds
  @param w:    the viewport width
  @param h:    the viewport height

**/
void DrawEngine::render_blur(float w,float h) {
    int radius = 3,dim = radius * 2 + 1;
    GLfloat kernel[dim * dim],offsets[dim * dim * 2];
    create_blur_kernel(radius,w,h,&kernel[0],&offsets[0]);
    // you may want to add code here
    framebuffer_objects_["fbo_1"]->bind();
    shader_programs_["blur"]->bind();
    shader_programs_["blur"]->setUniformValueArray("offsets",offsets,dim*dim*2,2);
    shader_programs_["blur"]->setUniformValueArray("kernel",kernel,dim*dim,1);
    glBindTexture(GL_TEXTURE_2D, framebuffer_objects_["fbo_2"]->texture());
    textured_quad(w,h,false);
    shader_programs_["blur"]->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    framebuffer_objects_["fbo_1"]->release();
}

/**
  @paragraph Renders the actual scene.  May be called multiple times by
  DrawEngine::draw_frame(float time,int w,int h) if necessary.

  @param w: the viewport width
  @param h: the viewport height

**/
void DrawEngine::render_scene(float time,int w,int h) {
    updateRipples();
    LOG(_ripples.size());
    GLfloat *ripples = new GLfloat[_ripples.size()*4];
    GLfloat *other_vals = new GLfloat[_ripples.size()*2];
    Ripple r;
    int i = 0;
    int j = 0;
    for (unsigned int it = 0; it < _ripples.size(); it++) {
        r = _ripples[it];
        ripples[i++] = r._position.x;
        ripples[i++] = r._position.z;
        ripples[i++] = r._amplitude;
        ripples[i++] = (float)r._time->elapsed();

        other_vals[j++] = r._energy;
        other_vals[j++] = r._speed;
    }
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textures_["cube_map_1"]);
    //glEnable(GL_CULL_FACE);
    glCallList(models_["skybox"].idx);
    //glDisable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    shader_programs_["water"]->bind();
    shader_programs_["water"]->setUniformValue("neighborDist", 0);
    shader_programs_["water"]->setUniformValue("time", time);
    shader_programs_["water"]->setUniformValue("ripples_count",(GLuint)_ripples.size());
    shader_programs_["water"]->setUniformValueArray("ripples",ripples,_ripples.size()*4,4);
    shader_programs_["water"]->setUniformValueArray("other_vals",other_vals,_ripples.size()*2,2);
    shader_programs_["water"]->setUniformValue("CubeMap",GL_TEXTURE0);

    glCallList(models_["grid"].idx);

    shader_programs_["water"]->release();

    delete[] ripples;
    delete[] other_vals;

    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);

    if (settings.show_boat) {
        float d = 12;
        float x = time/2000.f;
        boat_pos = float3(d*sin(x),0,d*cos(x));
        glPushMatrix();
        glTranslatef(boat_pos.x,5,boat_pos.z);
        glRotatef(x*55,0,1,0);
        glScalef(6,6,6);
       // GLUquadric *q = gluNewQuadric();
        //gluSphere(q,1.f,20,20);
        glCallList(models_["yacht"].idx);
        //gluDeleteQuadric(q);
        glPopMatrix();
        static QTime timer = QTime::currentTime();
        static int milliseconds = 0;

        milliseconds += timer.restart();
        if (milliseconds >= RIPPLE_DELAY) {
            if (_ripples.size() < MAX_RIPPLES) {
                Ripple r;
                r._position = boat_pos;
                r._amplitude = 0.1;
                r._energy = 0.2;
                r._speed = 100;
                r._time = new QTime();
                _ripples.push_back(r);
                r._time->start();
            }
        }
    }

    /*
    shader_programs_["reflect"]->bind();\
    shader_programs_["reflect"]->setUniformValue("CubeMap",GL_TEXTURE0);

    glPushMatrix();
    glTranslatef(1.25f,0.f,0.f);
    glCallList(models_["dragon"].idx);
    glPopMatrix();

    shader_programs_["reflect"]->release();
    */


}

/**
  @paragraph Draws a textured quad. The texture most be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound before hand.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
  @param flip: flip the texture vertically

**/
void DrawEngine::textured_quad(int w,int h,bool flip) {
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,flip ? 1.0f : 0.0f);
    glVertex2f(0.0f,0.0f);
    glTexCoord2f(1.0f,flip ? 1.0f : 0.0f);
    glVertex2f(w,0.0f);
    glTexCoord2f(1.0f,flip ? 0.0f : 1.0f);
    glVertex2f(w,h);
    glTexCoord2f(0.0f,flip ? 0.0f : 1.0f);
    glVertex2f(0.0f,h);
    glEnd();
}

/**
  @paragraph Called to switch to the perspective OpenGL camera.
  Used to render the scene regularly with the current camera parameters.

  @param w: the viewport width
  @param h: the viewport height

**/
void DrawEngine::perspective_camera(int w,int h) {
    float ratio = w / static_cast<float>(h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera_.fovy,ratio,camera_.near,camera_.far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera_.eye.x,camera_.eye.y,camera_.eye.z,
              camera_.center.x,camera_.center.y,camera_.center.z,
              camera_.up.x,camera_.up.y,camera_.up.z);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
}

float3 DrawEngine::getMouseRay(const float2 &mouse, const Camera &camera)
{
    int viewport[4];
    double worldX, worldY, worldZ;
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(mouse.x, (float)viewport[3] - mouse.y - 1, 1,
                 modelviewMatrix, projectionMatrix, viewport,
                 &worldX, &worldY, &worldZ);
    return (float3(worldX, worldY, worldZ) - camera.eye).getNormalized();
}

/**
  @paragraph Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.

  @param w: the viewport width
  @param h: the viewport height

**/
void DrawEngine::orthogonal_camera(int w,int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,static_cast<float>(w),static_cast<float>(h),0.f,-1.f,1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  @paragraph Called when the viewport has been resized. Needs to
  resize the camera perspective and reallocate the framebuffer
  sizes.

  @param w: the viewport width
  @param h: the viewport height

**/
void DrawEngine::resize_frame(int w,int h) {
    glViewport(0,0,w,h);
    realloc_framebuffers(w,h);
}

/**
  @paragraph Called by GLWidget when the mouse is dragged.  Rotates the camera
  based on mouse movement.

  @param p0: the old mouse position
  @param p1: the new mouse position
**/
void DrawEngine::mouse_drag_event(float2 p0,float2 p1) {
    int dx = p1.x - p0.x,dy = p1.y - p0.y;
    QQuaternion qq = QQuaternion::fromAxisAndAngle(0, 1, 0, -dx / 5.0);
    QVector3D qv3 = qq.rotatedVector(QVector3D(camera_.eye.x, camera_.eye.y,
                                               camera_.eye.z));
    qq = QQuaternion::fromAxisAndAngle(qq.rotatedVector(QVector3D(1, 0, 0)), dy / 5.0);
    qv3 = qq.rotatedVector(qv3);
    camera_.eye.x = qv3.x(), camera_.eye.y = qv3.y(), camera_.eye.z = qv3.z();
}

/**
  @paragraph Called by GLWidget when the mouse wheel is turned. Zooms the camera in
  and out.

  @param dx: The delta value of the mouse wheel movement.
**/
void DrawEngine::mouse_wheel_event(int dx) {
    if((camera_.center - camera_.eye).getMagnitude() > .5 || dx < 0)
        camera_.eye += (camera_.center - camera_.eye).getNormalized() * dx * .005;
}

void DrawEngine::mouse_press_event(float2 point)
{
    float3 R = getMouseRay(point, camera_);
    float3 p = camera_.eye;
    float3 intersect;
    float t = -p.y / R.y;
    intersect = p + R*t;
    Ripple r;
    addRipple(intersect);

}

/**
  @paragraph Loads the cube map into video memory.

  @param files: a list of files containing the cube map images (should be length
  six) in order.
  @return The assigned OpenGL id to the cube map.
**/
GLuint DrawEngine::load_cube_map(QList<QFile *> files) {
    GLuint id;
    glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_CUBE_MAP,id);
    for(unsigned i = 0; i < 6; ++i) {
        QImage image,texture;
        image.load(files[i]->fileName());
        image = image.mirrored(false,true);
        texture = QGLWidget::convertToGLFormat(image);
        texture = texture.scaledToWidth(1024,Qt::SmoothTransformation);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,3,3,texture.width(),texture.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,texture.bits());
        gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 3, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
        cout << "\t \033[32m" << files[i]->fileName().toStdString() << "\033[0m" << endl;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    return id;
}
/**
  @paragraph Creates a gaussian blur kernel with the specified radius.  The kernel values
  and offsets are stored.

  @param radius: The radius of the kernel to create.
  @param w: The width of the image.
  @param h: The height of the image.
  @param kernel: The array to write the kernel values to.
  @param offsets: The array to write the offset values to.
**/
void DrawEngine::create_blur_kernel(int radius,int w,int h,GLfloat* kernel,GLfloat* offsets) {
    int size = radius * 2 + 1;
    float sigma = radius / 3.0f,twoSigmaSigma = 2.0f * sigma * sigma,
    rootSigma = sqrt(twoSigmaSigma * M_PI),total = 0.0f;
    float xOff = 1.0f / w,yOff = 1.0f / h;
    int offsetIndex = 0;
    for(int y = -radius,idx = 0; y <= radius; ++y) {
        for(int x = -radius; x <= radius; ++x,++idx) {
            float d = x * x + y * y;
            kernel[idx] = exp(-d / twoSigmaSigma) / rootSigma;
            total += kernel[idx];
            offsets[offsetIndex++] = x * xOff;
            offsets[offsetIndex++] = y * yOff;
        }
    }
    for(int i = 0; i < size * size; ++i) kernel[i] /= total;
}

/**
  @paragraph Called when a key has been pressed in the GLWidget.

  @param event: The key press event associated with the current key press.
  **/
void DrawEngine::key_press_event(QKeyEvent *event) {
    switch(event->key()) {

    }
}
