/*
ui/component/control/ThingsMode.cpp: A control for managing 5/6 things in a
short-form improv show.

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

#include "ui/component/control/ThingsMode.h"

#include "config/TeamConfig.h"
#include "util/ProtoUtil.h"
#include "util/StringUtil.h"

namespace cszb_scoreboard {

const int DEFAULT_FONT_SIZE = 10;
const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

ThingsMode *ThingsMode::Create(PreviewPanel *preview_panel, wxWindow *parent) {
  ThingsMode *entry = new ThingsMode(preview_panel, parent);
  entry->initializeWidgets();
  entry->updatePreview();
  return entry;
}

void ThingsMode::createControls(wxPanel *control_panel) {
  scrollable_panel = new wxScrolledWindow(
      control_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);

  aui_manager.SetManagedWindow(scrollable_panel);

  screen_selection = new TeamSelector(scrollable_panel);

  new_activity_button =
      new wxButton(scrollable_panel, wxID_ANY, "New Activity");
  new_replacement_button =
      new wxButton(scrollable_panel, wxID_ANY, "New Replacement");

  home_activities_panel = new ActivityPanel(scrollable_panel, this);
  // away_activities_panel = new ActivityPanel(scrollable_panel, this);
  // all_activities_panel = new ActivityPanel(scrollable_panel, this);

  positionWidgets(control_panel);
  bindEvents();
}

void ThingsMode::addToScrollable(wxWindow *item, int row) {
  wxAuiPaneInfo info;
  info.CenterPane();
  // info.Row(row);
  switch (row) {
    case 0:
      info.Top();
      break;
    case 1:
      info.Center();
      break;
    default:
      info.Bottom();
  }
  info.MinSize(item->GetSize());
  aui_manager.AddPane(item, info);
}

void ThingsMode::positionWidgets(wxPanel *control_panel) {
  wxFlexGridSizer *outer_sizer = new wxFlexGridSizer(0, 1, 0, 0);
  outer_sizer->SetFlexibleDirection(wxBOTH);
  outer_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  addToScrollable(new_activity_button, 0);
  addToScrollable(new_replacement_button, 0);

  addToScrollable(home_activities_panel, 1);

  addToScrollable(screen_selection, 2);

  // aui_manager.DetachPane(away_activities_panel);
  // aui_manager.DetachPane(all_activities_panel);

  aui_manager.Update();
  // scrollable_panel->SetSizer(scrollable_sizer);
  // scrollable_panel->FitInside();
  scrollable_panel->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);

  outer_sizer->Add(scrollable_panel, 0, wxALL, BORDER_SIZE);
  control_panel->SetSizerAndFit(outer_sizer);
}

void ThingsMode::bindEvents() {
  new_activity_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
                            &ThingsMode::addActivity, this);
  screen_selection->Bind(wxEVT_COMMAND_RADIOBOX_SELECTED,
                         &ThingsMode::screenChanged, this);
}

void ThingsMode::updatePreview() {
  aui_manager.Update();
  // Send the combined text to both previews
  if (screen_selection->allSelected()) {
    proto::ScreenSide side;
    side.set_home(true);
    side.set_away(true);
    // previewPanel()->setTextForPreview(all_text, all_font_size, all_color,
    // false, side);
  } else {
  }
}

void ThingsMode::textUpdated(wxKeyEvent &event) { updatePreview(); }

void ThingsMode::screenChanged(wxCommandEvent &event) {
  aui_manager.DetachPane(all_activities_panel);
  aui_manager.DetachPane(away_activities_panel);
  aui_manager.DetachPane(home_activities_panel);

  if (screen_selection->allSelected()) {
    addToScrollable(all_activities_panel, 1);
  } else if (screen_selection->homeSelected()) {
    addToScrollable(home_activities_panel, 1);
  } else if (screen_selection->awaySelected()) {
    addToScrollable(away_activities_panel, 1);
  }
  aui_manager.Update();

  updatePreview();
}

void ThingsMode::addActivity(wxCommandEvent &event) {
  home_activities_panel->addActivity(scrollable_panel);

  // scrollable_panel->SetSizer(scrollable_panel->GetSizer());
  scrollable_panel->FitInside();
  scrollable_panel->SetScrollRate(0, scrollable_panel->GetSize().GetHeight());
  scrollable_panel->SetSizeHints(scrollable_panel->GetSize());
  //  scrollable_panel->SetSize(scrollable_panel->GetVirtualSize().GetWidth(),
  //                            scrollable_panel->GetSize().GetHeight());

  control_panel->SetSizerAndFit(control_panel->GetSizer());
}

void ThingsMode::addReplacement(wxCommandEvent &event) {}

}  // namespace cszb_scoreboard
