//
// Created by Stefan Fabian on 07.03.19.
//

#include "opengl_rviz_overlay/overlay.h"

#include <GL/gl.h>
#include <ros/ros.h>

#include <OgreRenderSystem.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>

#include <rviz/render_panel.h>
#include <opengl_rviz_overlay/overlay.h>


namespace opengl_rviz_overlay
{

void Overlay::show()
{
  visible_ = true;
}

void Overlay::hide()
{
  visible_ = false;
}

void Overlay::renderQueueStarted( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation )
{
}

void Overlay::renderQueueEnded( Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation )
{
  if (queueGroupId != Ogre::RENDER_QUEUE_MAIN ) return;
//  ROS_INFO("Rendering");

//  Ogre::Camera *camera = scene_manager_->getCameraIterator().getNext();
//  if (camera != mCamera)
//  {
//    ROS_INFO_STREAM("Replacing camera");
//    mCamera = camera;
//  }

  // save matrices
//  glMatrixMode(GL_MODELVIEW);
//  glPushMatrix();
//  glMatrixMode(GL_PROJECTION);
//  glPushMatrix();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity(); //Texture addressing should start out as direct.

  scene_manager_->getDestinationRenderSystem()->_setWorldMatrix( Ogre::Matrix4::IDENTITY );// scene_node_->_getFullTransform() );
  scene_manager_->getDestinationRenderSystem()->_setViewMatrix( Ogre::Matrix4::IDENTITY );// scene_node_->_getFullTransform() );
  scene_manager_->getDestinationRenderSystem()->_setProjectionMatrix( Ogre::Matrix4::IDENTITY );// scene_node_->_getFullTransform() );

  static Ogre::Pass* clearPass = NULL;
  if (!clearPass) {
    Ogre::MaterialPtr clearMat =
      Ogre::MaterialManager::getSingleton().getByName("BaseWhite");
    clearPass = clearMat->getTechnique(0)->getPass(0);
  }
  //Set a clear pass to give the renderer a clear renderstate
  scene_manager_->_setPass(clearPass, true, false);

  // save attribs
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // call native rendering function
  //////////////////
  nativeRender();
  //////////////////

  // restore original state
  glPopAttrib();

  // restore matrices
//  glMatrixMode(GL_TEXTURE);
//  glPopMatrix();
//  glMatrixMode(GL_PROJECTION);
//  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void Overlay::nativeRender() {
  if (!visible_) return;
  GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
  if (cullFaceEnabled) glDisable(GL_CULL_FACE);

  // do some OpenGL drawing here
  static const double x = 0, y = 0, r = 1;
  glBegin(GL_POLYGON);
  glVertex2f(x + r, y);
  glVertex2f(x, y + r);
  glVertex2f(x - r, y);
  glVertex2f(x, y - r);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnd();

  if (cullFaceEnabled) glEnable(GL_CULL_FACE);
}
}
