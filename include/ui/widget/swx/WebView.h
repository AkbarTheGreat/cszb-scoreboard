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

#include <wx/webview.h>
#include <wx/wx.h>

namespace cszb_scoreboard::swx {

class WebView {
 public:
  WebView(wxWindow *parent, const wxString &url = wxWebViewDefaultURLStr,
          wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          const wxString &backend = wxWebViewBackendDefault, long style = 0,
          const wxString &name = wxWebViewNameStr) {
    _wx = wxWebView::New(parent, id, url, pos, size, backend, style, name);
  }

  wxWebView *wx() { return _wx; }

 private:
  wxWebView *_wx;
};

}  // namespace cszb_scoreboard::swx
