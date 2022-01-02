/*
ui/widget/swx/FileDialog.h: A wrapper around wxFileDialog.

Copyright 2021-2022 Tracy Beck

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

#include "wx/filedlg.h"

namespace cszb_scoreboard::swx {

class FileDialog : public wxFileDialog {
 public:
  explicit FileDialog(
      wxWindow *parent, const wxString &message = wxFileSelectorPromptStr,
      const wxString &defaultDir = wxEmptyString,
      const wxString &defaultFile = wxEmptyString,
      const wxString &wildcard = wxFileSelectorDefaultWildcardStr,
      int64_t style = wxFD_DEFAULT_STYLE,
      const wxPoint &pos = wxDefaultPosition,
      const wxSize &size = wxDefaultSize,
      const wxString &name = wxFileDialogNameStr)
      : wxFileDialog(parent, message, defaultDir, defaultFile, wildcard, style,
                     pos, size, name) {}
};

}  // namespace cszb_scoreboard::swx
