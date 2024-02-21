#include "platform/GetTextFromKeyboard.h"

extern "C" void GetTextFromKeyboard(const char *message, text_callback callback, void *context)
{
    // $TODO
    callback("*dummy info*", context);
}