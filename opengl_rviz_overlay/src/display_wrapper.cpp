//
// Created by Stefan Fabian on 07.03.19.
//

#include "opengl_rviz_overlay/display_wrapper.h"

#include <rviz/display_context.h>
#include <rviz/render_panel.h>
#include <rviz/view_manager.h>

#include <OgreManualObject.h>

namespace opengl_rviz_overlay
{

DisplayWrapper::DisplayWrapper() : overlay_( nullptr )
{
}

DisplayWrapper::~DisplayWrapper()
{
  if ( overlay_ != nullptr )
  {
    context_->getSceneManager()->removeRenderQueueListener( overlay_.get());
  }
}

void DisplayWrapper::onInitialize()
{
  rviz::Display::onInitialize();


  Ogre::Node *node = scene_node_->createChildSceneNode();

  Ogre::String RenderSystemName = context_->getSceneManager()->getDestinationRenderSystem()->getName();
  ROS_INFO_STREAM("Render System: " << RenderSystemName);
//  if ("OpenGL Rendering Subsystem" == RenderSystemName)
//  {
//  }
  overlay_ =  std::make_shared<Overlay>(node, context_->getSceneManager());
  context_->getSceneManager()->addRenderQueueListener(overlay_.get());

  if ( !isEnabled())
  {
    overlay_->hide();
  }
  setStatus( rviz::StatusProperty::Ok, "Overlay", "Overlay created successfully." );
}

void DisplayWrapper::onEnable()
{
  rviz::Display::onEnable();
  if ( overlay_ == nullptr ) return;

  overlay_->show();
}

void DisplayWrapper::onDisable()
{
  rviz::Display::onDisable();
  if ( overlay_ == nullptr ) return;

  overlay_->hide();
}
}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS( opengl_rviz_overlay::DisplayWrapper, rviz::Display )
