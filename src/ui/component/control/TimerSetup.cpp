/*
ui/component/control/TimerSetup.cpp: This class is responsible for managing
timers which may be displayed at the bottom of every other view.

Copyright 2021-2024 Tracy Beck

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

#include "ui/component/control/TimerSetup.h"

#include <string>  // for string

#include "config/swx/event.h"       // for wxEVT_KEY_UP, wxEVT_TOGGLEBUTTON
#include "ui/frame/FrameManager.h"  // for FrameManager
#include "ui/widget/Panel.h"        // for Panel
#include "util/Singleton.h"         // for Singleton
#include "util/StringUtil.h"        // for StringUtil
#include "util/TimerManager.h"      // for TimerManager

namespace cszb_scoreboard {
class ScreenText;
namespace swx {
class Panel;
}  // namespace swx

const std::string START_AND_SHOW = "Start and Show";
const std::string START = "Start";
const std::string PAUSE = "Pause";
const std::string SHOW = "Show";
const std::string HIDE = "Hide";

auto TimerSetup ::Create(swx::Panel *wx) -> std::unique_ptr<TimerSetup> {
  auto control = std::make_unique<TimerSetup>(wx);
  control->initializeWidgets();
  return control;
}

void TimerSetup::createControls(Panel *control_panel) {
  minutes_label = control_panel->label("Minutes");
  minutes_text = control_panel->text("5");
  seconds_label = control_panel->label("Seconds");
  seconds_text = control_panel->text("0");

  start_stop_button = control_panel->toggle(START_AND_SHOW);
  show_hide_button = control_panel->toggle(SHOW);
  reset_button = control_panel->button("Reset");

  positionWidgets(control_panel);
  bindEvents();
  timeUpdated();
}

void TimerSetup::positionWidgets(Panel *control_panel) {
  control_panel->addWidget(*minutes_label, 0, 0);
  control_panel->addWidget(*minutes_text, 0, 1);
  control_panel->addWidget(*seconds_label, 1, 0);
  control_panel->addWidget(*seconds_text, 1, 1);
  control_panel->addWidget(*start_stop_button, 0, 2);
  control_panel->addWidget(*show_hide_button, 1, 2);
  control_panel->addWidget(*reset_button, 2, 2);
  control_panel->runSizer();
}

void TimerSetup::bindEvents() {
  start_stop_button->bind(
      wxEVT_TOGGLEBUTTON,
      [this](wxCommandEvent &event) -> void { this->startOrStopTimer(); });
  show_hide_button->bind(
      wxEVT_TOGGLEBUTTON,
      [this](wxCommandEvent &event) -> void { this->showOrHideTimer(); });
  reset_button->bind(
      wxEVT_COMMAND_BUTTON_CLICKED,
      [this](wxCommandEvent &event) -> void { this->resetTimer(); });
  minutes_text->bind(
      wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void { this->timeUpdated(); });
  seconds_text->bind(
      wxEVT_KEY_UP, [this](wxKeyEvent &event) -> void { this->timeUpdated(); });
}

void TimerSetup::startOrStopTimer() {
  if (singleton->timerManager()->timerRunning()) {
    singleton->timerManager()->pauseTimer();
  } else {
    singleton->timerManager()->startTimer();
    singleton->timerManager()->showTimer();
  }
  fixButtons();
}

void TimerSetup::showOrHideTimer() {
  if (singleton->timerManager()->timerOn()) {
    singleton->timerManager()->hideTimer();
    singleton->frameManager()->refreshFrames();
  } else {
    singleton->timerManager()->showTimer();
  }
  fixButtons();
}

void TimerSetup::resetTimer() {
  int64_t timer = secondsFromText();
  singleton->timerManager()->setTime(timer);
}

auto TimerSetup::secondsFromText() -> int64_t {
  // NOLINTNEXTLINE (readability-magic-numbers)
  return StringUtil::stringToInt(minutes_text->value()) * 60 +
         StringUtil::stringToInt(seconds_text->value());
}

void TimerSetup::timeUpdated() {
  if (secondsFromText() != last_set_timer_seconds) {
    last_set_timer_seconds = secondsFromText();
    resetTimer();
  }
}

void TimerSetup::fixButtons() {
  if (singleton->timerManager()->timerOn() &&
      singleton->timerManager()->timerRunning()) {
    // Both Pressed.
    start_stop_button->setLabel(PAUSE);
    start_stop_button->press();
    show_hide_button->setLabel(HIDE);
    show_hide_button->press();
  } else if (singleton->timerManager()->timerOn()) {
    // Timer shown, but paused.
    start_stop_button->setLabel(START);
    start_stop_button->unpress();
    show_hide_button->setLabel(HIDE);
    show_hide_button->press();
  } else if (singleton->timerManager()->timerRunning()) {
    // Timer running, but hidden.
    start_stop_button->setLabel(PAUSE);
    start_stop_button->press();
    show_hide_button->setLabel(SHOW);
    show_hide_button->unpress();
  } else {
    // Paused and hidden.
    start_stop_button->setLabel(START_AND_SHOW);
    start_stop_button->unpress();
    show_hide_button->setLabel(SHOW);
    show_hide_button->unpress();
  }
}

void TimerSetup::updateScreenText(ScreenText *screen_text) {}

}  // namespace cszb_scoreboard
