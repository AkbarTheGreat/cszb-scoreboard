/*
ui/widget/Browser.h: An embedded web browser in the application.

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

#include "ui/widget/Widget.h"
#include "ui/widget/swx/WebView.h"

namespace cszb_scoreboard {

class Browser : public Widget {
 public:
  explicit Browser(swx::WebView *web_view) { _wx = web_view; }
  void setURL(const std::string &url) { _wx->LoadURL(url); }
  void runJavascript(const std::string &script) { _wx->RunScript(script); }
  void bind(const wxEventTypeTag<wxWebViewEvent> &eventType,
            const std::function<void(wxWebViewEvent &)> &lambda,
            int id = wxID_ANY) const {
    _wx->Bind(eventType, lambda, id);
  }
  auto valid() -> bool { return _wx->valid(); }

 protected:
  [[nodiscard]] auto wx() const -> wxWindow * override { return _wx->wx(); }

 private:
  swx::WebView *_wx;
};

}  // namespace cszb_scoreboard
