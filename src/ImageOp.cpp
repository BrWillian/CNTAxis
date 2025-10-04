//
// Created by willian on 10/4/25.
//

#include "../include/ImageOp.h"

cv::Mat ImageOperator::preprocess(const cv::Mat& inputImage, const cv::Size& targetSize) {
    cv::Mat oImg = inputImage.clone();
    cv::cvtColor(oImg, oImg, cv::COLOR_BGR2RGB);
    cv::Mat outputImage(targetSize, CV_8UC3, cv::Scalar(0, 0, 0));
    input_size = inputImage.size();

    if (inputImage.cols >= inputImage.rows) {
        resize_scales = static_cast<float>(inputImage.cols) / targetSize.height;
        int newHeight = static_cast<int>(inputImage.rows / resize_scales);
        cv::resize(oImg, oImg, cv::Size(targetSize.height, newHeight));
        padding = (targetSize.height - newHeight) / 2;
        oImg.copyTo(outputImage(cv::Rect(0, padding, oImg.cols, oImg.rows)));
    } else {
        resize_scales = static_cast<float>(inputImage.rows) / targetSize.height;
        int newWidth = static_cast<int>(inputImage.cols / resize_scales);
        cv::resize(oImg, oImg, cv::Size(newWidth, targetSize.width));
        padding = (targetSize.width - newWidth) / 2;
        oImg.copyTo(outputImage(cv::Rect(padding, 0, oImg.cols, oImg.rows)));
    }

    return outputImage;
}
std::vector<float> ImageOperator::toBlob(const cv::Mat& inputImage){
    cv::Mat floatImage;
    inputImage.convertTo(floatImage, CV_32F, 1.0 / 255.0);

    std::vector<cv::Mat> channels(3);
    cv::split(floatImage, channels);

    std::vector<float> blob;
    for (const auto& channel : channels) {
        blob.insert(blob.end(), (float*)channel.datastart, (float*)channel.dataend);
    }

    return blob;
}