/*
ui/widget/swx/MediaCtrl.h: A wrapper around wxMediaCtrl.

Copyright 2023-2025 Tracy Beck

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

#include <wx/mediactrl.h>
#include <wx/wx.h>

#ifdef __WXMSW__
#include "wx/link.h"
wxFORCE_LINK_MODULE(wxmediabackend_am);
wxFORCE_LINK_MODULE(wxmediabackend_qt);
wxFORCE_LINK_MODULE(wxmediabackend_wmp10);
#endif  // #ifdef __WXMSW__

namespace cszb_scoreboard::swx {

#define BACKEND ""
// #define BACKEND "wxAMMediaBackend"
// #define BACKEND "wxWMP10MediaBackend"
// #define BACKEND "wxQTMediaBackend"

class MediaCtrl : public wxMediaCtrl {
 public:
  MediaCtrl(wxWindow* parent, wxWindowID id, const wxString& fileName = wxT(""),
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, int64_t style = 0,
            const wxString& szBackend = wxT(BACKEND),
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPanelNameStr)
      : wxMediaCtrl(parent, id, fileName, pos, size, style, szBackend,
                    validator, name) {}
};

}  // namespace cszb_scoreboard::swx
