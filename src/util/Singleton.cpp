
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

#include "util/Singleton.h"

#include <stdexcept>  // for runtime_error

#include "config/CommandArgs.h"      // for CommandArgs
#include "config/DisplayConfig.h"    // for DisplayConfig
#include "config/GeneralConfig.h"    // for GeneralConfig
#include "config/ImageLibrary.h"     // for ImageLibrary
#include "config/Persistence.h"      // for Persistence
#include "config/TeamConfig.h"       // for TeamConfig
#include "ui/frame/FrameManager.h"   // for FrameManager
#include "ui/frame/HotkeyTable.h"    // for HotkeyTable
#include "ui/graphics/TeamColors.h"  // for TeamColors
#include "util/AutoUpdate.h"         // for AutoUpdate
#include "util/TimerManager.h"       // for TimerManager

namespace cszb_scoreboard {

SingletonImpl::~SingletonImpl() {
  delete inst_auto_update;
  delete inst_command_args;
  delete inst_display_config;
  delete inst_frame_manager;
  delete inst_hotkey_table;
  delete inst_image_library;
  delete inst_persistence;
  delete inst_team_colors;
  delete inst_team_config;
  delete inst_timer_manager;
}

auto Singleton::getInstance() -> Singleton* {
  static SingletonImpl instance;
  return &instance;
}

auto SingletonImpl::autoUpdate() -> AutoUpdate* {
  if (inst_auto_update == nullptr) {
    inst_auto_update = new AutoUpdate(SingletonClass{});
  }
  return inst_auto_update;
}

auto SingletonImpl::commandArgs() -> CommandArgs* {
  if (inst_command_args == nullptr) {
    throw new std::runtime_error(
        "The generator generate_command_args() must be called before "
        "command_args() may be used.");
  }
  return inst_command_args;
}

void SingletonImpl::generateCommandArgs(const wxCmdLineParser& parser, int argc,
                                        const wxCmdLineArgsArray& argv) {
  if (inst_command_args != nullptr) {
    throw new std::runtime_error("Cannot generate_command_args twice.");
  }
  inst_command_args = new CommandArgs(SingletonClass{});
  inst_command_args->process_args(parser, argc, argv);
}

auto SingletonImpl::displayConfig() -> DisplayConfig* {
  if (inst_display_config == nullptr) {
    inst_display_config = new DisplayConfig(SingletonClass{});
  }
  return inst_display_config;
}

auto SingletonImpl::frameManager() -> FrameManager* {
  if (inst_frame_manager == nullptr) {
    inst_frame_manager = new FrameManager(SingletonClass{});
  }
  return inst_frame_manager;
}

auto SingletonImpl::generalConfig() -> GeneralConfig* {
  if (inst_general_config == nullptr) {
    inst_general_config = new GeneralConfig(SingletonClass{});
  }
  return inst_general_config;
}

auto SingletonImpl::hotkeyTable() -> HotkeyTable* {
  if (inst_hotkey_table == nullptr) {
    inst_hotkey_table = new HotkeyTable(SingletonClass{});
  }
  return inst_hotkey_table;
}

auto SingletonImpl::imageLibrary() -> ImageLibrary* {
  if (inst_image_library == nullptr) {
    inst_image_library = new ImageLibrary(SingletonClass{});
  }
  return inst_image_library;
}

auto SingletonImpl::persistence() -> Persistence* {
  if (inst_persistence == nullptr) {
    inst_persistence = new Persistence(SingletonClass{});
  }
  return inst_persistence;
}

auto SingletonImpl::teamColors() -> TeamColors* {
  if (inst_team_colors == nullptr) {
    inst_team_colors = new TeamColors(SingletonClass{});
  }
  return inst_team_colors;
}

auto SingletonImpl::teamConfig() -> TeamConfig* {
  if (inst_team_config == nullptr) {
    inst_team_config = new TeamConfig(SingletonClass{});
  }
  return inst_team_config;
}

auto SingletonImpl::timerManager() -> TimerManager* {
  if (inst_timer_manager == nullptr) {
    inst_timer_manager = new TimerManager(SingletonClass{});
  }
  return inst_timer_manager;
}

}  // namespace cszb_scoreboard
