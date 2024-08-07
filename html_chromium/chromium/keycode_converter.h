// A stripped down version of https://github.com/chromium/chromium/blob/main/ui/events/keycodes/dom/keycode_converter.h
// Last Updated - August 7, 2024: https://github.com/chromium/chromium/commit/36619c19f1c497cb673f1a5d1b82e20da90663df

// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_KEYCODES_DOM_KEYCODE_CONVERTER_H_
#define UI_EVENTS_KEYCODES_DOM_KEYCODE_CONVERTER_H_

#include <stddef.h>
#include <stdint.h>

#include <string_view>

#include "dom_code.h"

// For reference, the W3C UI Event spec is located at:
// http://www.w3.org/TR/uievents/

namespace ui {

// This structure is used to define the keycode mapping table.
// It is defined here because the unittests need access to it.
typedef struct {
  // USB keycode:
  //  Upper 16-bits: USB Usage Page.
  //  Lower 16-bits: USB Usage Id: Assigned ID within this usage page.
  uint32_t usb_keycode;

  // Contains one of the following:
  //  On Linux: XKB scancode
  //  On Windows: Windows OEM scancode
  //  On Mac: Mac keycode
  //  On Fuchsia: 16-bit Code from the USB Keyboard Usage Page.
  int native_keycode;

  // The UIEvents (aka: DOM4Events) |code| value as defined in:
  // http://www.w3.org/TR/DOM-Level-3-Events-code/
  const char* code;
} KeycodeMapEntry;

// A class to convert between the current platform's native keycode (scancode)
// and platform-neutral |code| values (as defined in the W3C UI Events
// spec (http://www.w3.org/TR/uievents/).
class KeycodeConverter {
 public:
  KeycodeConverter() = delete;
  KeycodeConverter(const KeycodeConverter&) = delete;
  KeycodeConverter& operator=(const KeycodeConverter&) = delete;

  // Return the value that identifies an invalid native keycode.
  static int InvalidNativeKeycode();

  // Convert a DomCode into a native keycode.
  static int DomCodeToNativeKeycode(DomCode code);

  // The following methods relate to USB keycodes.
  // Note that USB keycodes are not part of any web standard.
  // Please don't use USB keycodes in new code.

  // Conversion between USB keycode and native keycode values.
  // Returns the invalid value if the supplied code is not recognized,
  // or has no mapping.
  static int UsbKeycodeToNativeKeycode(uint32_t usb_keycode);
};

}  // namespace ui

#endif  // UI_EVENTS_KEYCODES_DOM_KEYCODE_CONVERTER_H_
