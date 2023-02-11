/*
ui/widget/PopUp.h: A simple pop-up message as a fire & forget method.

Copyright 2021-2023 Tracy Beck

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

#include <string>

namespace cszb_scoreboard {

class PopUp {
 public:
  static void Error(const std::string &title, const std::string &msg);
  static void Info(const std::string &title, const std::string &msg);
  static void Message(const std::string &msg);
  static void Warning(const std::string &title, const std::string &msg);
  // Returns true if the user agrees, false if they cancel.
  static auto Confirmation(const std::string &title, const std::string &msg)
      -> bool;
};

}  // namespace cszb_scoreboard
