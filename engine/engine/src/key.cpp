#include <ovis/engine/key.hpp>
#include <ovis/engine/lua.hpp>

namespace ovis {

std::string_view Key::id() const {
  // clang-format off
  switch (code) {
    case SDL_SCANCODE_1: return "DIGIT_1";
    case SDL_SCANCODE_2: return "DIGIT_2";
    case SDL_SCANCODE_3: return "DIGIT_3";
    case SDL_SCANCODE_4: return "DIGIT_4";
    case SDL_SCANCODE_5: return "DIGIT_5";
    case SDL_SCANCODE_6: return "DIGIT_6";
    case SDL_SCANCODE_7: return "DIGIT_7";
    case SDL_SCANCODE_8: return "DIGIT_8";
    case SDL_SCANCODE_9: return "DIGIT_9";
    case SDL_SCANCODE_0: return "DIGIT_0";
    case SDL_SCANCODE_MINUS: return "MINUS";
    case SDL_SCANCODE_EQUALS: return "EQUAL";
    case SDL_SCANCODE_KP_0: return "NUMPAD_0";
    case SDL_SCANCODE_KP_1: return "NUMPAD_1";
    case SDL_SCANCODE_KP_2: return "NUMPAD_2";
    case SDL_SCANCODE_KP_3: return "NUMPAD_3";
    case SDL_SCANCODE_KP_4: return "NUMPAD_4";
    case SDL_SCANCODE_KP_5: return "NUMPAD_5";
    case SDL_SCANCODE_KP_6: return "NUMPAD_6";
    case SDL_SCANCODE_KP_7: return "NUMPAD_7";
    case SDL_SCANCODE_KP_8: return "NUMPAD_8";
    case SDL_SCANCODE_KP_9: return "NUMPAD_9";
    case SDL_SCANCODE_KP_PERIOD: return "NUMPAD_DECIMAL";
    case SDL_SCANCODE_KP_ENTER: return "NUMPAD_ENTER";
    case SDL_SCANCODE_KP_PLUS: return "NUMPAD_ADD";
    case SDL_SCANCODE_KP_MINUS: return "NUMPAD_SUBTRACT";
    case SDL_SCANCODE_KP_MULTIPLY: return "NUMPAD_MULTIPLY";
    case SDL_SCANCODE_KP_DIVIDE: return "NUMPAD_DIVIDE";
    case SDL_SCANCODE_ESCAPE: return "ESCAPE";
    case SDL_SCANCODE_INSERT: return "INSERT";
    case SDL_SCANCODE_DELETE: return "DELETE";
    case SDL_SCANCODE_HOME: return "HOME";
    case SDL_SCANCODE_END: return "END";
    case SDL_SCANCODE_PAGEUP: return "PAGE_UP";
    case SDL_SCANCODE_PAGEDOWN: return "PAGE_DOWN";
    case SDL_SCANCODE_TAB: return "TAB";
    case SDL_SCANCODE_RETURN: return "ENTER";
    case SDL_SCANCODE_BACKSPACE: return "BACKSPACE";
    case SDL_SCANCODE_LCTRL: return "CONTROL_LEFT";
    case SDL_SCANCODE_RCTRL: return "CONTROL_RIGHT";
    case SDL_SCANCODE_LALT: return "ALT_LEFT";
    case SDL_SCANCODE_RALT: return "ALT_RIGHT";
    case SDL_SCANCODE_LGUI: return "META_LEFT";
    case SDL_SCANCODE_RGUI: return "META_RIGHT";
    case SDL_SCANCODE_LSHIFT: return "SHIFT_LEFT";
    case SDL_SCANCODE_RSHIFT: return "SHIFT_RIGHT";
    case SDL_SCANCODE_UP: return "ARROW_UP";
    case SDL_SCANCODE_DOWN: return "ARROW_DOWN";
    case SDL_SCANCODE_LEFT: return "ARROW_LEFT";
    case SDL_SCANCODE_RIGHT: return "ARROW_RIGHT";
    case SDL_SCANCODE_GRAVE: return "BACKQUOTE";
    case SDL_SCANCODE_LEFTBRACKET: return "BRACKET_LEFT";
    case SDL_SCANCODE_RIGHTBRACKET: return "BRACKET_RIGHT";
    case SDL_SCANCODE_SEMICOLON: return "SEMICOLON";
    case SDL_SCANCODE_APOSTROPHE: return "QUOTE";
    case SDL_SCANCODE_BACKSLASH: return "BACKSLASH";
    case SDL_SCANCODE_COMMA: return "COMMA";
    case SDL_SCANCODE_PERIOD: return "PERIOD";
    case SDL_SCANCODE_SLASH: return "SLASH";
    case SDL_SCANCODE_NONUSBACKSLASH: return "INTL_BACKSLASH";
    case SDL_SCANCODE_SPACE: return "SPACE";
    case SDL_SCANCODE_A: return "KEY_A";
    case SDL_SCANCODE_B: return "KEY_B";
    case SDL_SCANCODE_C: return "KEY_C";
    case SDL_SCANCODE_D: return "KEY_D";
    case SDL_SCANCODE_E: return "KEY_E";
    case SDL_SCANCODE_F: return "KEY_F";
    case SDL_SCANCODE_G: return "KEY_G";
    case SDL_SCANCODE_H: return "KEY_H";
    case SDL_SCANCODE_I: return "KEY_I";
    case SDL_SCANCODE_J: return "KEY_J";
    case SDL_SCANCODE_K: return "KEY_K";
    case SDL_SCANCODE_L: return "KEY_L";
    case SDL_SCANCODE_M: return "KEY_M";
    case SDL_SCANCODE_N: return "KEY_N";
    case SDL_SCANCODE_O: return "KEY_O";
    case SDL_SCANCODE_P: return "KEY_P";
    case SDL_SCANCODE_Q: return "KEY_Q";
    case SDL_SCANCODE_R: return "KEY_R";
    case SDL_SCANCODE_S: return "KEY_S";
    case SDL_SCANCODE_T: return "KEY_T";
    case SDL_SCANCODE_U: return "KEY_U";
    case SDL_SCANCODE_V: return "KEY_V";
    case SDL_SCANCODE_W: return "KEY_W";
    case SDL_SCANCODE_X: return "KEY_X";
    case SDL_SCANCODE_Y: return "KEY_Y";
    case SDL_SCANCODE_Z: return "KEY_Z";
    case SDL_SCANCODE_F1: return "F1";
    case SDL_SCANCODE_F2: return "F2";
    case SDL_SCANCODE_F3: return "F3";
    case SDL_SCANCODE_F4: return "F4";
    case SDL_SCANCODE_F5: return "F5";
    case SDL_SCANCODE_F6: return "F6";
    case SDL_SCANCODE_F7: return "F7";
    case SDL_SCANCODE_F8: return "F8";
    case SDL_SCANCODE_F9: return "F9";
    case SDL_SCANCODE_F10: return "F10";
    case SDL_SCANCODE_F11: return "F11";
    case SDL_SCANCODE_F12: return "F12";
    case SDL_SCANCODE_F13: return "F13";
    case SDL_SCANCODE_F14: return "F14";
    case SDL_SCANCODE_F15: return "F15";
    case SDL_SCANCODE_F16: return "F16";
    case SDL_SCANCODE_F17: return "F17";
    case SDL_SCANCODE_F18: return "F18";
    case SDL_SCANCODE_F19: return "F19";
    case SDL_SCANCODE_F20: return "F20";
    case SDL_SCANCODE_F21: return "F21";
    case SDL_SCANCODE_F22: return "F22";
    case SDL_SCANCODE_F23: return "F23";
    case SDL_SCANCODE_F24: return "F24";
    default: return "";
  }
  // clang-format on
}

std::string_view Key::name() const {
  // clang-format off
  switch (code) {
    case SDL_SCANCODE_1: return "Digit1";
    case SDL_SCANCODE_2: return "Digit2";
    case SDL_SCANCODE_3: return "Digit3";
    case SDL_SCANCODE_4: return "Digit4";
    case SDL_SCANCODE_5: return "Digit5";
    case SDL_SCANCODE_6: return "Digit6";
    case SDL_SCANCODE_7: return "Digit7";
    case SDL_SCANCODE_8: return "Digit8";
    case SDL_SCANCODE_9: return "Digit9";
    case SDL_SCANCODE_0: return "Digit0";
    case SDL_SCANCODE_MINUS: return "Minus";
    case SDL_SCANCODE_EQUALS: return "Equal";

    case SDL_SCANCODE_KP_0: return "Numpad0";
    case SDL_SCANCODE_KP_1: return "Numpad1";
    case SDL_SCANCODE_KP_2: return "Numpad2";
    case SDL_SCANCODE_KP_3: return "Numpad3";
    case SDL_SCANCODE_KP_4: return "Numpad4";
    case SDL_SCANCODE_KP_5: return "Numpad5";
    case SDL_SCANCODE_KP_6: return "Numpad6";
    case SDL_SCANCODE_KP_7: return "Numpad7";
    case SDL_SCANCODE_KP_8: return "Numpad8";
    case SDL_SCANCODE_KP_9: return "Numpad9";
    case SDL_SCANCODE_KP_PERIOD: return "NumpadDecimal";
    case SDL_SCANCODE_KP_ENTER: return "NumpadEnter";
    case SDL_SCANCODE_KP_PLUS: return "NumpadAdd";
    case SDL_SCANCODE_KP_MINUS: return "NumpadSubtract";
    case SDL_SCANCODE_KP_MULTIPLY: return "NumpadMultiply";
    case SDL_SCANCODE_KP_DIVIDE: return "NumpadDivide";

    case SDL_SCANCODE_ESCAPE: return "Escape";
    case SDL_SCANCODE_INSERT: return "Insert";
    case SDL_SCANCODE_DELETE: return "Delete";
    case SDL_SCANCODE_HOME: return "Home";
    case SDL_SCANCODE_END: return "End";
    case SDL_SCANCODE_PAGEUP: return "PageUp";
    case SDL_SCANCODE_PAGEDOWN: return "PageDown";
    case SDL_SCANCODE_TAB: return "Tab";
    case SDL_SCANCODE_RETURN: return "Enter";
    case SDL_SCANCODE_BACKSPACE: return "Backspace";
  
    case SDL_SCANCODE_LCTRL: return "ControlLeft";
    case SDL_SCANCODE_RCTRL: return "ControlRight";
    case SDL_SCANCODE_LALT: return "AltLeft";
    case SDL_SCANCODE_RALT: return "AltRight";
    case SDL_SCANCODE_LGUI: return "MetaLeft";
    case SDL_SCANCODE_RGUI: return "MetaRight";
    case SDL_SCANCODE_LSHIFT: return "ShiftLeft";
    case SDL_SCANCODE_RSHIFT: return "ShiftRight";
  
    case SDL_SCANCODE_UP: return "ArrowUp";
    case SDL_SCANCODE_DOWN: return "ArrowDown";
    case SDL_SCANCODE_LEFT: return "ArrowLeft";
    case SDL_SCANCODE_RIGHT: return "ArrowRight";

    case SDL_SCANCODE_GRAVE: return "Backquote";
    case SDL_SCANCODE_LEFTBRACKET: return "BracketLeft";
    case SDL_SCANCODE_RIGHTBRACKET: return "BracketRight";
    case SDL_SCANCODE_SEMICOLON: return "Semicolon";
    case SDL_SCANCODE_APOSTROPHE: return "Quote";
    case SDL_SCANCODE_BACKSLASH: return "Backslash";
    case SDL_SCANCODE_COMMA: return "Comma";
    case SDL_SCANCODE_PERIOD: return "Period";
    case SDL_SCANCODE_SLASH: return "Slash";
    case SDL_SCANCODE_NONUSBACKSLASH: return "IntlBackslash";
    case SDL_SCANCODE_SPACE: return "Space";

    case SDL_SCANCODE_A: return "KeyA";
    case SDL_SCANCODE_B: return "KeyB";
    case SDL_SCANCODE_C: return "KeyC";
    case SDL_SCANCODE_D: return "KeyD";
    case SDL_SCANCODE_E: return "KeyE";
    case SDL_SCANCODE_F: return "KeyF";
    case SDL_SCANCODE_G: return "KeyG";
    case SDL_SCANCODE_H: return "KeyH";
    case SDL_SCANCODE_I: return "KeyI";
    case SDL_SCANCODE_J: return "KeyJ";
    case SDL_SCANCODE_K: return "KeyK";
    case SDL_SCANCODE_L: return "KeyL";
    case SDL_SCANCODE_M: return "KeyM";
    case SDL_SCANCODE_N: return "KeyN";
    case SDL_SCANCODE_O: return "KeyO";
    case SDL_SCANCODE_P: return "KeyP";
    case SDL_SCANCODE_Q: return "KeyQ";
    case SDL_SCANCODE_R: return "KeyR";
    case SDL_SCANCODE_S: return "KeyS";
    case SDL_SCANCODE_T: return "KeyT";
    case SDL_SCANCODE_U: return "KeyU";
    case SDL_SCANCODE_V: return "KeyV";
    case SDL_SCANCODE_W: return "KeyW";
    case SDL_SCANCODE_X: return "KeyX";
    case SDL_SCANCODE_Y: return "KeyY";
    case SDL_SCANCODE_Z: return "KeyZ";

    case SDL_SCANCODE_F1: return "F1";
    case SDL_SCANCODE_F2: return "F2";
    case SDL_SCANCODE_F3: return "F3";
    case SDL_SCANCODE_F4: return "F4";
    case SDL_SCANCODE_F5: return "F5";
    case SDL_SCANCODE_F6: return "F6";
    case SDL_SCANCODE_F7: return "F7";
    case SDL_SCANCODE_F8: return "F8";
    case SDL_SCANCODE_F9: return "F9";
    case SDL_SCANCODE_F10: return "F10";
    case SDL_SCANCODE_F11: return "F11";
    case SDL_SCANCODE_F12: return "F12";
    case SDL_SCANCODE_F13: return "F13";
    case SDL_SCANCODE_F14: return "F14";
    case SDL_SCANCODE_F15: return "F15";
    case SDL_SCANCODE_F16: return "F16";
    case SDL_SCANCODE_F17: return "F17";
    case SDL_SCANCODE_F18: return "F18";
    case SDL_SCANCODE_F19: return "F19";
    case SDL_SCANCODE_F20: return "F20";
    case SDL_SCANCODE_F21: return "F21";
    case SDL_SCANCODE_F22: return "F22";
    case SDL_SCANCODE_F23: return "F23";
    case SDL_SCANCODE_F24: return "F24";
    default: return "";
  }
  // clang-format on
}

void RegisterToLua() {
  sol::usertype<Key> key_type = Lua::state.new_usertype<Key>("Key", sol::no_constructor);

  sol::table key_table = Lua::state["Key"];
  key_table["DIGIT_1"] = Key::DIGIT_1;
  key_table["DIGIT_2"] = Key::DIGIT_2;
  key_table["DIGIT_3"] = Key::DIGIT_3;
  key_table["DIGIT_4"] = Key::DIGIT_4;
  key_table["DIGIT_5"] = Key::DIGIT_5;
  key_table["DIGIT_6"] = Key::DIGIT_6;
  key_table["DIGIT_7"] = Key::DIGIT_7;
  key_table["DIGIT_8"] = Key::DIGIT_8;
  key_table["DIGIT_9"] = Key::DIGIT_9;
  key_table["DIGIT_0"] = Key::DIGIT_0;
  key_table["MINUS"] = Key::MINUS;
  key_table["EQUAL"] = Key::EQUAL;
  key_table["NUMPAD_0"] = Key::NUMPAD_0;
  key_table["NUMPAD_1"] = Key::NUMPAD_1;
  key_table["NUMPAD_2"] = Key::NUMPAD_2;
  key_table["NUMPAD_3"] = Key::NUMPAD_3;
  key_table["NUMPAD_4"] = Key::NUMPAD_4;
  key_table["NUMPAD_5"] = Key::NUMPAD_5;
  key_table["NUMPAD_6"] = Key::NUMPAD_6;
  key_table["NUMPAD_7"] = Key::NUMPAD_7;
  key_table["NUMPAD_8"] = Key::NUMPAD_8;
  key_table["NUMPAD_9"] = Key::NUMPAD_9;
  key_table["NUMPAD_DECIMAL"] = Key::NUMPAD_DECIMAL;
  key_table["NUMPAD_ENTER"] = Key::NUMPAD_ENTER;
  key_table["NUMPAD_ADD"] = Key::NUMPAD_ADD;
  key_table["NUMPAD_SUBTRACT"] = Key::NUMPAD_SUBTRACT;
  key_table["NUMPAD_MULTIPLY"] = Key::NUMPAD_MULTIPLY;
  key_table["NUMPAD_DIVIDE"] = Key::NUMPAD_DIVIDE;
  key_table["ESCAPE"] = Key::ESCAPE;
  key_table["INSERT"] = Key::INSERT;
  key_table["DELETE"] = Key::DELETE;
  key_table["HOME"] = Key::HOME;
  key_table["END"] = Key::END;
  key_table["PAGE_UP"] = Key::PAGE_UP;
  key_table["PAGE_DOWN"] = Key::PAGE_DOWN;
  key_table["TAB"] = Key::TAB;
  key_table["ENTER"] = Key::ENTER;
  key_table["BACKSPACE"] = Key::BACKSPACE;
  key_table["CONTROL_LEFT"] = Key::CONTROL_LEFT;
  key_table["CONTROL_RIGHT"] = Key::CONTROL_RIGHT;
  key_table["ALT_LEFT"] = Key::ALT_LEFT;
  key_table["ALT_RIGHT"] = Key::ALT_RIGHT;
  key_table["META_LEFT"] = Key::META_LEFT;
  key_table["META_RIGHT"] = Key::META_RIGHT;
  key_table["SHIFT_LEFT"] = Key::SHIFT_LEFT;
  key_table["SHIFT_RIGHT"] = Key::SHIFT_RIGHT;
  key_table["ARROW_UP"] = Key::ARROW_UP;
  key_table["ARROW_DOWN"] = Key::ARROW_DOWN;
  key_table["ARROW_LEFT"] = Key::ARROW_LEFT;
  key_table["ARROW_RIGHT"] = Key::ARROW_RIGHT;
  key_table["BACKQUOTE"] = Key::BACKQUOTE;
  key_table["BRACKET_LEFT"] = Key::BRACKET_LEFT;
  key_table["BRACKET_RIGHT"] = Key::BRACKET_RIGHT;
  key_table["SEMICOLON"] = Key::SEMICOLON;
  key_table["QUOTE"] = Key::QUOTE;
  key_table["BACKSLASH"] = Key::BACKSLASH;
  key_table["COMMA"] = Key::COMMA;
  key_table["PERIOD"] = Key::PERIOD;
  key_table["SLASH"] = Key::SLASH;
  key_table["INTL_BACKSLASH"] = Key::INTL_BACKSLASH;
  key_table["SPACE"] = Key::SPACE;
  key_table["KEY_A"] = Key::KEY_A;
  key_table["KEY_B"] = Key::KEY_B;
  key_table["KEY_C"] = Key::KEY_C;
  key_table["KEY_D"] = Key::KEY_D;
  key_table["KEY_E"] = Key::KEY_E;
  key_table["KEY_F"] = Key::KEY_F;
  key_table["KEY_G"] = Key::KEY_G;
  key_table["KEY_H"] = Key::KEY_H;
  key_table["KEY_I"] = Key::KEY_I;
  key_table["KEY_J"] = Key::KEY_J;
  key_table["KEY_K"] = Key::KEY_K;
  key_table["KEY_L"] = Key::KEY_L;
  key_table["KEY_M"] = Key::KEY_M;
  key_table["KEY_N"] = Key::KEY_N;
  key_table["KEY_O"] = Key::KEY_O;
  key_table["KEY_P"] = Key::KEY_P;
  key_table["KEY_Q"] = Key::KEY_Q;
  key_table["KEY_R"] = Key::KEY_R;
  key_table["KEY_S"] = Key::KEY_S;
  key_table["KEY_T"] = Key::KEY_T;
  key_table["KEY_U"] = Key::KEY_U;
  key_table["KEY_V"] = Key::KEY_V;
  key_table["KEY_W"] = Key::KEY_W;
  key_table["KEY_X"] = Key::KEY_X;
  key_table["KEY_Y"] = Key::KEY_Y;
  key_table["KEY_Z"] = Key::KEY_Z;
  key_table["F1"] = Key::F1;
  key_table["F2"] = Key::F2;
  key_table["F3"] = Key::F3;
  key_table["F4"] = Key::F4;
  key_table["F5"] = Key::F5;
  key_table["F6"] = Key::F6;
  key_table["F7"] = Key::F7;
  key_table["F8"] = Key::F8;
  key_table["F9"] = Key::F9;
  key_table["F10"] = Key::F10;
  key_table["F11"] = Key::F11;
  key_table["F12"] = Key::F12;
  key_table["F13"] = Key::F13;
  key_table["F14"] = Key::F14;
  key_table["F15"] = Key::F15;
  key_table["F16"] = Key::F16;
  key_table["F17"] = Key::F17;
  key_table["F18"] = Key::F18;
  key_table["F19"] = Key::F19;
  key_table["F20"] = Key::F20;
  key_table["F21"] = Key::F21;
  key_table["F22"] = Key::F22;
  key_table["F23"] = Key::F23;
  key_table["F24"] = Key::F24;
}

}  // namespace ovis
