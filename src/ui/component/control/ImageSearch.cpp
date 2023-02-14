/*
ui/component/control/ImageSearch.h: Handles searching for images from a webview
for quick use as a bit.

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

#include "ui/component/control/ImageSearch.h"

#include <filesystem>  // for path
#include <optional>    // for optional
#include <string>      // for string

#include "ScoreboardCommon.h"                   // for DEFAULT_BORDER_SIZE
#include "config/swx/event.h"                   // for wxEVT_BUTTON
#include "config/swx/image.h"                   // for Image
#include "ui/component/control/TeamSelector.h"  // for TeamSelector
#include "ui/widget/FilePicker.h"               // for FilePicker
#include "ui/widget/Label.h"                    // for Label
#include "ui/widget/PopUp.h"                    // for PopUp
#include "ui/widget/Widget.h"                   // for NO_BORDER
#include "util/Clipboard.h"                     // for Clipboard
#include "util/FilesystemPath.h"                // for FilesystemPath
#include "util/HttpReader.h"
#include "util/Log.h"

namespace cszb_scoreboard {

const int BORDER_SIZE = DEFAULT_BORDER_SIZE;

const std::string DROP_MESSAGE = "<Drop Image Here To Load>";
const std::string URL = "https://images.google.com";
constexpr Size DROP_TARGET_SIZE{.width = 120, .height = 360};
constexpr Size BROWSER_SIZE{.width = 960, .height = 360};
constexpr int DROP_TARGET_BORDER = 50;

auto ImageSearch::Create(swx::Panel *wx) -> std::unique_ptr<ImageSearch> {
  auto local_image = std::make_unique<ImageSearch>(wx);
  local_image->initializeWidgets();
  local_image->updatePreview();
  return local_image;
}

void ImageSearch::createControls(Panel *control_panel) {
  ScreenImageController::createControls(control_panel);
  inner_panel = control_panel->panel();
  screen_selection->setParent(inner_panel.get());
  current_image_label->setParent(inner_panel.get());

  drop_target = inner_panel->panel();
  drop_target->setBorder();
  drop_text = drop_target->label(DROP_MESSAGE);
  browser = inner_panel->browser(URL);
  reset_button_panel = inner_panel->panel();
  reset_button = reset_button_panel->button("Reset\nBrowser", true);

  all_screen_image_name = "";
  away_screen_image_name = "";
  home_screen_image_name = "";
  current_image_label->set("");

  positionWidgets(control_panel);
  bindEvents();
}

void ImageSearch::positionWidgets(Panel *control_panel) {
  control_panel->addWidget(*inner_panel, 0, 0);
  drop_target->addWidget(*drop_text, 0, 0, DROP_TARGET_SIZE.height / 2, wxTOP);
  reset_button_panel->addWidget(*reset_button, 0, 0, NO_BORDER);

  inner_panel->addWidgetWithSpan(*drop_target, 0, 0, 2, 1, DROP_TARGET_SIZE,
                                 NO_BORDER);

  inner_panel->addWidget(*screen_selection, 0, 1, NO_BORDER);
  inner_panel->addWidget(*reset_button_panel, 1, 1, NO_BORDER);

  inner_panel->addWidgetWithSpan(*browser, 0, 2, 2, 1, BROWSER_SIZE, NO_BORDER);

  // I have to put this empty label somewhere to be compliant, so I just shove
  // it at the bottom.
  inner_panel->addWidget(*current_image_label, 2, 0);

  drop_target->runSizer();
  inner_panel->runSizer();
}

void ImageSearch::bindEvents() {
  drag_handler = std::make_unique<DragAndDropHandler>(
      drop_target.get(),
      [this](int32_t x, int32_t y, const std::string &url) -> void {
        this->onURLDrop(url);
      });
  browser->bind(wxEVT_WEBVIEW_LOADED, [this](wxWebViewEvent &e) -> void {
    this->tweakGoogleImages();
  });
  reset_button->bind(wxEVT_COMMAND_BUTTON_CLICKED,
                     [this](wxCommandEvent &e) -> void { this->resetURL(); });
}

void ImageSearch::onURLDrop(const std::string &url) {
  HttpReader reader;
  LogDebug("URL %s", url);
  std::vector<char> image_data;
  reader.readBinary(url.c_str(), &image_data);
  Image urlImage = Image::FromData(image_data);
  if (screen_selection->allSelected()) {
    all_screen_image = urlImage;
  } else if (screen_selection->awaySelected()) {
    away_screen_image = urlImage;
  } else {
    home_screen_image = urlImage;
  }

  control_panel->update();
  updatePreview();
}

void ImageSearch::tweakGoogleImages() {
  browser->runJavascript(
/*
      "document.onclick = function() {"
      // This chunk of code removes anchors on the larger boxes when you click
      // in from the selection, so that drag & drop drops the image instead of
      // the link.
      "  var anchors = "
      "  document.querySelectorAll('[role=\"link\"]').forEach(function(anchor) "
      "  {"
      "    var anchorParent = anchor.parentNode;"
      "    while (anchor.firstChild) {"
      "      anchorParent.insertBefore(anchor.firstChild, anchor);"
      "    }"
      "    anchorParent.removeChild(anchor);"
      "  });"
*/
      // TODO: decode data:image URLS like data:image/jpeg;base64,/9j/4AAQSkZJRgABAQAAAQABAAD/2wCEAAkGBw8PDw8PDw8PDw8PFQ8PDw8PDxUPDw8PFRUWFhUVFRUYHSggGBolHRUVITEhJSkrLi4uFx81ODMtNygtLisBCgoKBQUFDgUFDisZExkrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrKysrK//AABEIAKIBNwMBIgACEQEDEQH/xAAcAAEBAAIDAQEAAAAAAAAAAAAAAQIHBAUGAwj/xABCEAACAQMCAwYDBAYGCwAAAAAAAQIDBBESIQUxUQYHE0FhkSJxgTJCobEjYnKCksEUM1JjovAIJDVDU2Rzo8PR8f/EABQBAQAAAAAAAAAAAAAAAAAAAAD/xAAUEQEAAAAAAAAAAAAAAAAAAAAA/9oADAMBAAIRAxEAPwDaIAAAACkAAAAAAAABQBkRGSAIpCgAWMcmse2XerTpeNbcOiqlVYgrxtSoxlnEnTjv4jW6Te2d91zDZstk5NpJbtt4SXqzq6/aPh9OShO+s4ye6i7mmnj+I/P15fXVy8XFxWr/AH5KrVlUg59VFvC57JLB9KFmo/PSpzeN9+UUB+g7LjNpXemhdW1aX9mlXhOXsnk5x+cZWUZZSy9PnFpKE15apNLOem/tt3/Z3t7fcPkqdebvbZbOnVnmvTXWFV7v5SyvVcwN3g63s92gtOI0/Etaqm448SlL4a1LPlOD3Xz3T8mzsmgITBkQDEhk0RgQAAAAAAAAAACFIAAAAAAAAAAAAAACgACgoAoKAAOi7c8VlZ8Nu7im1GrGGilJ/dqTahF/NZyvVIDXne129k3U4baVMU0nC8qw5zl50oyXKK5S6t45J51nZRUprG/J+x87ePjVlFb5xzf8zZfAuz9Gnp+BNvGZNb58wPM2PCKtT7EJPO2cYXPr9F7HbW/ZK/qSz8CWYvDlnLibEsLCMVskjuLa2S8gNN3PZTiFu1pUaiSUcKW+ElleT35vHPBwLmjWgmq1tUi1neCUYrz2UV/n1N51rVZ2RwbvhMWsteoGi+GcSqUa0a1rVlSrU86Wmo1EvNYe0k/OL59De3YTtauJ0Ja4qnd0NKr01tGSf2asFzUXh7eTTW+zeve3HY2n4crqgnCpTxKSSSU15v5o6fsrxiVldW93qxBNU7lPCbt5NKbx0W0v3UBv8FZABizIjAxIVkAAAAAAAAAgAAAAAAABSAAAABQUCFBQAAApSFAGvu/G6UOFwhl6qlejjfCelSl9eWfobBNZd/aj/RLHP/Hln9nw5f8Apgaz7IcNbnGo8vdG2rKisRxjKPEdl6ajThJ4TaUm3ySaPa8Kv7dJydTlu20/y6Aegs03heR2qWEdFwvjlpUliNVZzjEk45f1PSRUZRTTTzywB8IPPkWrHJ87i/o0VmpLQuri2vwPnS4rb1f6uvTn10vcDreO2jlQqLGcxexqDiVnptpprGG3Br1W6+a/FP5G8qk1LbZ5/FGrO3tioOqofDGSjNdFLzX4sDafZm8dxYWVdrDq0KE5LpJwWr8cnZHnu7q48Xg9g9LjopeA01zdGUqWpej0Z+p6EAAAMWRmTMWBAAAAAAAACAAAAAAAAAACgACgoAAAAABUUiKBTSff/wAR1V7W1xJeFTdbntN1G45x6KD/AIjdhpPv64LP+kW97/u6sY2z89M465L5ZTfs/qHAsoRjSoue60QePJvSjteG8euKVJVaUaLjr0OEpNOK0t/dafplvn0OXZWcakIwaXwpJfJLH5HdcP4bGKbSS8vsp/mgPlx26k4U5x0SdRZxDMk3vjOpaoZ5rPXfB3vd3eyrQkp5zBtLPQ6e/t4JapZfXL5o5nYGtqlWccLL2+S8gO37TcS8JaYQUpT2Tkk4x8vPmzz3D+LKVxK2naXE6kXFSlDRiOrZbOa2zhbdV5Hr76wjcQ+JLKed1tk66w4LRp1HUdOPiYxr5TSxjGemG0Bnb1IVYvD5PZyWiafSUeaaNe959Tw6c295TcYQXzWZP2izZDtoQfwxUVnOEtjWPe5HNW3S+9GtJftQcX+TYHsO5/iHjcMdPDzbVZwy/OM/0i9tTX0Pamt+5KM/Cvp86UpUIqX97GMtS9pQNkACFAEIykAhCkAAhQAAAAgAAAAAAKAABQUCFAAAAAAUCopEUAec7xeHRueFXsJJtwpyr08c1VpfHFr2a+TZ6M+V5QVWlVpN4VWE6bfRSi45/EDUXAr1Smmnt+aZ7K3qp7LmeEXDqnD7h21fRKcVB6qbbg4y+y1lJ8vxyejs62GnnOGsf5+oGfaLWqajHZ1JRpqXTU8HM7BX9Gn41BxanRqSpVE4tTjKKzlprLWFlPkzG+fiR0tJxfXzOd2dtqdCm1Fp1HPMt1qjF828+QHe8L4jRrwdWhUVSk3KGUmnGcXhpp7rDOXUjHGX7nylTSTaSxLdteb6khN4wBhcpYPF8e4RG7vbTXJeHBVsx+9LUo5+mF+KPaVt8nEsLNSnqxvlJy6RTzj6gcjs7wWlYW0LWimoxcpyb3lOpN6pSb+uPkkdiVkAhCkAEKQCApAIAAAAAgAAAFAgBQBSFAoAAAAAAABQUCgAACkA1p3qW2i6tbjyqU5Um/LVTk5L64qf4Tg8FrKeF0a9j2feRZwq8LuZSXxW6VxTkucZwf5NOSfzNS9n+J4kt/RoD3PaBXOqELd01FR3lLU5Z6YW31yY8L4NdOKcq+Hu09MVs92nsZ2lxndvUuayei4dVlLDwopZxtlv8QM7a2vaSThOnUT+3Cb0xl6ppPD+hzbS4ck1OGiae8c6l9H5o5Gp4RxqkmvmB9qk9jmW0cQivTP1e/8AM6u3aqTUPnKX7K/+ncMCAACEKAIQpAAAAxBSAAAAIUgAAACkKBSkRQAAAAFAAuAAKCgAAAIU8d2t7x+H8PUoKaublZSoUZJ4l/eT5Q/F+gHM7xbyjS4XeKrUjB1qVShRTfxVK84tQjFc28+yy+SNEO3nHFSGVjZi/wC0tfiXEIXF01iLxClHKp0YZ5RT/Fvds9nZcMWmdP4XKD0tp5UlhOL+qaf1A4XZ/i0tUIVNk+UnyybK4Pe00kspLGd2a2t+HrxFDC54xyNjcI4BSjTilqb5tt5+mAO4lf0/KSb9N2fCSnUe3wx6+eDm0eHwgsRil1wse5akUlt5AeR7WdpZcIhTuIUlVXiRpVKbelunJNtp+UvhW56vs9xy34hbwubaWqnPZp7TpzXOE15NGqu9KuqtKcM/BT1yT61VKMcf42v3ZdDwvYrthc8KrOpRxUpTx41vJ4hUS5NP7sl5P8wP1CDzHZLt1Y8TSVKp4dfztqzUav7vlNeq+uD04EAAEBSAQFIBCFZAAAAEKAIAUAUhQBQAAKABQUACnW9ouNUbC1q3Vdvw6S+yvtTm9owj6t7AdkDRku+m/wD0jVraJSz4SfiN01+s9Xx/4TzHF+8Li1zlTvalOL+5b4t4pdMw+Jr5tgfori3HLOzWbq5o0M7pVKijKXyjzf0Rr7tH3y2tL4LClK6l51amqjRXyTWqXsvmaPq1JSk5SblKW8pSblJvq292YAes7Q94vFL6LhUr+DSltKlbR8KDXrLLk16N4PJ5BAPtbT0v57fzNpdjeIwrSt25/pHFUqsX543g+n9pfWJqmUdljmem7GX2ifPTut/JPbHz3wBsfi1lKFw01jUtUWvNHt+y1VyopTXxRWHnz9Tobm6p1qFKvLG3wz6xfn/n1OdQ4xSpU1JTSSTy3yA9TVmlu3/I6C84g685ULdt6dq9aL+GiuemL86jXl5J5fkn42+7VVb6p4NvKpClH+urwjrcI/qxysv6+/I7uF5RtaEacJJ2unVKSllpOWM6nu9c3ped4tvPUDy3eZKEbRxilFR0fDy+COY0l8/jnL941Aez7x+LOrVVHOd/EqY5Z3UUlnkk3t8jxgFUmmmm01hprZp9Ue27Nd6PErPEak1eUVheHcNuaX6tVfF/FqW3I8OUD9FdnO9Phl41Cc5WdV7aLjCpt/q1V8Pvg9vCSaTTTT3TTymvRn49O44D2ov7B/6pc1KUebp510X86csx+uMgfqsGoOzHfNmShxKjGEXyuLaMmo/t0228esX9DY/A+1FhfynC0uadacFqlBaoyUeWcSSbW63XUDtwUjAxIZGIAAACAACkAFKRFAoAApUQyQApCgDTv+kBxN5srNP4cVLmourzop/+T3NxH5577bpVOMTgnnwaVvSfo2nUx/3APCMwZlNmAAFIAAAH2axsczg9dU5VZbZVOcoKXKVSPJevm/ocSZ823zXPdfNPZgbO7D3852dynUlWxSlXlrTxGUcOUVnm8Z+qRwOF8NuOIqtOMmqFFNNanhzabWUt8bHnuzvGKlvbXKg9OcUspbqNRT1Y9WotZ8lJ/MvA+P3drOf9DrTouazNQjGSljllST6get7K3UYW9ZVF4UrduVbK06oJP4kn5NJ/VPc6fivaCpbzlDd+KvFuKTe0Kko/o4rnvCDSa5S1Sz6cCr2krTuqde8k60UtFSEYQpeJFfFFT0xWpa1Fv0T6nQXlzKrOVSbcpzblJ9W+YHzq1HOTlJtuTy223+Z82ZJGMgIUgAoRCoCnoOwHFZWnFLOquTqRo1FnCdOq9Es9catXzijzxYzcWpReJRakn0a3TA/YRD52tZVKdOouVSMJr1Ukn/M+gGLIZMxYEAAAgAAAAUpCgUpCgUpEUCgACn5X7b3zuOJ31V/er1Yx/Yg9Efwij9UI/IfEauuvWn/aqVZe82wOPIxMp8zEAAUCAAD7y5mGDOfMxAsajSaTaUsZSezxyyiQqOLynh9TEAZVKkpPMpOT6t5ZikCgRGLM0YMAQAAUgApGUgH6t7Hyzw3hzznNraPL5/1MDtjoO72rr4Tw5/8AL0o/wrT/ACPQAYsxZkyMDEAACFIAAKBQUACgAUyQAAFABH5KuILXPZfal5erAAwcF0XsY+HHovYAB4cei9i6I9F7AANEei9iaI9F7AAfSUF0XsTQui9gAJoXRew0LovYABoXRew0LovYoAKC6L2MXCPRewAE8OPRexfDj0XsABPDj0XsNEei9gAHhx6L2Hhx6L2AA/Sndv8A7H4f/wBGP5s9IABGYsADFgAAQoAIAAf/2Q==
      "document.ondragstart = function(event) {"
      "  event.dataTransfer.setData( 'text', "
      "  event.srcElement.getAttribute('src'));"
      // This next bit attempts to remove the "Search any image with Google
      // Lens" popup on images.google.com.
      "  var popup = document.getElementsByClassName('ea0Lbe')[0];"
      "  if (popup) {"
      "    popup.parentElement.removeChild(popup);"
      "  }"
      "};");
}

void ImageSearch::resetURL() { browser->setURL(URL); }

}  // namespace cszb_scoreboard
