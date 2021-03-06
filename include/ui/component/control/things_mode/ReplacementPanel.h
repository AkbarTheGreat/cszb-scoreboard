/*
ui/component/control/things_mode/ReplacementPanel.h: Represents all replacements
for one activity in 5/6 things.

Copyright 2019-2021 Tracy Beck

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

#include <vector>

#include "config.pb.h"
#include "ui/component/control/things_mode/Replacement.h"

namespace cszb_scoreboard {
class ReplacementPanel : public wxPanel {
 public:
  ReplacementPanel(wxWindow *parent, wxWindow *activity_panel);
  void copyFrom(ReplacementPanel *other);
  void addReplacement();
  void deleteReplacement(wxCommandEvent &event);  // NOLINT(google-runtime-references)
                                                  // wxWidgets callback.
  auto previewText(int font_size) -> std::vector<proto::RenderableText>;
  void textUpdated(wxKeyEvent &event);  // NOLINT(google-runtime-references)
                                        // wxWidgets callback.

 private:
  wxWindow *activity_panel;
  std::vector<std::shared_ptr<Replacement>> replacements;

  void bindEvents();
  void positionWidgets();
  void updateNotify();
};
}  // namespace cszb_scoreboard
