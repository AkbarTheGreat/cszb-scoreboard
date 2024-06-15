/*
util/Singleton.h: A class (a singleton itself) which manages all
other singletons.  All users should inject this in their constructor for
testability's sake, but may have a non-injected constructor which calls the
singleton version as a convenience.

Copyright 2019-2024 Tracy Beck

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

class wxCmdLineParser;
class wxCmdLineArgsArray;

namespace cszb_scoreboard {

class AutoUpdate;
class CommandArgs;
class DisplayConfig;
class FrameManager;
class GeneralConfig;
class HotkeyTable;
class ImageLibrary;
class Persistence;
class TeamColors;
class TeamConfig;
class TimerManager;

// Singletons are created with this object as a reminder to developers not to
// arbitrarily create them outside of test situations.  It's a friendly
// reminder, not a strong enforcement, but should be listened to regardless.
struct SingletonClass {};

// The Singleton class is abstract and SingletonImpl does all of the actual
// work.  This is for easier mocking without having to include the world when
// using MockSingleton.
class Singleton {
 public:
  virtual ~Singleton() = default;
  static auto getInstance() -> Singleton*;

  virtual auto autoUpdate() -> AutoUpdate* = 0;
  virtual auto commandArgs() -> CommandArgs* = 0;
  virtual auto displayConfig() -> DisplayConfig* = 0;
  virtual auto generalConfig() -> GeneralConfig* = 0;
  virtual auto frameManager() -> FrameManager* = 0;
  virtual auto hotkeyTable() -> HotkeyTable* = 0;
  virtual auto imageLibrary() -> ImageLibrary* = 0;
  virtual auto persistence() -> Persistence* = 0;
  virtual auto teamColors() -> TeamColors* = 0;
  virtual auto teamConfig() -> TeamConfig* = 0;
  virtual auto timerManager() -> TimerManager* = 0;

  virtual void generateCommandArgs(const wxCmdLineParser& parser, int argc,
                                   const wxCmdLineArgsArray& argv) = 0;
};

class SingletonImpl : public Singleton {
 public:
  ~SingletonImpl() override;
  auto autoUpdate() -> AutoUpdate* override;
  auto commandArgs() -> CommandArgs* override;
  auto displayConfig() -> DisplayConfig* override;
  auto frameManager() -> FrameManager* override;
  auto generalConfig() -> GeneralConfig* override;
  auto hotkeyTable() -> HotkeyTable* override;
  auto imageLibrary() -> ImageLibrary* override;
  auto persistence() -> Persistence* override;
  auto teamColors() -> TeamColors* override;
  auto teamConfig() -> TeamConfig* override;
  auto timerManager() -> TimerManager* override;

  void generateCommandArgs(const wxCmdLineParser& parser, int argc,
                           const wxCmdLineArgsArray& argv) override;

 protected:
  // This class uses raw pointers to avoid over-circular reliance on the headers
  // that are Singletons themselves.  This means they must be deleted in the
  // destructor.
  AutoUpdate* inst_auto_update = nullptr;
  CommandArgs* inst_command_args = nullptr;
  DisplayConfig* inst_display_config = nullptr;
  FrameManager* inst_frame_manager = nullptr;
  GeneralConfig* inst_general_config = nullptr;
  HotkeyTable* inst_hotkey_table = nullptr;
  ImageLibrary* inst_image_library = nullptr;
  Persistence* inst_persistence = nullptr;
  TeamColors* inst_team_colors = nullptr;
  TeamConfig* inst_team_config = nullptr;
  TimerManager* inst_timer_manager = nullptr;
};

}  // namespace cszb_scoreboard
