#ifndef _GLOBAL_H
#define _GLOBAL_H

//#define _DEVELOPMENT_ 1
#define LITE_VERSION 1

typedef int         (*global_error_handler_t)(const char *format, ...);
//typedef const char *(global_translate_path_t)(const char *filename);
//typedef const char *(global_read_text_file_t)(const char *filename);

//extern global_error_handler_t globalErrorHandler;
//extern global_translate_path_t globalTranslatePath;
//extern global_read_text_file_t globalReadTextFile;

#define globalErrorHandler printf

//extern "C" getGlobalErrorHandler();

const char *globalReadTextFile(const char *);
const char *globalTranslatePath(const char *);
const char *globalTranslateReadWritePath(const char *);

#endif // _GLOBAL_H