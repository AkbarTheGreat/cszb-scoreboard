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
//#include <wx/msw/webview_edge.h>
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
    // Check if a fixed version of edge is present in
    // $executable_path/edge_fixed and use it.
    // TODO(akbar): I believe this is required for Windows usage of WebKit2, but verify it at some point.
    /*
    wxFileName edgeFixedDir(wxStandardPaths::Get().GetExecutablePath());
    edgeFixedDir.SetFullName("");
    edgeFixedDir.AppendDir("edge_fixed");
    if (edgeFixedDir.DirExists()) {
      wxWebViewEdge::MSWSetBrowserExecutableDir(edgeFixedDir.GetFullPath());
      wxLogMessage("Using fixed edge version");
    }
    */
    //_wx = wxWebView::New(parent, id, url, pos, size, backend, style, name);

    _wx = wxWebView::New(parent, id, url, pos, size, wxWebViewBackendEdge, style, name);
    //_wx = wxWebView::New();
    //_wx->Create(parent, id, url, pos, size);

    // Log backend information
    /* Broken on Linux -- disabling temporarily 
    LogDebug("Backend: %s Version: %s", _wx->GetClassInfo()->GetClassName(),
             wxWebView::GetBackendVersionInfo().ToString());
    LogDebug("User Agent: %s", _wx->GetUserAgent());
    */
  }

  auto wx() -> wxWebView * { return _wx; }

  void LoadURL(const std::string &url) { _wx->LoadURL(url); }

 private:
  wxWebView *_wx;
};

}  // namespace cszb_scoreboard::swx
