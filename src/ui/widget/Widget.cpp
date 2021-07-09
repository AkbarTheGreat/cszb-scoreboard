/*
ui/widget/Widget.cpp: Methods common to all of our widgets.

Copyright 2021 Tracy Beck

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

#include "ui/widget/Widget.h"

namespace cszb_scoreboard {

void Widget::addWidgetWithSpan(const Widget &widget, int row, int column,
                               int row_span, int column_span, int border_size,
                               int flag) {
  sizer()->Add(widget._wx(), wxGBPosition(row, column),
               wxGBSpan(row_span, column_span), flag, border_size);
}

auto Widget::widgetAtIndex(int row, int column) -> wxWindow * {
  if (window_sizer == nullptr) {
    return nullptr;
  }
  wxGBSizerItem *item =
      window_sizer->FindItemAtPosition(wxGBPosition(row, column));
  if (item == nullptr || !item->IsWindow()) {
    return nullptr;
  }
  return item->GetWindow();
}

void Widget::moveWidget(wxWindow *widget, int row, int column) {
  sizer()->SetItemPosition(widget, wxGBPosition(row, column));
}

void Widget::removeColumnFromSizer(int column) {
  if (window_sizer == nullptr) {
    return;
  }
  for (int i = 0; i < sizer()->GetRows(); i++) {
    wxWindow *entry = widgetAtIndex(i, column);
    if (entry != nullptr) {
      sizer()->Detach(entry);
    }
  }
  for (int c = column + 1; c < sizer()->GetCols(); c++) {
    for (int r = 0; r < sizer()->GetRows(); r++) {
      wxWindow *item = widgetAtIndex(r, c);
      if (item != nullptr) {
        sizer()->SetItemPosition(item, wxGBPosition(r, c - 1));
      }
    }
  }
}

void Widget::removeRowFromSizer(int row) {
  if (window_sizer == nullptr) {
    return;
  }
  for (int i = 0; i < sizer()->GetCols(); i++) {
    wxWindow *entry = widgetAtIndex(row, i);
    if (entry != nullptr) {
      sizer()->Detach(entry);
    }
  }
  for (int r = row + 1; r < sizer()->GetRows(); r++) {
    for (int c = 0; c < sizer()->GetCols(); c++) {
      wxWindow *item = widgetAtIndex(r, c);
      if (item != nullptr) {
        sizer()->SetItemPosition(item, wxGBPosition(r - 1, c));
      }
    }
  }
}

auto Widget::sizer() -> swx::Sizer * {
  if (window_sizer == nullptr) {
    window_sizer = new swx::Sizer();
  }
  return window_sizer;
}

}  // namespace cszb_scoreboard
