#ifndef DOMCODECONVERTER_H_
#define DOMCODECONVERTER_H_

#include <map>

#include "chromium/dom_code.h"

using namespace std;
using namespace ui;

// WARN(winter): This might not be right all the time! Tested on Windows 10 with US Keyboard Layout with English (United States) locale and UTF-8 enabled
// TODO(winter): Incomplete! Populate with https://github.com/chromium/chromium/blob/master/ui/events/keycodes/keyboard_defines_win.h
// TODO(winter): Make LinuxToDomMap with https://github.com/chromium/chromium/blob/master/ui/events/keycodes/keyboard_codes_posix.h
map<int, DomCode> WindowsToDomMap = {
	// Control (1)
	{0x08, DomCode::BACKSPACE},
	{0x09, DomCode::TAB},
	{0x0D, DomCode::ENTER},
	{0x10, DomCode::SHIFT_LEFT},
	//{0x10, DomCode::SHIFT_RIGHT}, //  TODO: Duplicate!
	{0x11, DomCode::CONTROL_LEFT},
	//{0x11, DomCode::CONTROL_RIGHT}, // TODO: Duplicate!
	{0x12, DomCode::ALT_LEFT},
	//{0x12, DomCode::ALT_RIGHT}, // TODO: Duplicate!
	{0x13, DomCode::PAUSE},
	{0x14, DomCode::CAPS_LOCK},
	{0x1B, DomCode::ESCAPE},
	{0x20, DomCode::SPACE},

	// Control (2)
	{0x21, DomCode::PAGE_UP},
	{0x22, DomCode::PAGE_DOWN},
	{0x23, DomCode::END},
	{0x24, DomCode::HOME},
	{0x25, DomCode::ARROW_LEFT},
	{0x26, DomCode::ARROW_UP},
	{0x27, DomCode::ARROW_RIGHT},
	{0x28, DomCode::ARROW_DOWN},
	{0x29, DomCode::SELECT},
	{0x2A, DomCode::PRINT},
	{0x2C, DomCode::PRINT_SCREEN},
	{0x2D, DomCode::INSERT},
	{0x2E, DomCode::DEL},

	// Digits
	{0x30, DomCode::DIGIT0},
	{0x31, DomCode::DIGIT1},
	{0x32, DomCode::DIGIT2},
	{0x33, DomCode::DIGIT3},
	{0x34, DomCode::DIGIT4},
	{0x35, DomCode::DIGIT5},
	{0x36, DomCode::DIGIT6},
	{0x37, DomCode::DIGIT7},
	{0x38, DomCode::DIGIT8},
	{0x39, DomCode::DIGIT9},

	// Symbols (2)
	{0x3A, DomCode::SEMICOLON}, // :
	{0x3B, DomCode::SEMICOLON}, // ;
	{0x3C, DomCode::COMMA}, // <
	{0x3D, DomCode::EQUAL}, // =
	{0x3E, DomCode::PERIOD}, // >
	{0x3F, DomCode::SLASH}, // ?
	{0x40, DomCode::DIGIT2}, // @

	// Uppercase
	{0x41, DomCode::US_A},
	{0x42, DomCode::US_B},
	{0x43, DomCode::US_C},
	{0x44, DomCode::US_D},
	{0x45, DomCode::US_E},
	{0x46, DomCode::US_F},
	{0x47, DomCode::US_G},
	{0x48, DomCode::US_H},
	{0x49, DomCode::US_I},
	{0x4A, DomCode::US_J},
	{0x4B, DomCode::US_K},
	{0x4C, DomCode::US_L},
	{0x4D, DomCode::US_M},
	{0x4E, DomCode::US_N},
	{0x4F, DomCode::US_O},
	{0x50, DomCode::US_P},
	{0x51, DomCode::US_Q},
	{0x52, DomCode::US_R},
	{0x53, DomCode::US_S},
	{0x54, DomCode::US_T},
	{0x55, DomCode::US_U},
	{0x56, DomCode::US_V},
	{0x57, DomCode::US_W},
	{0x58, DomCode::US_X},
	{0x59, DomCode::US_Y},
	{0x5A, DomCode::US_Z},

	// Control (3)
	// TODO: Conflict!
	//{0x5B, DomCode::META_LEFT},
	//{0x5C, DomCode::META_RIGHT},

	// Symbols (3)
	{0x5B, DomCode::BRACKET_LEFT}, // [
	{0x5C, DomCode::BACKSLASH}, // \ (backslash)
	{0x5D, DomCode::BRACKET_RIGHT}, // ]
	{0x5E, DomCode::DIGIT6}, // ^
	{0x5F, DomCode::MINUS}, // _
	{0x60, DomCode::BACKQUOTE}, // `

	// Lowercase
	// TODO: This makes sense according to UTF, but not Windows' VK_* defines...
	{0x61, DomCode::US_A},
	{0x62, DomCode::US_B},
	{0x63, DomCode::US_C},
	{0x64, DomCode::US_D},
	{0x65, DomCode::US_E},
	{0x66, DomCode::US_F},
	{0x67, DomCode::US_G},
	{0x68, DomCode::US_H},
	{0x69, DomCode::US_I},
	{0x6A, DomCode::US_J},
	{0x6B, DomCode::US_K},
	{0x6C, DomCode::US_L},
	{0x6D, DomCode::US_M},
	{0x6E, DomCode::US_N},
	{0x6F, DomCode::US_O},
	{0x70, DomCode::US_P},
	{0x71, DomCode::US_Q},
	{0x72, DomCode::US_R},
	{0x73, DomCode::US_S},
	{0x74, DomCode::US_T},
	{0x75, DomCode::US_U},
	{0x76, DomCode::US_V},
	{0x77, DomCode::US_W},
	{0x78, DomCode::US_X},
	{0x79, DomCode::US_Y},
	{0x7A, DomCode::US_Z},

	// Symbols (4)
	{0x7B, DomCode::BRACKET_LEFT}, // {
	{0x7C, DomCode::BACKSLASH}, // |
	{0x7D, DomCode::BRACKET_RIGHT}, // }
	{0x7E, DomCode::BACKQUOTE}, // ~
};

// Conflicts with the normal map...but symbols seem to be broken in a way we can detect!
map<int, DomCode> NullKeyDownWindowsToDomMap = {
	// Symbols (1)
	{0x21, DomCode::DIGIT1}, // !
	{0x22, DomCode::QUOTE}, // "
	{0x23, DomCode::DIGIT3}, // #
	{0x24, DomCode::DIGIT4}, // $
	{0x25, DomCode::DIGIT5}, // %
	{0x26, DomCode::DIGIT7}, // &
	{0x27, DomCode::QUOTE}, // '
	{0x28, DomCode::DIGIT9}, // (
	{0x29, DomCode::DIGIT0}, // )
	{0x2A, DomCode::DIGIT8}, // *
	{0x2B, DomCode::EQUAL}, // +
	{0x2C, DomCode::COMMA}, // ,
	{0x2D, DomCode::MINUS}, // -
	{0x2E, DomCode::PERIOD}, // .
	{0x2F, DomCode::SLASH}, // /
};

// static?
DomCode WindowsKeyCodeToDomCode(int windows_key_code, bool lastkeydown_null) {
	if (lastkeydown_null) {
		if (windows_key_code > 0 && NullKeyDownWindowsToDomMap.count(windows_key_code) == 1) {
			return NullKeyDownWindowsToDomMap.at(windows_key_code);
		}
	} else {
		if (windows_key_code > 0 && WindowsToDomMap.count(windows_key_code) == 1) {
			return WindowsToDomMap.at(windows_key_code);
		}
	}

	return DomCode::NONE;
}

#endif  // DOMCODECONVERTER_H_
