typedef void (*text_callback)(const char *text, void *context);

extern "C" void GetTextFromKeyboard(const char *message, text_callback callback, void *context);