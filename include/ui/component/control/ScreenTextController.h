/*
ui/component/control/ScreenTextController.h: Any class which extends this serves
to set information from the user into a ScreenText.  It could be arbitrary text,
a single image, or a score update, to name a few.

Copyright 2019-2020 Tracy Beck

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <wx/wx.h>

#include "config.pb.h"
#include "ui/component/PreviewPanel.h"

namespace cszb_scoreboard {

// Subclasses should create members via a Create static method which calls
// initializeWidgets() on the newly created pointer.
class ScreenTextController : public wxPanel {
 public:
  /* Called externally to request an update to the given panel.  All internally
   * triggered updates to previews should filter  through here, for consistency.
   */
  virtual void updateScreenText(ScreenText *screen_text) = 0;

  /* May be used to select the current preview panel*/
  void updatePreview();

 protected:
  /* Populate this control_panel in child classes with whatever controls this
   * ScreenTextController would like to populate to the window. */
  virtual void createControls(wxPanel *control_panel) = 0;
  /* Dictates which screen(s) will receive this change on updateClicked.
   * Defaults to all screens, may be overridden for more control. */
  ScreenTextController(PreviewPanel *preview_panel, wxWindow *parent);
  void initializeWidgets();
  auto previewPanel() -> PreviewPanel *;
  wxPanel *control_panel;

 private:
  PreviewPanel *preview_panel;
  wxButton *update_screens;

  void bindEvents();
  void positionWidgets();
  void updateClicked(
      wxCommandEvent &event);  // NOLINT(google-runtime-references)
                               // wxWidgets callback.
};

}  // namespace cszb_scoreboard
