//
// Created by willian on 10/5/25.
//

#ifndef CNTAXIS_CNTCORE_H
#define CNTAXIS_CNTCORE_H
#include "../meta/Types.h"
#include "CNTAxis.h"
#include <regex>
#include <optional>


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

class CNTCore {
private:
    CNTAxis *cnt_axis_{};

    const cv::Size input_size_ = cv::Size(640, 640);

    const float vh_thr = 0.5;
    const float axis_thr = 0.25;

    CNTAXIS_API std::vector<Detection> performInference(const cv::Mat& image) const;
public:
    CNTAXIS_API CNTCore();
    CNTAXIS_API CNTCore(const std::string &model_path);
    CNTAXIS_API ~CNTCore();


    CNTAXIS_API std::vector<Vehicle> getAxis(const cv::Mat &frame) const;

    // Utils
    CNTAXIS_API static void associateAxisToVehicle(std::vector<Vehicle>& vehicles, const std::vector<Axis>& axis);

    CNTAXIS_API static float calculateIou(const cv::Rect& a, const cv::Rect& b);
};


#endif //CNTAXIS_CNTCORE_H