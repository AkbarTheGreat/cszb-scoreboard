/*
ui/component/control/things_mode/ActivityPanel.h: Represents all activities in
5/6 things.

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

#include <wx/clrpicker.h>
#include <wx/wx.h>

#include <vector>

#include "config.pb.h"
#include "ui/component/control/things_mode/Activity.h"
#include "ui/graphics/Color.h"

namespace cszb_scoreboard {
class ActivityPanel : public wxPanel {
 public:
  ActivityPanel(wxWindow *parent, ScreenTextController *owning_controller,
                proto::ScreenSide side);
  ~ActivityPanel();
  void addActivity(wxPanel *parent_panel);
  void addReplacement();
  void deleteActivity(wxCommandEvent &event);
  Color getColor();
  std::vector<proto::RenderableText> previewText(int font_size);
  void refreshSizers();
  ReplacementPanel *replacementPanel();
  void selectionChanged(wxCommandEvent &event);
  std::string selectedActivityText();
  void textUpdated(wxKeyEvent &event);
  void updateNotify();

 private:
  wxPanel *activity_half;
  wxColourPickerCtrl *color_picker;
  wxPanel *replacement_half;
  proto::ScreenSide side;
  std::vector<Activity *> activities;
  ScreenTextController *owning_controller;
  wxWindow *parent;

  void bindEvents();
  void positionWidgets();
  void colorChanged(wxColourPickerEvent &event);
};
}  // namespace cszb_scoreboard
