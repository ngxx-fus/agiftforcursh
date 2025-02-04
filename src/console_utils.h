#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#if SHOW_AUTHOR_MESSAGE == true
    #pragma message("\nIncluded console_utils.h! (legacy header)\n")
#endif

#include "Arduino.h"
#include <tft_utils.h>
#include <serial_utils.h>

#include <algorithm>
#include <vector>
using namespace std;

template<class T> using vct = vector<T>;

/// @brief Sync to serial
#ifndef SYNC_TO_SERIAL
    #if defined(SERIAL_UTILS_H)
        #define SYNC_TO_SERIAL true
    #endif
#endif

/// @brief Maximun # of line in console
#ifndef MAX_LINES
#define MAX_LINES 15
#endif

/// @brief Display console to tft screen after any changed
#ifndef SHOW_AFTER_CHANGED
#define SHOW_AFTER_CHANGED false
#endif

/// @brief console's content (buffer = 10 lines, no wrap).
String console[MAX_LINES];

/// @brief The pointer to the current line is shown.
int8_t line_ptr = -1;

/// @brief Display console in tft screen.
/// @param clear_all Run tft.clear to clear all screen
/// @param line_inv  Invert the color at the line <line_inv>
/// @note The line is inverted that have light backgroud and black text.
void console_show(bool clear_all = false, int8_t line_inv = -1){
    if(clear_all) tft.clear();
    for(int8_t i = 0; i <= line_ptr; ++i){
        if(line_inv == i){
            tft.clearTextLine(i, 0xF, 0xFFFF);
            tft.drawGFXTextLine(i, console[i], 0x1, 0xF);
        }else{
            if(!clear_all) tft.clearTextLine(i);
            tft.drawGFXTextLine(i, console[i], 0xFFFF, 0xF);
        }
    }
    delay(300);
}

/// @brief Clear console and tft screen.
void console_clear(bool clear_screen = true){
    if(clear_screen) tft.clear();
    line_ptr = -1;
}

/// @brief Scroll console up by n line(s).
/// @param n # of line(s) will be scrolled up.
void console_scroll_up(int8_t n = 1){
    /// nothin' to scroll
    if(line_ptr < 0 || line_ptr < n || n < 0) return;
    /// scroll by n
    line_ptr -= n;
    for(int8_t i = 0; i <= line_ptr - n; ++i){
        console[i] = console[i+n];
    }
    #if SHOW_AFTER_CHANGED == true
        console_show();
    #endif
}

/// @brief Push new message onto console
/// @tparam Tmsg Any type (will be auto casted to 
///         <String> if the type is supported!)
/// @param msg The message to be displayed on console.
/// @note With each message having no more than 15 characters!
template<class Tmsg>
void console_push_msg( Tmsg msg){
    ++line_ptr;
    if(line_ptr == MAX_LINES) {
        console_scroll_up();
    }
    console[line_ptr] = String(msg);
    #if SHOW_AFTER_CHANGED == true
        console_show();
    #endif
    #if SYNC_TO_SERIAL == true
        msg2ser(msg);
    #endif
}

/// @brief Push new messages onto console
/// @tparam Tmsg Any type (will be auto casted to 
///         <String> if the type is supported!)
/// @tparam ...Tmsgs Similar to Tmsg
/// @param msg The message to be displayed on console.
/// @param msgs The messages to be displayed on console. 
/// @note With each message having no more than 15 characters!
/// @note Each message will display one line.
template<class Tmsg, class...Tmsgs>
void console_push_msg( Tmsg msg, Tmsgs... msgs){
    console_push_msg(msg);
    console_push_msg(msgs...);
}

/// @brief Upodate new messages onto console
/// @param line The line to be displayed on console (0, ...)
/// @param new_msg The message to be updated on the line
void console_set_line(int8_t line, String new_msg){
    if(line > line_ptr) return;
    console[line] = new_msg;
}

#endif