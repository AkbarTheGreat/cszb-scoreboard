/*
ui/dialog/EditImageLibraryDialog.cpp: A dialog which allows a user to
add/remove/edit images in the image library.

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

#include "ui/dialog/EditImageLibraryDialog.h"

#include <wx/arrstr.h>    // for wxArrayString
#include <wx/bookctrl.h>  // for wxBookCtrlBase
#include <wx/defs.h>      // for wxID_ANY, wxALL
#include <wx/editlbox.h>  // for wxEditableListBox
#include <wx/event.h>     // for wxEventTypeTag
#include <wx/listbase.h>  // for wxListEvent
#include <wx/panel.h>     // IWYU pragma: keep for wxPanel
#include <wx/panel.h>     // for wxPanel
#include <wx/sizer.h>     // for wxSizer
#include <wx/stattext.h>  // IWYU pragma: keep for wxStaticText
#include <wx/textctrl.h>  // IWYU pragma: keep for wxTextCtrl

#include <algorithm>   // for max
#include <filesystem>  // for operator<
#include <string>      // for string, basic_...
#include <vector>      // for vector

#include "ScoreboardCommon.h"                          // for DEFAULT_BORDER...
#include "config/ImageLibrary.h"                       // for ImageLibrary
#include "ui/UiUtil.h"                                 // for UiUtil
#include "ui/dialog/edit_image_library/FileListBox.h"  // for FileListBox
#include "wx/window.h"                                 // for wxWindow
// IWYU pragma: no_include: <wx/generic/panelg.h>
// IWYU pragma: no_include: <wx/gtk/stattext.h>
// IWYU pragma: no_include: <wx/gtk/textctrl.h>

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

EditImageLibraryDialog::EditImageLibraryDialog(swx::PropertySheetDialog *wx,
                                               Panel *parent)
    : TabbedDialog(wx) {
  this->parent = parent;

  images = ImageLibrary::getInstance()->imageMap();
  box_panel = panel();
  file_list =
      std::make_unique<FileListBox>(box_panel->childPanel(), "Filename");

  name_entry = new wxTextCtrl(box_panel->wx, wxID_ANY);
  name_label = new wxStaticText(box_panel->wx, wxID_ANY, "Display name");

  tag_list = new wxEditableListBox(box_panel->wx, wxID_ANY, "Tags");

  positionWidgets();
  bindEvents();
}

void EditImageLibraryDialog::positionWidgets() {
  box_panel->addWidget(*file_list, 0, 0);
  UiUtil::addToGridBag(box_panel->sizer(), tag_list, 0, 1);
  UiUtil::addToGridBag(box_panel->sizer(), name_label, 1, 0);
  UiUtil::addToGridBag(box_panel->sizer(), name_entry, 1, 1);

  // CreateButtons(wxOK | wxCANCEL);

  box_panel->runSizer();

  addPage(*box_panel, "");

  runSizer();
}

void EditImageLibraryDialog::bindEvents() {
  bind(
      wxEVT_BUTTON,
      [this](wxCommandEvent &event) -> void { this->onOk(event); }, wxID_OK);
  bind(
      wxEVT_BUTTON,
      [this](wxCommandEvent &event) -> void { this->onCancel(event); },
      wxID_CANCEL);
  bind(wxEVT_CLOSE_WINDOW,
       [this](wxCloseEvent &event) -> void { this->onClose(event); });
  file_list->bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent &event) -> void {
    this->fileSelected(event);
  });
  name_entry->Bind(wxEVT_KEY_UP, &EditImageLibraryDialog::nameUpdated, this);
  tag_list->Bind(wxEVT_LIST_END_LABEL_EDIT,
                 &EditImageLibraryDialog::tagsUpdated, this);
  tag_list->Bind(wxEVT_LIST_DELETE_ITEM, &EditImageLibraryDialog::tagDeleted,
                 this);
}

void EditImageLibraryDialog::onOk(wxCommandEvent &event) {
  if (validateSettings()) {
    saveSettings();
    close();
    return;
  }
}

void EditImageLibraryDialog::onCancel(wxCommandEvent &event) { close(); }

void EditImageLibraryDialog::onClose(wxCloseEvent &event) {
  // Sometimes closing out this menu has given focus to a totally different
  // window for focus for me in testing.  That's really obnoxious, because it
  // can have the effect of sending the main window to the back of another
  // window by virtue of exiting a dialog.  To top that off, if you set focus
  // before calling Destroy(), things quit working.  But Destroying calls the
  // destructor, so we can't rely on this->parent anymore after Destroy is
  // called.  So we save it in a local pointer temporarily for this purpose.
  Panel *local_parent = parent;
  selfDestruct();
  local_parent->focus();
}

auto EditImageLibraryDialog::validateSettings() -> bool { return true; }

void EditImageLibraryDialog::saveSettings() {
  ImageLibrary::getInstance()->clearLibrary();
  for (const auto &filename : file_list->getFilenames()) {
    std::vector<std::string> tags;
    for (const auto &tag : images[filename].tags()) {
      // Strip out empty tags that're left by accident.
      if (!tag.empty()) {
        tags.push_back(tag);
      }
    }
    ImageLibrary::getInstance()->addImage(filename, images[filename].name(),
                                          tags);
  }
  ImageLibrary::getInstance()->saveLibrary();
}

void EditImageLibraryDialog::fileSelected(wxListEvent &event) {
  FilesystemPath filename = file_list->selectedFilename();

  name_entry->SetValue(images[filename].name());
  wxArrayString tags;
  for (const auto &tag : images[filename].tags()) {
    tags.Add(tag);
  }
  tag_list->SetStrings(tags);
  event.Skip();
}

void EditImageLibraryDialog::nameUpdated(wxKeyEvent &event) {
  images[file_list->selectedFilename()].set_name(name_entry->GetValue());
}

void EditImageLibraryDialog::tagDeleted(wxListEvent &event) {
  wxArrayString tags;
  tag_list->GetStrings(tags);

  tags.RemoveAt(event.GetIndex(), 1);

  tag_list->SetStrings(tags);

  FilesystemPath filename = file_list->selectedFilename();
  images[filename].clear_tags();
  for (const auto &tag : tags) {
    images[filename].add_tags(tag);
  }
}

void EditImageLibraryDialog::tagsUpdated(wxListEvent &event) {
  wxArrayString tags;
  tag_list->GetStrings(tags);
  int index = event.GetIndex();
  if (index >= tags.size()) {
    tags.Add(event.GetText());
  } else {
    tags[index] = event.GetText();
  }
  tag_list->SetStrings(tags);

  FilesystemPath filename = file_list->selectedFilename();
  images[filename].clear_tags();
  for (const auto &tag : tags) {
    images[filename].add_tags(tag);
  }
}

}  // namespace cszb_scoreboard
