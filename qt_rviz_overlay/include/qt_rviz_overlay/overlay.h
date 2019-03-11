//
// Created by Stefan Fabian on 07.03.19.
//

#ifndef OPENGL_RVIZ_OVERLAY_OVERLAY_H
#define OPENGL_RVIZ_OVERLAY_OVERLAY_H

#include <OgreRenderQueueListener.h>

class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFramebufferObject;
class QOpenGLPaintDevice;

namespace rviz
{
class RenderPanel;
}

namespace qt_rviz_overlay
{

class Overlay : public Ogre::RenderQueueListener
{
  struct NativeContextInformation;
public:
  Overlay(Ogre::Node* object, Ogre::SceneManager* sceneMgr, rviz::RenderPanel *render_panel);

  ~Overlay();

//  void initialize();
//
//  void makeCurrent();
//
//  void doneCurrent();

  void show();

  void hide();

  void
  renderQueueEnded( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation ) override;


  void
  renderQueueStarted( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation ) override;

protected:
  void nativeRender();

  Ogre::Node* scene_node_;
  Ogre::SceneManager* scene_manager_;

  QOpenGLContext *context_;
  QOpenGLContext *native_context_;
  QOffscreenSurface *surface_;
  QOpenGLFramebufferObject *fbo_;
  QOpenGLFramebufferObject *texture_fbo_;
  QOpenGLPaintDevice *paint_device_;

  int timer_index_;
  double timer_history_[15];
  double timer_average_;

  NativeContextInformation *native_context_information_;

  bool visible_;
};
} // qt_rviz_overlay

#endif //OPENGL_RVIZ_OVERLAY_OVERLAY_H
