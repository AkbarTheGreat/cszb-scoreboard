/*
ui/frame/FrameManager.h: This class is a singleton which manages all top
level frames (or windows) in the current application.  This class owns
all of these objects and any pointers returned from here should be considered
non-authoritative.  All frame pointers are destroyed on exitFrames and any
dangling references will then be invalid.

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

#include <set>

#include "ui/component/ScreenPresenter.h"
#include "ui/component/ScreenText.h"
#include "ui/frame/MainView.h"
#include "ui/widget/Frame.h"

namespace cszb_scoreboard {

class FrameManager {
 public:
  static auto getInstance() -> FrameManager *;
  auto createMainView(const wxString &title, const wxPoint &pos,
                      const wxSize &size) -> MainView *;
  auto createScreenPresenter(int monitor_number, const ScreenText &preview)
      -> ScreenPresenter *;
  inline auto mainView() -> MainView * { return main_view.get(); }
  void exitFrames();

 private:
  std::unique_ptr<MainView> main_view;
  std::set<std::unique_ptr<Frame>> frames;
  FrameManager() = default;
};

}  // namespace cszb_scoreboard
