/**
 * To enable various logging levels, either use #define PNG_LOG_ERROR, #define PNG_LOG_WARNING, or #define PNG_LOG_INFO
 */
#ifndef PNG_LOGGER_H
#define PNG_LOGGER_H

#include <stdio.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define PNG_LOG_ERROR

#if defined(PNG_LOG_ERROR) || defined(PNG_LOG_WARNING) || defined(PNG_LOG_INFO) 
// TODO: https://stackoverflow.com/questions/20639632/how-to-wrap-printf-into-a-function-or-macro
// TODO: fix this
#define _LOG_HELPER(fmt, ...) printf(fmt "\n%s", __VA_ARGS__)
#define PNG_LOGE(...) _LOG_HELPER(__VA_ARGS__, "")
#if defined(PNG_LOG_WARNING) || defined(PNG_LOG_INFO) 
#define PNG_LOGW(fmt, ...) printf("[WARN ] %s:%d: " fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#if defined(PNG_LOG_INFO) 
#define PNG_LOGI(fmt, ...) printf("[INFO ] %s:%d: " fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#else
#define PNG_LOGI(...)
#endif // LOG_INFO
#else
#define PNG_LOGW(...)
#endif // LOG_WARN
#else
#define PNG_LOGE(...)
#endif // LOG_ERROR

#endif // PNG_LOGGER_H
