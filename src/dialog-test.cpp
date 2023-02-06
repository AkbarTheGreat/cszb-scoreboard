/*
dialog-test.cpp : Defines the entry point for the application.

Copyright 2022-2023 Tracy Beck

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

#include <wx/app.h>    // for wxIMPLEMENT_APP
#include <wx/defs.h>   // for wxID_ANY
#include <wx/event.h>  // for wxCloseEvent (ptr only)

#include <cstdlib>  // for exit
#include <memory>   // for unique_ptr, make_unique

#include "config/Position.h"                    // for Position, Size
#include "ui/dialog/SettingsDialog.h"           // for SettingsDialog
#include "ui/widget/Frame.h"                    // for Frame
#include "ui/widget/Panel.h"                    // for Panel
#include "ui/widget/swx/PropertySheetDialog.h"  // for PropertySheetDialog
#include "util/Singleton.h"                     // for Singleton
// IWYU pragma: no_include <wx/unix/app.h>
// IWYU pragma: no_include <wx/gtk/app.h>

namespace cszb_scoreboard {

static constexpr int START_X = 50;
static constexpr int START_Y = 50;
static constexpr int START_WIDTH = 700;
static constexpr int START_HEIGHT = 500;

class TestFrame : public Frame {
 public:
  std::unique_ptr<Panel> panel;
  TestFrame()
      : Frame("Testing", /*self_managed =*/false,
              Position{.x = START_X, .y = START_Y},
              Size{.width = START_WIDTH, .height = START_HEIGHT}) {
    panel = std::make_unique<Panel>(childPanel());
    addWidget(*panel, 0, 0);
    runSizer();
  }
};

class DialogTest : public wxApp {
 public:
  auto OnInit() -> bool final;

 private:
  std::unique_ptr<TestFrame> frame;
  // std::unique_ptr<EditImageLibraryDialog> dialog;
  std::unique_ptr<SettingsDialog> dialog;
};

auto DialogTest::OnInit() -> bool {
  if (!wxApp::OnInit()) {
    return false;
  }
  frame = std::make_unique<TestFrame>();
  frame->show(true);
  Singleton::getInstance()->persistence();
  auto *prop_sheet =
      new swx::PropertySheetDialog(frame->panel->wx(), wxID_ANY, "Test");
  // dialog = std::make_unique<EditImageLibraryDialog>(prop_sheet, nullptr);
  dialog = std::make_unique<SettingsDialog>(prop_sheet, nullptr);
  dialog->bind(wxEVT_CLOSE_WINDOW,
               [this](wxCloseEvent &event) -> void { std::exit(0); });
  dialog->show();
  return true;
}

}  // namespace cszb_scoreboard

wxIMPLEMENT_APP(cszb_scoreboard::DialogTest);
