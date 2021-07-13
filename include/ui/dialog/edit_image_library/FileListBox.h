/*
ui/dialog/edit_image_library/FileListBox.h: A subclass of wxEditableListBox
which specifically contains a list of files.

Copyright 2020-2021 Tracy Beck

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

#include <stdint.h>       // for int32_t
#include <wx/defs.h>      // for wxID_ANY
#include <wx/editlbox.h>  // for wxEL_ALLOW_DELETE, wxEL_ALLOW_NEW
#include <wx/event.h>     // for wxCommandEvent (ptr only), wxEventT...

#include <functional>  // for function
#include <memory>      // for unique_ptr
#include <string>      // for string
#include <vector>      // for vector

#include "ui/widget/ListBox.h"    // for ListBox
#include "ui/widget/Panel.h"      // for Panel
#include "util/FilesystemPath.h"  // for FilesystemPath

class wxListEvent;

namespace cszb_scoreboard {
namespace swx {
class Panel;
}  // namespace swx

const int32_t FILE_LIST_BOX_DEFAULT_STYLE = wxEL_ALLOW_NEW | wxEL_ALLOW_DELETE;

class FileListBox : public Panel {
 public:
  FileListBox(swx::Panel *wx, const std::string &title);

  void bind(const wxEventTypeTag<wxListEvent> &eventType,
            const std::function<void(wxListEvent &)> &lambda,
            int id = wxID_ANY) {
    box->bind(eventType, lambda, id);
  }
  auto getFilenames() -> std::vector<FilesystemPath>;
  auto selectedFilename() -> FilesystemPath;

 protected:
  void bindEvents();
  void newPressed(wxCommandEvent &event);  // NOLINT(google-runtime-references)
                                           // wxWidgets callback.
  void updateStrings(const std::vector<FilesystemPath> &filenames,
                     int32_t select_index = 0);

 private:
  std::unique_ptr<ListBox> box;
};

}  // namespace cszb_scoreboard
