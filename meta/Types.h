//
// Created by willian on 10/2/25.
//

#ifndef UNTITLED_TYPES_H
#define UNTITLED_TYPES_H

#include <opencv2/opencv.hpp>

struct Detection {
    int classId;
    float confidence;
    cv::Rect bbox;
};

#endif //UNTITLED_TYPES_H