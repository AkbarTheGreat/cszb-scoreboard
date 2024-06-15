/*
ui/widget/Frame.cpp: A frame represents a free-standing window that is not a
dialog.

Copyright 2021-2024 Tracy Beck

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

#include "ui/widget/Frame.h"

#include <wx/menu.h>      // IWYU pragma: keep for wxMenuBar, wxMenu (ptr only)
#include <wx/toplevel.h>  // for wxSTAY_ON_TOP

#include "config/Position.h"  // for Position, Size
#include "wx/window.h"        // for wxWindow
// IWYU pragma: no_include <wx/gtk/menu.h>

namespace cszb_scoreboard {

Frame::~Frame() {
  if (should_self_delete) {
    _wx->Destroy();
  }
}
void Frame::alwaysOnTop(bool isOnTop) {
  if (isOnTop) {
    wx()->SetWindowStyle(wx()->GetWindowStyle() | wxSTAY_ON_TOP);
  } else {
    int64_t styleMask = 0L ^ wxSTAY_ON_TOP;
    wx()->SetWindowStyle(wx()->GetWindowStyle() & styleMask);
  }
}

void Frame::fullScreen(bool show) {
  _wx->EnableFullScreenView(true);
  _wx->ShowFullScreen(show);
}

void Frame::menuBar(const std::vector<MenuCategory> &menu) {
  auto *menu_bar = new wxMenuBar();
  for (const auto &category : menu) {
    auto *menu_cat = new wxMenu;
    for (auto item : category.items) {
      if (item.name) {
        menu_cat->Append(item.id, *item.name, *item.description);
      } else {
        menu_cat->Append(item.id);
      }
    }
    menu_bar->Append(menu_cat, category.name);
  }
  _wx->SetMenuBar(menu_bar);
}

void Frame::setDimensions(const Position &position, const Size &size) {
  wx()->SetSize(size.toWx());
  wx()->SetPosition(position.toWx());
}

void Frame::setStatusBar(const wxString &text) {
  if (!has_status_bar) {
    has_status_bar = true;
    _wx->CreateStatusBar();
  }
  _wx->SetStatusText(text);
}

}  // namespace cszb_scoreboard
