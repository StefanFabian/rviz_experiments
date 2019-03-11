//
// Created by Stefan Fabian on 07.03.19.
//
// This file is just a result from experimenting with some code I've found
// which I modified until I got something working.
// I've cleaned up the result in the project where I used this functionality
// This file, however, isn't clean or minimal in any way.
// But it should work in Ubuntu 16.04 with ROS Kinetic.

#include "qt_rviz_overlay/overlay.h"


#include <QMetaType>
#include <QSurfaceFormat>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>


#include <OgreRenderSystem.h>
#include <OgreRenderWindow.h>
#include <OgreManualObject.h>
#include <OgreSceneManager.h>

#include <rviz/render_panel.h>
#include <GL/glx.h>
#include <ros/ros.h>

#include <QtPlatformHeaders/QGLXNativeContext>

#include <chrono>

namespace qt_rviz_overlay
{

struct Overlay::NativeContextInformation
{
  GLXDrawable drawable;
  GLXContext context;
  Display *display;
};

Overlay::Overlay( Ogre::Node *object, Ogre::SceneManager *sceneMgr, rviz::RenderPanel *render_panel ) :
  scene_node_(object), scene_manager_(sceneMgr), context_(nullptr), surface_(nullptr), fbo_(nullptr),
  paint_device_( nullptr), timer_average_( 0 ), timer_index_( 0 )
{
  std::fill( timer_history_, timer_history_ + 15, 0.0 );
  native_context_information_ = new NativeContextInformation;
  render_panel->getRenderWindow()->getCustomAttribute( "DISPLAY", &native_context_information_->display );
}

Overlay::~Overlay() {
  delete context_;
  delete surface_;
  delete fbo_;
  delete paint_device_;
}

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

  // save matrices
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
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
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void Overlay::nativeRender() {
  if (!visible_) return;
  auto start = std::chrono::high_resolution_clock::now();

  native_context_information_->drawable = glXGetCurrentDrawable();
  native_context_information_->context = glXGetCurrentContext();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glLoadIdentity(); //Texture addressing should start out as direct.
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  if (context_ == nullptr)
  {
    QSurfaceFormat format;

    format.setDepthBufferSize( 16 );
    format.setStencilBufferSize( 8 );
    format.setMajorVersion(3);
    format.setMinorVersion(3);

    native_context_ = new QOpenGLContext;
    native_context_->setNativeHandle( QVariant::fromValue(
      QGLXNativeContext( native_context_information_->context, native_context_information_->display )));
//  context_->setFormat( format );

    if ( !native_context_->create())
    {
      ROS_ERROR( "OverlayManager: Fatal! Failed to create context!" );
    }

    context_ = new QOpenGLContext;
    context_->setFormat( format );
    context_->setShareContext( native_context_ );

    if ( !context_->create())
    {
      ROS_ERROR( "OverlayManager: Fatal! Failed to create context!" );
    }

    surface_ = new QOffscreenSurface;
    surface_->setFormat( format );
    surface_->create();
    if ( !surface_->isValid()) ROS_ERROR( "Surface invalid!" );

    context_->makeCurrent( surface_ );

    paint_device_ = new QOpenGLPaintDevice( 1920, 1080 );

    if (!QOpenGLFramebufferObject::hasOpenGLFramebufferBlit()) ROS_WARN("No blit stuff");

    {
      QOpenGLFramebufferObjectFormat format;
      format.setSamples(16);
      format.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );
      fbo_ = new QOpenGLFramebufferObject( 1920, 1080, format );
      texture_fbo_ = new QOpenGLFramebufferObject( 1920, 1080 );
    }
    fbo_->bind();
  }
  else
  {
    context_->makeCurrent( surface_ );
    fbo_->bind();
  }
  context_->functions()->glClear(GL_COLOR_BUFFER_BIT);

  QPainter painter(paint_device_);
  painter.setRenderHint(QPainter::RenderHint::Antialiasing);
  painter.setBrush(QBrush(Qt::green));
  painter.drawRect(0, 0, 400, 300);
//  painter.setPen(Qt::red);
  painter.setPen(QPen(QBrush(Qt::red), 4));
  painter.setFont(QFont("Arial", 20));
  painter.drawText(100, 120, "Hello");
  painter.drawText( 10, 80, QString( "Rendertime (ms): %1" ).arg( timer_average_ / 15.0 ));
  painter.end();
  fbo_->release();
  context_->functions()->glFinish();
  QOpenGLFramebufferObject fbo(1920, 1080);
  QOpenGLFramebufferObject::blitFramebuffer(texture_fbo_, fbo_);
//  texture_fbo_->toImage().save("/home/stefan/test.png", "png");
  context_->doneCurrent();
  glXMakeCurrent( native_context_information_->display, native_context_information_->drawable,
                  native_context_information_->context );


  // restore original state
  glPopAttrib();

  // restore matrices
  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glOrtho(0, 1920, 0, 1080, -1, 1);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_fbo_->texture());

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(0, 0);
  glTexCoord2f(0, 1); glVertex2f(0, 1080);
  glTexCoord2f(1, 1); glVertex2f(1920, 1080);
  glTexCoord2f(1, 0); glVertex2f(1920, 0);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnd();

  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glMatrixMode(GL_MODELVIEW);


  auto end = std::chrono::high_resolution_clock::now();
  timer_average_ -= timer_history_[timer_index_];
  timer_history_[timer_index_] = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count() / 1000.0;
  timer_average_ += timer_history_[timer_index_];
  timer_index_ = (timer_index_ + 1) % 15;
}
}
