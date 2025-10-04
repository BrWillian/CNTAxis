//
// Created by willian on 10/4/25.
//

#ifndef UNTITLED_IMAGEOP_H
#define UNTITLED_IMAGEOP_H

#include <opencv2/opencv.hpp>

class ImageOperator {
public:
    static cv::Mat preprocess(const cv::Mat& inputImage, const cv::Size& targetSize);
    static std::vector<float> toBlob(const cv::Mat& inputImage);

    static inline float resize_scales;
    static inline int padding;


    static inline cv::Size target_size;
    static inline cv::Size input_size;
};


#endif //UNTITLED_IMAGEOP_H