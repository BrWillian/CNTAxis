//
// Created by willian on 10/13/25.
//

#ifndef CNTAXIS_C_WRAPPER_H
#define CNTAXIS_C_WRAPPER_H

#ifdef __cplusplus
#include <string>
#include "../include/CNTCore.h"
#include <opencv2/opencv.hpp>
#endif

#if defined(__GNUC__)
//  GCC
#define CNTAXIS_API __attribute__((visibility("default")))
#define IMPORT
#define CDECL __attribute__((cdecl))
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#define CDECL
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
    struct AxisDetect {
        CNTCore* cntCore;
        std::vector<Vehicle>* vehicles;
        cv::Mat* image;
        std::stringstream* ss;
    };
#else
    struct AxisDetect {
        void* cntCore;
        void* vehicles;
        void* image;
        void* ss;
    };
#endif

    typedef struct AxisDetect cntaxis_t;

    CNTAXIS_API cntaxis_t* C_CNTAXISCREATE();

    CNTAXIS_API void C_CNTAXISDELETE(cntaxis_t* anpr);

    CNTAXIS_API const char* C_CNTAXISINFERENCE(const cntaxis_t* cntaxis, unsigned char* imgData, int imgSize);

    CNTAXIS_API const char* C_CNTAXISVERSION();

#ifdef __cplusplus

    CNTAXIS_API std::string CPP_CNTAXISINFERENCE(cntaxis_t* cntaxis, cv::Mat& img);

    CNTAXIS_API std::string CPP_CNTAXISVERSION();

#endif

#ifdef __cplusplus
}
#endif

#endif //CNTAXIS_C_WRAPPER_H