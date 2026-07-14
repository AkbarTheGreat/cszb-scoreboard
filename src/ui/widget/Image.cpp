/*
ui/widget/Image.cpp: A widget wrapper around swx::Image with custom animation
and layout helpers.  Much of this was written by Antigravity, with a manual
cleanup pass by a human.

Copyright 2026 Tracy Beck

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

#include "ui/widget/Image.h"

#include <wx/gdicmn.h>    // for wxSize, wxPoint, wxBitmapType, wxRect
#include <wx/gifdecod.h>  // for wxGIFDecoder, wxGIFErrorCode
#include <wx/mstream.h>   // for wxMemoryInputStream
#include <wx/wfstream.h>  // for wxFileInputStream

#include <cstring>  // for size_t, memset

#include "ui/graphics/Color.h"    // for Color
#include "util/FilesystemPath.h"  // for FilesystemPath
#include "wx/animdecod.h"         // for wxAnimationDisposal
#include "wx/string.h"            // for wxString

// IWYU pragma: no_include <bits/chrono.h>

namespace cszb_scoreboard {

/**
 * Converts the image's transparency mask into an alpha channel.
 *
 * If the image has a mask color defined (e.g., a specific key color meant to be
 * transparent), this function initializes the image's alpha channel, iterates
 * through each pixel, and sets the alpha to fully transparent (0) for pixels
 * matching the mask color, and fully opaque (255) for all other pixels.
 * Finally, it disables the legacy mask mechanism.
 *
 * @param image The wxImage to process.
 */
static void convertMaskToAlpha(wxImage& image) {
  if (image.IsOk() && image.HasMask()) {
    unsigned char red = image.GetMaskRed();
    unsigned char green = image.GetMaskGreen();
    unsigned char blue = image.GetMaskBlue();
    image.InitAlpha();
    int width = image.GetWidth();
    int height = image.GetHeight();
    unsigned char* alpha = image.GetAlpha();
    unsigned char* data = image.GetData();
    if (alpha != nullptr && data != nullptr) {
      constexpr int NUM_CHANNELS = 3;
      constexpr int GREEN_OFFSET = 1;
      constexpr int BLUE_OFFSET = 2;
      constexpr unsigned char OPAQUE_ALPHA = 255;
      constexpr unsigned char TRANSPARENT_ALPHA = 0;

      for (int i = 0; i < width * height; ++i) {
        if (data[i * NUM_CHANNELS] == red &&
            data[i * NUM_CHANNELS + GREEN_OFFSET] == green &&
            data[i * NUM_CHANNELS + BLUE_OFFSET] == blue) {
          alpha[i] = TRANSPARENT_ALPHA;
        } else {
          alpha[i] = OPAQUE_ALPHA;
        }
      }
    }
    image.SetMask(false);
  }
}

/**
 * Blends a foreground image onto a background image at a given offset.
 *
 * This function performs alpha blending of the foreground image (`fg`) onto
 * the background image (`bg`) at the specified coordinate offset (`x_offset`,
 * `y_offset`). If the background image does not have an alpha channel, one is
 * initialized. The function iterates through each pixel of the foreground image
 * and calculates the composite color using standard alpha blending formulas:
 *   out_alpha = fg_alpha + bg_alpha * (1 - fg_alpha)
 *   out_color = (fg_color * fg_alpha + bg_color * bg_alpha * (1 - fg_alpha)) /
 * out_alpha
 *
 * @param bg_image The destination background image.
 * @param fg_image The source foreground image.
 * @param x_offset The horizontal offset where the foreground image should be
 * drawn.
 * @param y_offset The vertical offset where the foreground image should be
 * drawn.
 */
static void blendImage(wxImage& bg_image, const wxImage& fg_image, int x_offset,
                       int y_offset) {
  int bg_width = bg_image.GetWidth();
  int bg_height = bg_image.GetHeight();
  int fg_width = fg_image.GetWidth();
  int fg_height = fg_image.GetHeight();

  unsigned char* bg_data = bg_image.GetData();
  unsigned char* bg_alpha = bg_image.GetAlpha();
  const unsigned char* fg_data = fg_image.GetData();
  const unsigned char* fg_alpha = fg_image.GetAlpha();

  if (bg_alpha == nullptr) {
    bg_image.InitAlpha();
    bg_alpha = bg_image.GetAlpha();
  }

  constexpr int NUM_CHANNELS = 3;
  constexpr int GREEN_OFFSET = 1;
  constexpr int BLUE_OFFSET = 2;
  constexpr unsigned char OPAQUE_VAL = 255;
  constexpr float OPAQUE_FLOAT = 255.0F;

  for (int y = 0; y < fg_height; ++y) {
    int bg_y = y + y_offset;
    if (bg_y < 0 || bg_y >= bg_height) {
      continue;
    }
    for (int x = 0; x < fg_width; ++x) {
      int bg_x = x + x_offset;
      if (bg_x < 0 || bg_x >= bg_width) {
        continue;
      }

      int fg_idx = y * fg_width + x;
      unsigned char alpha =
          (fg_alpha != nullptr) ? fg_alpha[fg_idx] : OPAQUE_VAL;
      if (alpha == 0) {
        continue;
      }

      int bg_idx = bg_y * bg_width + bg_x;
      int fg_rgb_idx = fg_idx * NUM_CHANNELS;
      int bg_rgb_idx = bg_idx * NUM_CHANNELS;

      if (alpha == OPAQUE_VAL) {
        bg_data[bg_rgb_idx] = fg_data[fg_rgb_idx];
        bg_data[bg_rgb_idx + GREEN_OFFSET] = fg_data[fg_rgb_idx + GREEN_OFFSET];
        bg_data[bg_rgb_idx + BLUE_OFFSET] = fg_data[fg_rgb_idx + BLUE_OFFSET];
        bg_alpha[bg_idx] = OPAQUE_VAL;
      } else {
        float a = static_cast<float>(alpha) / OPAQUE_FLOAT;
        float bg_a = static_cast<float>(bg_alpha[bg_idx]) / OPAQUE_FLOAT;
        float out_a = a + bg_a * (1.0F - a);

        bg_data[bg_rgb_idx] = static_cast<unsigned char>(
            (fg_data[fg_rgb_idx] * a +
             bg_data[bg_rgb_idx] * bg_a * (1.0F - a)) /
            out_a);
        bg_data[bg_rgb_idx + GREEN_OFFSET] = static_cast<unsigned char>(
            (fg_data[fg_rgb_idx + GREEN_OFFSET] * a +
             bg_data[bg_rgb_idx + GREEN_OFFSET] * bg_a * (1.0F - a)) /
            out_a);
        bg_data[bg_rgb_idx + BLUE_OFFSET] = static_cast<unsigned char>(
            (fg_data[fg_rgb_idx + BLUE_OFFSET] * a +
             bg_data[bg_rgb_idx + BLUE_OFFSET] * bg_a * (1.0F - a)) /
            out_a);
        bg_alpha[bg_idx] = static_cast<unsigned char>(out_a * OPAQUE_FLOAT);
      }
    }
  }
}

/**
 * Clears a specific rectangular region of the image to transparent black.
 *
 * This is used during GIF playback when a frame's disposal method is "Restore
 * to Background". It sets the alpha channel and the RGB data of the specified
 * rectangle (defined by `pos` and `size`) to 0 (fully transparent black).
 *
 * @param canvas The destination image canvas to modify.
 * @param pos The top-left corner of the region to clear.
 * @param size The width and height of the region to clear.
 */
static void clearFrameToBackground(wxImage& canvas, const wxPoint& pos,
                                   const wxSize& size) {
  constexpr int NUM_CHANNELS = 3;
  constexpr int GREEN_OFFSET = 1;
  constexpr int BLUE_OFFSET = 2;

  int width = canvas.GetWidth();
  int height = canvas.GetHeight();
  unsigned char* alpha = canvas.GetAlpha();
  unsigned char* data = canvas.GetData();
  for (int y = 0; y < size.GetHeight(); ++y) {
    int bg_y = y + pos.y;
    if (bg_y < 0 || bg_y >= height) {
      continue;
    }
    for (int x = 0; x < size.GetWidth(); ++x) {
      int bg_x = x + pos.x;
      if (bg_x < 0 || bg_x >= width) {
        continue;
      }
      int idx = bg_y * width + bg_x;
      alpha[idx] = 0;
      data[idx * NUM_CHANNELS] = 0;
      data[idx * NUM_CHANNELS + GREEN_OFFSET] = 0;
      data[idx * NUM_CHANNELS + BLUE_OFFSET] = 0;
    }
  }
}

/**
 * Decodes all frames of a GIF animation and composites them based on disposal
 * methods.
 *
 * Since GIF frames can be stored as smaller, partial images overlaid on a
 * larger canvas, and since each frame specifies how the previous frame should
 * be disposed of, this function handles the full decoding and rendering of each
 * frame onto a cumulative canvas. It processes:
 * 1. Frame conversion to image and mask-to-alpha conversion.
 * 2. Pre-frame disposal methods from the prior frame (such as restoring to the
 * previous state, clearing to background, or keeping the current frame).
 * 3. Blending the current frame onto the correct canvas state.
 * 4. Storing the final composited frames and their corresponding frame delays.
 *
 * @param decoder The active wxGIFDecoder containing the loaded GIF.
 * @param delays Output vector to be populated with the duration (in
 * milliseconds) of each frame.
 * @return A vector of fully-composited wxImage frames ready for playback.
 */
static auto decodeAndCompositeGIF(wxGIFDecoder& decoder,
                                  std::vector<int>& delays)
    -> std::vector<wxImage> {
  size_t frame_count = decoder.GetFrameCount();
  std::vector<wxImage> frames;
  if (frame_count <= 1) {
    return frames;
  }

  std::vector<wxImage> canvas_history;
  wxSize animation_size = decoder.GetAnimationSize();
  wxImage current_canvas(animation_size.GetWidth(), animation_size.GetHeight(),
                         true);
  current_canvas.InitAlpha();
  memset(current_canvas.GetAlpha(), 0,
         animation_size.GetWidth() * animation_size.GetHeight());

  frames.reserve(frame_count);
  delays.reserve(frame_count);
  canvas_history.reserve(frame_count);

  constexpr size_t MIN_HISTORY_FOR_PREVIOUS = 2;
  constexpr int DEFAULT_DELAY_MS = 100;

  for (size_t i = 0; i < frame_count; ++i) {
    wxImage frame;
    if (decoder.ConvertToImage(i, &frame)) {
      convertMaskToAlpha(frame);

      wxImage canvas_to_draw;
      if (i == 0) {
        canvas_to_draw = current_canvas.Copy();
      } else {
        wxAnimationDisposal prev_disposal = decoder.GetDisposalMethod(i - 1);
        if (prev_disposal == wxANIM_TOPREVIOUS &&
            canvas_history.size() >= MIN_HISTORY_FOR_PREVIOUS) {
          canvas_to_draw = canvas_history[i - MIN_HISTORY_FOR_PREVIOUS].Copy();
        } else if (prev_disposal == wxANIM_TOBACKGROUND) {
          canvas_to_draw = canvas_history[i - 1].Copy();
          clearFrameToBackground(canvas_to_draw,
                                 decoder.GetFramePosition(i - 1),
                                 decoder.GetFrameSize(i - 1));
        } else {
          canvas_to_draw = canvas_history[i - 1].Copy();
        }
      }

      wxPoint pos = decoder.GetFramePosition(i);
      blendImage(canvas_to_draw, frame, pos.x, pos.y);

      canvas_history.push_back(canvas_to_draw);
      frames.push_back(canvas_to_draw);

      int delay = decoder.GetDelay(i);
      if (delay <= 0) {
        delay = DEFAULT_DELAY_MS;
      }
      delays.push_back(delay);
    }
  }

  return frames;
}

Image::Image(const ::cszb_scoreboard::Size& sz, bool clear)
    : _wx(sz.toWx(), clear) {}

/**
 * Constructs an Image by loading it from a file path.
 *
 * Loads the image data, converts any legacy transparent mask to an alpha
 * channel, and attempts to load it as an animation (such as a GIF) if
 * applicable.
 *
 * @param file The path to the image file.
 */
Image::Image(const FilesystemPath& file) : _wx(file.string()) {
  convertMaskToAlpha(_wx);
  loadAnimation(file);
}

/**
 * Constructs an Image from in-memory binary data.
 *
 * Decodes the image from raw binary data, initializes the alpha channel from
 * mask, and checks if the binary data represents an animated GIF. If it is an
 * animation, decodes and composites all frames and initializes the animation
 * timing structure.
 *
 * @param bin_data The binary data of the image.
 */
Image::Image(const std::vector<char>& bin_data) {
  if (bin_data.empty()) {
    return;
  }
  wxMemoryInputStream inputStream(bin_data.data(), bin_data.size());
  _wx = swx::Image(wxImage(inputStream, wxBITMAP_TYPE_ANY, -1));
  convertMaskToAlpha(_wx);

  wxMemoryInputStream animation_stream(bin_data.data(), bin_data.size());
  wxGIFDecoder decoder;
  if (decoder.LoadGIF(animation_stream) == wxGIF_OK) {
    std::vector<int> input_delays;
    std::vector<wxImage> input_frames =
        decodeAndCompositeGIF(decoder, input_delays);
    if (!input_frames.empty()) {
      frames = std::make_shared<std::vector<wxImage>>(input_frames);
      delays = std::make_shared<std::vector<int>>(input_delays);
      start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count();
    }
  }
}

/**
 * Attempts to load and decode a GIF animation from the specified file.
 *
 * Opens the file stream, decodes it using wxGIFDecoder, and if successful,
 * populates the animated frame list and delays, initializing the start time
 * for animation playback.
 *
 * @param file The path to the GIF file.
 */
void Image::loadAnimation(const FilesystemPath& file) {
  if (!file.existsWithRoot("")) {
    return;
  }
  wxFileInputStream stream(file.string());
  if (!stream.IsOk()) {
    return;
  }
  wxGIFDecoder decoder;
  if (decoder.LoadGIF(stream) == wxGIF_OK) {
    std::vector<int> delaysList;
    std::vector<wxImage> framesList =
        decodeAndCompositeGIF(decoder, delaysList);
    if (!framesList.empty()) {
      frames = std::make_shared<std::vector<wxImage>>(framesList);
      delays = std::make_shared<std::vector<int>>(delaysList);
      start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count();
    }
  }
}

auto Image::size() const -> ::cszb_scoreboard::Size {
  return ::cszb_scoreboard::Size::fromWx(_wx.GetSize());
}

void Image::color(const Color& color) {
  wxRect dimensions(wxPoint(0, 0), _wx.GetSize());
  _wx.SetRGB(dimensions, color.Red(), color.Green(), color.Blue());
}

auto Image::isAnimated() const -> bool { return frames && !frames->empty(); }

/**
 * Retrieves the specific frame of an animation corresponding to the given
 * timestamp.
 *
 * If the image is not animated, returns the static image.
 * Otherwise, calculates the elapsed time since the animation started, takes the
 * remainder relative to the total animation loop duration, and determines which
 * frame should be visible at `current_time_ms` based on individual frame
 * delays.
 *
 * @param current_time_ms The current timestamp in milliseconds.
 * @return The wxImage frame for the current point in time.
 */
auto Image::animate(int64_t current_time_ms) const -> wxImage {
  if (!isAnimated()) {
    return _wx;
  }
  int total_duration = 0;
  for (int delay : *delays) {
    total_duration += delay;
  }
  if (total_duration <= 0) {
    return (*frames)[0];
  }
  int64_t elapsed = current_time_ms - start_time;
  if (elapsed < 0) {
    elapsed = 0;
  }
  int64_t offset = elapsed % total_duration;
  int sum = 0;
  for (size_t i = 0; i < delays->size(); ++i) {
    sum += (*delays)[i];
    if (offset < sum) {
      return (*frames)[i];
    }
  }
  return (*frames).back();
}

/**
 * Rescales the image (and all of its animation frames, if animated) to the
 * specified dimensions.
 *
 * Resizes the primary static image and, if the image contains animation frames,
 * resizes each frame individually to maintain consistent dimensions across
 * playback.
 *
 * @param width The target width.
 * @param height The target height.
 * @param quality The interpolation quality used during resizing.
 * @return A reference to this Image for method chaining.
 */
auto Image::rescale(int width, int height, wxImageResizeQuality quality)
    -> Image& {
  _wx.Rescale(width, height, quality);
  if (isAnimated()) {
    auto new_frames = std::make_shared<std::vector<wxImage>>();
    new_frames->reserve(frames->size());
    for (const auto& frame : *frames) {
      new_frames->push_back(frame.Scale(width, height, quality));
    }
    frames = new_frames;
  }
  return *this;
}

auto Image::isOk() const -> bool { return _wx.IsOk(); }

auto Image::alpha() const -> unsigned char* { return _wx.GetAlpha(); }

auto Image::red(int x, int y) const -> unsigned char {
  return _wx.GetRed(x, y);
}

auto Image::green(int x, int y) const -> unsigned char {
  return _wx.GetGreen(x, y);
}

auto Image::blue(int x, int y) const -> unsigned char {
  return _wx.GetBlue(x, y);
}

void Image::initAlpha() { _wx.InitAlpha(); }

void Image::alpha(unsigned char* alpha) { _wx.SetAlpha(alpha); }

void Image::rgb(const wxRect& rect, unsigned char r, unsigned char g,
                unsigned char b) {
  _wx.SetRGB(rect, r, g, b);
}

void Image::rgb(int x, int y, unsigned char r, unsigned char g,
                unsigned char b) {
  _wx.SetRGB(x, y, r, g, b);
}

}  // namespace cszb_scoreboard
