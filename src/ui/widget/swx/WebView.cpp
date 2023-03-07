/*
ui/widget/swx/WebView.cpp: A wrapper around wxWebView.  Because wxWebView is
constructed via a factory, the easiest way to contain wx code is via containment
rather than extension.  It's not my favorite thing to do, but it's what I'm
doing for this one class, for consistency with other classes.

Copyright 2022-2023 Tracy Beck

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

#include "ui/widget/swx/WebView.h"

namespace cszb_scoreboard::swx {

#ifdef SCOREBOARD_ENABLE_IMAGE_SEARCH

#ifdef _WIN32
#define WX_WEB_ENGINE                                                        \
  wxWebView::IsBackendAvailable(wxWebViewBackendEdge) ? wxWebViewBackendEdge \
                                                      : wxWebViewBackendWebKit
#else  // #ifdef _WIN32
#define WX_WEB_ENGINE wxWebViewBackendDefault
#endif  // #ifdef _WIN32

WebView::WebView(wxWindow *parent, const wxString &url, wxWindowID id,
                 const wxPoint &pos, const wxSize &size,
                 const wxString &backend, int64_t style, const wxString &name) {
  _wx = wxWebView::New(parent, id, url, pos, size, WX_WEB_ENGINE, style, name);
#ifdef SCOREBOARD_DEBUG
  // Allow access to the developer console in debug mode.
  if (_wx) {
    _wx->EnableAccessToDevTools(true);
  }
#endif
}

void WebView::Bind(const wxEventTypeTag<wxWebViewEvent> &eventType,
                   const std::function<void(wxWebViewEvent &)> &lambda,
                   int id) const {
  if (valid()) {
    _wx->Bind(eventType, lambda, id);
  }
}

void WebView::LoadURL(const std::string &url) {
  if (valid()) {
    _wx->LoadURL(url);
  }
}

void WebView::RunScript(const std::string &script) {
  if (valid()) {
    _wx->RunScript(script);
  }
}

#endif  // #ifdef SCOREBOARD_ENABLE_IMAGE_SEARCH

}  // namespace cszb_scoreboard::swx
