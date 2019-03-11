//
// Created by Stefan Fabian on 07.03.19.
//

#ifndef OPENGL_RVIZ_OVERLAY_DISPLAY_WRAPPER_H
#define OPENGL_RVIZ_OVERLAY_DISPLAY_WRAPPER_H

#include "overlay.h"

#include <rviz/display.h>

namespace qt_rviz_overlay
{

class DisplayWrapper : public rviz::Display
{
Q_OBJECT
public:
  DisplayWrapper();

  ~DisplayWrapper() override;

protected:

  /*!
   * Creates the Overlay instance using createOverlay() and tries to add it to the OverlayManager.
   * @see rviz::Display::onInitialize()
   */
  void onInitialize() override;

  /*!
   * Shows the Overlay.
   * @see rviz::Display::onEnable()
   */
  void onEnable() override;

  /*!
   * Hides the Overlay.
   * @see rviz::Display::onDisable()
   */
  void onDisable() override;

  /*! A pointer to an instance of Overlay which is created in onInitialize(). */
  std::shared_ptr<qt_rviz_overlay::Overlay> overlay_;
};
} // qt_rviz_overlay

#endif //OPENGL_RVIZ_OVERLAY_DISPLAY_WRAPPER_H
