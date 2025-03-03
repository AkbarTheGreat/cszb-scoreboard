/*
ui/widget/swx/DirDialog.h: A wrapper around wxDirDialog.

Copyright 2022-2025 Tracy Beck

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

#include "wx/dirdlg.h"

namespace cszb_scoreboard::swx {

class DirDialog : public wxDirDialog {
 public:
  explicit DirDialog(wxWindow *parent,
                     const wxString &message = wxDirSelectorPromptStr,
                     const wxString &defaultPath = wxEmptyString,
                     int64_t style = wxDD_DEFAULT_STYLE,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     const wxString &name = wxDirDialogNameStr)
      : wxDirDialog(parent, message, defaultPath, style, pos, size, name) {}
};

}  // namespace cszb_scoreboard::swx
