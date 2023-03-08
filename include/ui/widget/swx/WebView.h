/*
ui/widget/swx/WebView.h: A wrapper around wxWebView.  Because wxWebView is
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

#pragma once

#include <wx/defs.h>      // for wxID_ANY, wxWindowID
#include <wx/gdicmn.h>    // for wxPoint, wxSize (ptr only)
#include <wx/string.h>    // for wxString
#include <wx/tbarbase.h>  // for wxDefaultPosition, wxDefaultSize
#include <wx/webview.h>   // IWYU pragma: keep

#include <cstdint>     // for int64_t
#include <functional>  // for function
#include <string>      // for string

class wxWindow;
template <typename T>
class wxEventTypeTag;

namespace cszb_scoreboard::swx {

#ifdef SCOREBOARD_ENABLE_IMAGE_SEARCH

class WebView {
 public:
  explicit WebView(wxWindow *parent,
                   const wxString &url = wxWebViewDefaultURLStr,
                   wxWindowID id = wxID_ANY,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   const wxString &backend = wxWebViewBackendDefault,
                   int64_t style = 0, const wxString &name = wxWebViewNameStr);

  [[nodiscard]] auto valid() const -> bool { return (_wx != nullptr); }
  auto wx() -> wxWebView * { return _wx; }

  void Bind(const wxEventTypeTag<wxWebViewEvent> &eventType,
            const std::function<void(wxWebViewEvent &)> &lambda,
            int id = wxID_ANY) const;
  void LoadURL(const std::string &url);
  void RunScript(const std::string &script);

 private:
  wxWebView *_wx;
};

#else  // #ifdef SCOREBOARD_ENABLE_IMAGE_SEARCH

// Stub to satisfy a linker, intentionally non-functional
class WebView {
 public:
  explicit WebView(wxWindow *parent, const wxString &url = "",
                   wxWindowID id = wxID_ANY,
                   const wxPoint &pos = wxDefaultPosition,
                   const wxSize &size = wxDefaultSize,
                   const wxString &backend = "", int64_t style = 0,
                   const wxString &name = "") {}
  auto valid() const -> bool { return false; }
  auto wx() -> wxWebView * { return nullptr; }
  void Bind(const wxEventTypeTag<wxWebViewEvent> &eventType,
            const std::function<void(wxWebViewEvent &)> &lambda,
            int id = wxID_ANY) const {}
  void LoadURL(const std::string &url) {}
  void RunScript(const std::string &script) {}
};

#endif  // #ifdef SCOREBOARD_ENABLE_IMAGE_SEARCH

}  // namespace cszb_scoreboard::swx
