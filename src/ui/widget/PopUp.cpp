/*
ui/widget/PopUp.cpp: A simple pop-up message as a fire & forget method.

Copyright 2021-2025 Tracy Beck

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

#include "ui/widget/PopUp.h"

#include <wx/defs.h>    // for wxCENTRE, wxICON_ERROR, wxICON_EXCLAMATION
#include <wx/msgdlg.h>  // for wxMessageBox

#include <cstdint>  // for int64_t

namespace cszb_scoreboard {

auto BoxWithIcon(const std::string &title, const std::string &msg, int64_t icon,
                 int64_t buttons = wxOK) -> bool {
  // wxMessageBox puts msg & title in a weird order in my mind, so this is the
  // one place that's message first.
  return wxMessageBox(msg, title, buttons | wxCENTRE | icon) == wxOK;
}

auto PopUp::Confirmation(const std::string &title, const std::string &msg)
    -> bool {
  return BoxWithIcon(title, msg, wxICON_EXCLAMATION, wxOK | wxCANCEL);
}

void PopUp::Error(const std::string &title, const std::string &msg) {
  BoxWithIcon(title, msg, wxICON_ERROR);
}

void PopUp::Info(const std::string &title, const std::string &msg) {
  BoxWithIcon(title, msg, wxICON_INFORMATION);
}

void PopUp::Message(const std::string &msg) { wxMessageBox(msg); }

void PopUp::Warning(const std::string &title, const std::string &msg) {
  BoxWithIcon(title, msg, wxICON_EXCLAMATION);
}

}  // namespace cszb_scoreboard
