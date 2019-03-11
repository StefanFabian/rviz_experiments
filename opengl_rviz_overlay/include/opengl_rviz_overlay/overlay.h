//
// Created by Stefan Fabian on 07.03.19.
//

#ifndef OPENGL_RVIZ_OVERLAY_OVERLAY_H
#define OPENGL_RVIZ_OVERLAY_OVERLAY_H

#include <OgreRenderQueueListener.h>

namespace rviz
{
class RenderPanel;
}

namespace opengl_rviz_overlay
{

class Overlay : public Ogre::RenderQueueListener
{
public:
  Overlay(Ogre::Node* object, Ogre::SceneManager* sceneMgr) :
  scene_node_(object),
  scene_manager_(sceneMgr)
  {}

  void show();

  void hide();

  void
  renderQueueEnded( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation ) override;


  void
  renderQueueStarted( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation ) override;

protected:
  void nativeRender();

  rviz::RenderPanel *render_panel_;
  Ogre::Node* scene_node_;
  const Ogre::Camera* mCamera;
  Ogre::SceneManager* scene_manager_;

  bool visible_;
};
} // opengl_rviz_overlay

#endif //OPENGL_RVIZ_OVERLAY_OVERLAY_H
