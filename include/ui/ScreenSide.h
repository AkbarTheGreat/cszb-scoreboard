/* 
ui/ScreenSide.h: This enum represents the position of a monitor in a
multi-monitor scoreboard setup, as well as an error state (NONE).

Copyright 2019 Tracy Beck

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

enum ScreenSide {
  SIDE_NONE,    // Represents a case where no external monitors found
  SIDE_SINGLE,  // A single monitor
  SIDE_LEFT,    // Left monitor
  SIDE_RIGHT    // Right monitor
};