/*
ui/widget/Widget.cpp: Methods common to all of our widgets.

Copyright 2021-2023 Tracy Beck

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

#include <wx/gbsizer.h>  // for wxGBSizerItem, wxGBPosition
#include <wx/sizer.h>    // for wxSizerItemList, wxSizerItemLis...

#include <cstdint>  // for int32_t
#include <memory>   // for allocator_traits<>::value_type
#include <vector>   // for vector

#include "ui/widget/RenderContext.h"  // for RenderContext
#include "ui/widget/swx/Sizer.h"      // for Sizer
// IWYU pragma: no_include <ext/alloc_traits.h>

class wxWindow;

namespace cszb_scoreboard {

void Widget::addWidgetWithSpan(const Widget &widget, int row, int column,
                               int row_span, int column_span, int border_size,
                               int flag) {
  sizer()->Add(widget.wx(), wxGBPosition(row, column),
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

void Widget::moveWidget(Widget *widget, int row, int column) {
  sizer()->SetItemPosition(widget->wx(), wxGBPosition(row, column));
}

auto getOrderedRepresentation(wxGridBagSizer *sizer)
    -> std::vector<std::vector<const wxGBSizerItem *>> {
  std::vector<std::vector<const wxGBSizerItem *>> table;
  if (sizer == nullptr) {
    return table;
  }
  for (auto *base : sizer->GetChildren()) {
    wxGBSizerItem *item = sizer->FindItem(base->GetWindow());
    int32_t row;
    int32_t col;
    item->GetPos(row, col);
    if (row >= table.size()) {
      table.resize(row + 1);
    }
    if (col >= table[row].size()) {
      table[row].resize(col + 1);
    }
    table[row][col] = item;
  }
  return table;
}

void Widget::removeColumnFromSizer(int column) {
  if (window_sizer == nullptr) {
    return;
  }
  std::vector<std::vector<const wxGBSizerItem *>> table =
      getOrderedRepresentation(window_sizer);
  for (int r = 0; r < table.size(); r++) {
    for (int c = 0; c < table[r].size(); c++) {
      if (table[r][c] == nullptr || table[r][c]->GetWindow() == nullptr) {
        continue;
      }
      if (c == column) {
        sizer()->Detach(table[r][c]->GetWindow());
      } else if (c > column) {
        sizer()->SetItemPosition(table[r][c]->GetWindow(),
                                 wxGBPosition(r, c - 1));
      }
    }
  }
}

void Widget::removeRowFromSizer(int row) {
  if (window_sizer == nullptr) {
    return;
  }
  std::vector<std::vector<const wxGBSizerItem *>> table =
      getOrderedRepresentation(window_sizer);
  for (int r = 0; r < table.size(); r++) {
    for (int c = 0; c < table[r].size(); c++) {
      if (table[r][c] == nullptr || table[r][c]->GetWindow() == nullptr) {
        continue;
      }
      if (r == row) {
        sizer()->Detach(table[r][c]->GetWindow());
      } else if (r > row) {
        sizer()->SetItemPosition(table[r][c]->GetWindow(),
                                 wxGBPosition(r - 1, c));
      }
    }
  }
}

void Widget::resetSizer() {
  std::vector<std::vector<const wxGBSizerItem *>> table =
      getOrderedRepresentation(window_sizer);
  for (const auto &row : table) {
    for (const auto &widget : row) {
      if (widget == nullptr || widget->GetWindow() == nullptr) {
        continue;
      }
      sizer()->Detach(widget->GetWindow());
    }
  }
  wx()->SetSizerAndFit(sizer());
}

void Widget::runSizer() {
  std::vector<std::vector<const wxGBSizerItem *>> table =
      getOrderedRepresentation(window_sizer);
  for (const auto &row : table) {
    for (const auto &widget : row) {
      if (widget == nullptr || widget->GetWindow() == nullptr) {
        continue;
      }
      widget->GetWindow()->Layout();
    }
  }
  wx()->SetSizerAndFit(sizer());
}

auto Widget::sizer() -> swx::Sizer * {
  if (window_sizer == nullptr) {
    window_sizer = new swx::Sizer();
    wx()->SetSizer(window_sizer);
  }
  return window_sizer;
}

// Paint events also need to create a DC to work appropriately, so we handle
// that here.
void Widget::bind(const wxEventTypeTag<wxPaintEvent> &eventType,
                  const std::function<void(RenderContext *)> &lambda, int id) {
  wx()->Bind(
      eventType,
      [this, lambda](wxPaintEvent &event) -> void {
        std::unique_ptr<RenderContext> render_context =
            RenderContext::forEvent(wx());
        lambda(render_context.get());
      },
      id);
}

}  // namespace cszb_scoreboard
