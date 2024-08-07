// A stripped down version of https://github.com/chromium/chromium/blob/main/ui/events/keycodes/dom/keycode_converter.cc
// Last Updated - August 7, 2024: https://github.com/chromium/chromium/commit/24d53e3e9a861c76d0e1626522c93e371f5d9223

// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "keycode_converter.h"

#include <string_view>

#include "dom_code.h"

namespace ui {

namespace {

// Table of USB codes (equivalent to DomCode values), native scan codes,
// and DOM Level 3 |code| strings.
#if defined(_WIN32)
#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) \
  { usb, win, code }
#elif defined(__linux__)
#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) \
  { usb, xkb, code }
#elif defined(__APPLE__)
#define DOM_CODE(usb, evdev, xkb, win, mac, code, id) \
  { usb, mac, code }
#else
#error Unsupported platform
#endif
#define DOM_CODE_DECLARATION constexpr KeycodeMapEntry kDomCodeMappings[] =
#include "dom_code_data.inc"
#undef DOM_CODE
#undef DOM_CODE_DECLARATION

}  // namespace

// static
int KeycodeConverter::InvalidNativeKeycode() {
  return kDomCodeMappings[0].native_keycode;
}

// TODO(zijiehe): Most of the following functions can be optimized by using
// either multiple arrays or unordered_map.

// static
int KeycodeConverter::DomCodeToNativeKeycode(DomCode code) {
  return UsbKeycodeToNativeKeycode(static_cast<uint32_t>(code));
}

// USB keycodes
// Note that USB keycodes are not part of any web standard.
// Please don't use USB keycodes in new code.

// static
int KeycodeConverter::UsbKeycodeToNativeKeycode(uint32_t usb_keycode) {
  // Deal with some special-cases that don't fit the 1:1 mapping.
  if (usb_keycode == 0x070032)  // non-US hash.
    usb_keycode = 0x070031;     // US backslash.
#ifdef __APPLE__
  if (usb_keycode == 0x070046) // PrintScreen.
    usb_keycode = 0x070068; // F13.
#endif

  for (auto& mapping : kDomCodeMappings) {
    if (mapping.usb_keycode == usb_keycode)
      return mapping.native_keycode;
  }
  return InvalidNativeKeycode();
}

}  // namespace ui
