/**
 * To enable various logging levels, either use #define PNG_LOG_ERROR, #define PNG_LOG_WARNING, or #define PNG_LOG_INFO
 */
#ifndef PNG_LOGGER_H
#define PNG_LOGGER_H

#include <stdio.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if defined(PNG_LOG_ERROR) || defined(PNG_LOG_WARNING) || defined(PNG_LOG_INFO) 
#define PNG_LOGE(fmt, ...) printf("[ERROR] %s:%d: " fmt, __FILENAME__, __LINE__ __VA_ARGS__)
#if defined(PNG_LOG_WARNING) || defined(PNG_LOG_INFO) 
#define PNG_LOGW(fmt, ...) printf("[WARN ] %s:%d: " fmt, __FILENAME__, __LINE__ __VA_ARGS__)
#if defined(PNG_LOG_INFO) 
#define PNG_LOGI(fmt, ...) printf("[INFO ] %s:%d: " fmt, __FILENAME__, __LINE__ __VA_ARGS__)
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
