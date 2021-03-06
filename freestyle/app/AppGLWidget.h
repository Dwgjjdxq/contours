//
//  Filename         : AppConfig.h
//  Author           : Stephane Grabli
//  Purpose          : Configuration file
//  Date of creation : 26/02/2003
//
///////////////////////////////////////////////////////////////////////////////

//
//  Copyright (C) : Please refer to the COPYRIGHT file distributed 
//   with this source distribution. 
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef  ARTGLWIDGET_H
# define ARTGLWIDGET_H

# ifndef WIN32
#  include <algorithm>
using namespace std;
#  define __min(x,y) (min(x,y))
#  define __max(x,y) (max(x,y))
# endif // WIN32


# include <qstringlist.h>
# include "../geometry/Geom.h"
# include "../geometry/BBox.h"
# include "../scene_graph/NodeDrawingStyle.h"
# include "../system/TimeUtils.h"
# include "../system/Precision.h"
# include "AppConfig.h"
# include "../rendering/GLDebugRenderer.h"
# include <QGLViewer/qglviewer.h>
# include <QGLViewer/camera.h>
# include <QGLViewer/manipulatedCameraFrame.h>
# include "../view_map/ViewMap.h" // for debugvisoptions

GLenum checkForError(const char *file, int line, const char *func);


#ifndef CHECK_FOR_ERROR
#  ifdef QT_NO_DEBUG      // NDEBUG
#     define CHECK_FOR_ERROR   ;
#  else
#     define CHECK_FOR_ERROR   checkForError(__FILE__, __LINE__, __func__);
#  endif
#endif



using namespace Geometry;

typedef enum {SURFACIC, LINE, DEPTHBUFFER} RenderStyle;   // It appears that this is not used

typedef enum {INVISIBLE_SURFACE_STYLE, SMOOTH_SURFACE_STYLE, LINE_SURFACE_STYLE, SMOOTH_LINES_SURFACE_STYLE, FRONTBACK_SURFACE_STYLE, SELECTION_SURFACE_STYLE, N_DOT_V_STYLE} SurfaceRenderStyle;

class FEdge;
class QMainWindow;
class GLRenderer;
class GLSelectRenderer;
class GLBBoxRenderer;
class GLMonoColorRenderer;
class GLDebugRenderer;

struct DebugVisOptions;

namespace qglviewer {
class ScreenManipulatedCameraFrame : public ManipulatedCameraFrame
{
public:
    ScreenManipulatedCameraFrame();

    virtual void mouseMoveEvent(QMouseEvent* const event, Camera* const camera);
    virtual void wheelEvent(QWheelEvent* const event, Camera* const camera);

    float scaleFactor;
    Vec trans;

private:
    QTimer flyTimer_;
};
}

class AppGLWidget : public QGLViewer
{
  Q_OBJECT
    
public:

  AppGLWidget(QWidget *iParent, const char *iName = 0);
  ~AppGLWidget();

public slots:
  virtual void updateSilhouettes();
  
public:
  virtual void help();
  
  // captures a frame animation that was previously registered
  void captureMovie();

  /*! Sets the rendering style.
        iStyle
          The style used to render. Can be:
          SURFACIC    : usual rendering
          LINES       : line rendering
          DEPTHBUFFER : grey-levels rendering of the depth buffer
          */
  inline void SetRenderStyle(RenderStyle iStyle)
  {
    _RenderStyle = iStyle;
  }

  inline void SetSurfaceRenderStyle(SurfaceRenderStyle style)
  {
    _SurfaceRenderStyle = style;
  }

  SurfaceRenderStyle GetSurfaceRenderStyle()
  {
    return _SurfaceRenderStyle;
  }

  NodeDrawingStyle * GetModelRootNode() { return _ModelRootNode; }

  /*! Sets the model to draw in the viewer 
   *  iModel
   *    The Root Node of the model 
   */
  inline void SetModel(NodeGroup *iModel)
  {
    if(0 != _ModelRootNode->numberOfChildren())
    {
      _ModelRootNode->DetachChildren();
      _ModelRootNode->clearBBox();
    }
      
    AddModel(iModel);
  }

  /*! Adds a model for displaying in the viewer */
  inline void AddModel(NodeGroup *iModel) 
  {
    _ModelRootNode->AddChild(iModel);
    
    _ModelRootNode->UpdateBBox();

    _minBBox = __min(__min(_ModelRootNode->bbox().getMin()[0], 
                            _ModelRootNode->bbox().getMin()[1]),
                      _ModelRootNode->bbox().getMin()[2]);
    _maxBBox = __max(__max(_ModelRootNode->bbox().getMax()[0], 
                            _ModelRootNode->bbox().getMax()[1]),
                      _ModelRootNode->bbox().getMax()[2]);
    
     _maxAbs = __max(rabs(_minBBox), rabs(_maxBBox));

     _minAbs = __min(rabs(_minBBox), rabs(_maxBBox));

     // DEBUG:
    ReInitRenderers();
 
  }

  inline void AddSilhouette(NodeGroup* iSilhouette)
  {
    _SilhouetteRootNode->AddChild(iSilhouette);
    //ToggleSilhouette(true);
    updateGL();
  }

  inline void AddViewMapVisNode(NodeGroup* visnode)
  {
    _ViewMapVisNode->AddChild(visnode);
    updateGL();
  }

  inline void AddViewMapColorNode(NodeGroup* colornode)
  {
    _ViewMapColorNode->AddChild(colornode);
    updateGL();
  }

  inline void Add2DSilhouette(NodeGroup *iSilhouette)
  {
    //_pFENode->AddChild(iSilhouette);
    //ToggleSilhouette(true);
    updateGL();
  }

  inline void Add2DVisibleSilhouette(NodeGroup *iVSilhouette)
  {
    //_pVisibleSilhouetteNode->AddChild(iVSilhouette);
    updateGL();
  }

  inline void SetDebug(NodeGroup* iDebug)
  {
    if(0 != _DebugRootNode->numberOfChildren())
    {
      _DebugRootNode->DetachChildren();
      _DebugRootNode->clearBBox();
    }
      
    AddDebug(iDebug);
  }

  inline void AddDebug(NodeGroup* iDebug)
  {
    _DebugRootNode->AddChild(iDebug);
    updateGL();
  }

  inline void SetPODebug(NodeGroup* iDebug)
  {
    if(0 != _punchOutRootNode->numberOfChildren())
    {
      _punchOutRootNode->DetachChildren();
      _punchOutRootNode->clearBBox();
    }
      
    AddPODebug(iDebug);
  }

  inline void AddPODebug(NodeGroup* iDebug)
  {
    _punchOutRootNode->AddChild(iDebug);
    updateGL();
  }



  inline void DetachModel(Node *iModel)
  {
    _ModelRootNode->DetachChild(iModel);
    _ModelRootNode->UpdateBBox();
    
    _minBBox = __min(__min(_ModelRootNode->bbox().getMin()[0], 
                            _ModelRootNode->bbox().getMin()[1]),
                      _ModelRootNode->bbox().getMin()[2]);
    _maxBBox = __max(__max(_ModelRootNode->bbox().getMax()[0], 
                            _ModelRootNode->bbox().getMax()[1]),
                      _ModelRootNode->bbox().getMax()[2]);
    
     _maxAbs = __max(rabs(_minBBox), rabs(_maxBBox));
     _minAbs = __min(rabs(_minBBox), rabs(_maxBBox));
  } 

  inline void DetachModel() 
  {
    _ModelRootNode->DetachChildren();
    _ModelRootNode->clearBBox();
    
    // 2D Scene
    //_p2DNode.DetachChildren();
    //_pFENode->DetachChildren();
    //_pVisibleSilhouetteNode->DetachChildren();
    updateGL();
  }

  inline void DetachSilhouette()
  {
    _SilhouetteRootNode->DetachChildren();
    //_pFENode->DetachChildren();
    //_pVisibleSilhouetteNode->DetachChildren();
    _p2DSelectionNode->destroy();
    //updateGL(); //FIXME
  }

  inline void DetatchViewMapVis()
  {
    _ViewMapVisNode->DetachChildren();
  }

  inline void DetachViewMapColor()
  {
    _ViewMapColorNode->DetachChildren();
  }

  inline void DetachVisibleSilhouette()
  {
    //_pVisibleSilhouetteNode->DetachChildren();
    _p2DSelectionNode->destroy();
    updateGL();
  }

  inline void DetachDebug()
  {
    _DebugRootNode->DetachChildren();
    updateGL();
  }


  inline void DetachPODebug()
  {
    _punchOutRootNode->DetachChildren();
    updateGL();
  }

  void SetMainWindow(QMainWindow *iMainWindow) ;

  inline void Set3DContext()
  {
    // GL_PROJECTION matrix
    camera()->loadProjectionMatrix();
    // GL_MODELVIEW matrix
    camera()->loadModelViewMatrix();
  }
  
  inline void RetriveModelViewMatrix(float *p)
  {
    makeCurrent();
    glGetFloatv(GL_MODELVIEW_MATRIX, p);
  }
  inline void RetriveModelViewMatrix(double *p)
  {
    makeCurrent();
    glGetDoublev(GL_MODELVIEW_MATRIX, p);
  }

  inline void RetrieveProjectionMatrix(float *p)
  {
    makeCurrent();
    glGetFloatv(GL_PROJECTION_MATRIX, p);

  }
  inline void RetrieveProjectionMatrix(double *p)
  {
    makeCurrent();
    glGetDoublev(GL_PROJECTION_MATRIX, p);

  }

  inline void RetrieveViewport(int *p)
  {
    makeCurrent();
    glGetIntegerv(GL_VIEWPORT,(GLint *)p);
  }
  
  inline real GetFocalLength() const
  {
    real Near =  __max(0.1,(double)(-2.f*_maxAbs+camera()->distanceToSceneCenter()));
    return Near;
  }

  inline real GetAspect() const
  {
    return ((real) width()/(real) height());
  }

  inline real GetFovyRadian() const
  {
    return _Fovy/180.0 * M_PI;
  }

  inline real GetFovyDegrees() const
  {
    return _Fovy;
  }

  inline void FitBBox()
  {
    qglviewer::Vec min_(_ModelRootNode->bbox().getMin()[0],
			_ModelRootNode->bbox().getMin()[1],
			_ModelRootNode->bbox().getMin()[2]);
    qglviewer::Vec max_(_ModelRootNode->bbox().getMax()[0],
			_ModelRootNode->bbox().getMax()[1],
			_ModelRootNode->bbox().getMax()[2]);
    setSceneBoundingBox(min_, max_);
    camera()->showEntireScene();
  }
  /*  
  inline void ToggleSilhouette(bool enabled) 
  {
    _fedges = enabled;
    updateGL();
  }
  
  inline void ToggleViewMap(bool enabled)
  {
    _drawViewMap = enabled;
    updateGL();
  }
  */
  // Reinit the renderers which need to be informed
  // when a model is added to the scene.
  void ReInitRenderers();

  inline void SetSelectedFEdge(FEdge* iFEdge) { _pDebugRenderer->SetSelectedFEdge(iFEdge); }

  inline GLDebugRenderer* debugRenderer() { return _pDebugRenderer; }
  inline void toggle3D() { _Draw3DScene == true ? _Draw3DScene = false : _Draw3DScene = true; updateGL();}
  inline void toggle2D() { _Draw2DScene == true ? _Draw2DScene = false : _Draw2DScene = true; updateGL();}

  /*! glReadPixels */
  typedef enum{
    RGB,
    DEPTH
  } PixelFormat;
  void readPixels(int x,
                  int y,
                  int width,
                  int height,
                  PixelFormat format,
                  float *pixels) 
  {
    makeCurrent();
    //glReadBuffer(GL_FRONT); //in reality: glReadBuffer and glDrawBuffer are both set to GL_BACK
    glReadBuffer(GL_BACK);
    GLenum glformat;
    switch(format)
    {
    case RGB:
      glformat = GL_RGB;
      break;
    case DEPTH:
      glformat = GL_DEPTH_COMPONENT;
      break;
    default:
      break;
    }
    glReadPixels(x,y,width, height, glformat, GL_FLOAT, (GLfloat*)pixels);
  }

  void clear() { makeCurrent(); glClear(GL_COLOR_BUFFER_BIT ); }

  void prepareCanvas();
  void releaseCanvas();

  typedef enum {
    FRONT,
    BACK
  } GLBuffer;

  void setReadPixelsBuffer(int iBuffer) 
  {
    makeCurrent();
    switch(iBuffer)
    {
    case FRONT:
      glReadBuffer(GL_FRONT);
      break;
    case BACK:
      glReadBuffer(GL_BACK);
      break;
    default:
      break;
    }
  }

  BBox<Vec3r> scene3DBBox() const { return _ModelRootNode->bbox(); }

  inline real znear() const {
    //    return __max((float)_maxAbs/5,(float)(-_maxAbs+camera()->distanceToSceneCenter()));
    return camera()->zNear();
  }
  
  inline real zfar() const {
    //    return _maxAbs+camera()->distanceToSceneCenter();
    return camera()->zFar();
  }

  inline bool draw3DsceneEnabled() const { return _Draw3DScene; }
  inline bool draw2DsceneEnabled() const { return _Draw2DScene; }

  inline bool getRecordFlag() const {return _record;}

  void setCameraState(const float* position, const float* orientation) {
    camera()->setPosition(qglviewer::Vec(position[0], position[1], position[2]));
    camera()->setOrientation(qglviewer::Quaternion(orientation[0], orientation[1], orientation[2], orientation[3]));
  }

  void getCameraState(float* position, float* orientation) const {
    qglviewer::Vec pos = camera()->position();
    qglviewer::Quaternion orient = camera()->orientation();
    int i;
    for(i=0;i<3;++i){
      position[i] = pos[i];
    }
    for(i=0;i<4;++i){
      orientation[i] = orient[i];
    }
  }

  void saveCameraState() {
    getCameraState(_cameraPosition, _cameraOrientation);
    _cameraStateSaved = true;
  }

  void setUpdateMode(bool b) {
    _enableUpdateSilhouettes = b;
  }

  bool getUpdateMode() const {
    return _enableUpdateSilhouettes;
  }
  static void setFrontBufferFlag(bool iBool);
  static bool getFrontBufferFlag();
  static void setBackBufferFlag(bool iBool);
  static bool getBackBufferFlag();

  // help
  virtual QString helpString() const ;

  virtual QString mouseString() const;
  virtual QString keyboardString() const;

protected:
  virtual void    mousePressEvent(QMouseEvent *);
  virtual void    mouseReleaseEvent  (  QMouseEvent *    e  ) ;
  virtual void    select(const QMouseEvent *);
  virtual void    keyPressEvent(QKeyEvent* e);
  virtual void	  init();
  virtual void    draw();

  /*! Loads an envmap */
  void LoadEnvMap(const char *filename);

public:
  /*! Core scene drawing */
  void            DrawScene(SceneVisitor *iRenderer);

  /*! 2D Scene Drawing */
  void            Draw2DScene(SceneVisitor *iRenderer);

  /*! Draws scene silhouettes in real time */
  void            DrawSilhouette();
  
  /*! Draws the Scene in lines style */
  //  void            DrawLines();
  //  /*! Draws the scene in surfacic style */
  //  void            DrawSurfacic();
  //  /*! Draws the scene as a depth buffer image */
  //  void            DrawDepthBuffer();

  GLRenderer* glRenderer() {return _pGLRenderer;}

protected:


  //QString shortcutBindingsString() const;

  /*! fabs or abs */
  inline int rabs(int x) {return abs(x);}
  inline real rabs(real x) {return fabs(x);}

  
protected:
  float _Fovy;
  //float _SceneDepth;
  //BBox<Vec3f> _BBox;
  
  RenderStyle _RenderStyle;  // this isn't used, I think

  SurfaceRenderStyle _SurfaceRenderStyle;
  bool _frontCameraMode;

  //The root node container
  NodeGroup        _RootNode;
  NodeDrawingStyle *_ModelRootNode;
  NodeDrawingStyle *_SilhouetteRootNode;
  NodeDrawingStyle *_ViewMapVisNode;
  NodeDrawingStyle *_ViewMapColorNode;
  NodeDrawingStyle *_DebugRootNode;
  NodeDrawingStyle *_punchOutRootNode;

  bool _silhouette;

  //  bool _fedges;
  //  bool _drawViewMap;
  //  bool _drawViewColor;

  bool _debug;
  bool _punchOutDebug;
  bool _selection_mode;

  DebugVisOptions _visOptions;

  //a Universal light:
  NodeGroup _Light;

  real _minBBox;
  real _maxBBox;
  real _maxAbs;

  real _minAbs;
  bool _drawBBox;

  // OpenGL Renderer
  GLRenderer *_pGLRenderer;
  GLSelectRenderer *_pSelectRenderer;
  GLBBoxRenderer *_pBBoxRenderer;
  GLMonoColorRenderer *_pMonoColorRenderer;
  GLDebugRenderer *_pDebugRenderer;
  
  QMainWindow *_pMainWindow;

  Chronometer _Chrono;

  // 2D Scene
  bool _Draw2DScene;
  bool _Draw3DScene;  NodeGroup _p2DNode;
  //NodeDrawingStyle *_pFENode; // Feature edges node
  //NodeDrawingStyle *_pVisibleSilhouetteNode;
  NodeDrawingStyle *_p2DSelectionNode;

  qglviewer::Camera* _standardCamera;
  qglviewer::Camera _frontCamera;
  qglviewer::ScreenManipulatedCameraFrame _sframe;

  // EnvMap
  bool _drawEnvMap;
  int _currentEnvMap;
  int _maxId;
  int _blendFunc;

  // Each time we compute the view map, the camera state is 
  // saved in order to be able to restore it later
  bool  _cameraStateSaved;
  float _cameraPosition[3];
  float _cameraOrientation[4];

  // interactive silhouette update
  bool _enableUpdateSilhouettes;
  //capture movie
  bool _captureMovie;
  // 2D drawing buffers
  static bool _frontBufferFlag;
  static bool _backBufferFlag;

  bool _record;

  //  GLuint _displayListName;
  //  bool _recomputeDisplayList;
  //
  // public:
  //  void recomputeDisplayList() { _recomputeDisplayList = true; }
};




#endif // ARTGLWIDGET_H
