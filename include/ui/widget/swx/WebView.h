/*
ui/widget/swx/WebView.h: A wrapper around wxWebView.  Because wxWebView is
constructed via a factory, the easiest way to contain wx code is via containment
rather than extension.  It's not my favorite thing to do, but it's what I'm
doing for this one class, for consistency with other classes.

Copyright 2022 Tracy Beck

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

#include <wx/filesys.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>
#include <wx/wx.h>

#include "util/Log.h"

namespace cszb_scoreboard::swx {

class WebView {
 public:
  explicit WebView(wxWindow *parent,
                   const wxString &url = wxWebViewDefaultURLStr,
                   wxWindowID id = wxID_ANY,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   const wxString &backend = wxWebViewBackendDefault,
                   int64_t style = 0, const wxString &name = wxWebViewNameStr) {

#ifdef _WIN32
    _wx = wxWebView::New(parent, id, url, pos, size, wxWebViewBackendEdge, style, name);
    // Log backend information
    LogDebug("Backend: %s Version: %s", _wx->GetClassInfo()->GetClassName(),
             wxWebView::GetBackendVersionInfo().ToString());
    LogDebug("User Agent: %s", _wx->GetUserAgent());
#else // #ifdef _WIN32
    _wx = wxWebView::New(parent, id, url, pos, size, wxWebViewBackendDefault, style, name);
#endif // #ifdef _WIN32
  }

  auto wx() -> wxWebView * { return _wx; }

  void LoadURL(const std::string &url) { _wx->LoadURL(url); }

 private:
  wxWebView *_wx;
};

}  // namespace cszb_scoreboard::swx
