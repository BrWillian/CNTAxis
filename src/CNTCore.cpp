//
// Created by willian on 10/5/25.
//

#include "../include/CNTCore.h"

float CNTCore::calculateIou(const cv::Rect& a, const cv::Rect& b) {
    int xA = std::max(a.x, b.x);
    int yA = std::max(a.y, b.y);
    int xB = std::min(a.x + a.width, b.x + b.width);
    int yB = std::min(a.y + a.height, b.y + b.height);

    int interWidth = std::max(0, xB - xA);
    int interHeight = std::max(0, yB - yA);
    float interArea = static_cast<float>(interWidth * interHeight);
    float areaA = static_cast<float>(a.width * a.height);
    float areaB = static_cast<float>(b.width * b.height);

    return interArea / (areaA + areaB - interArea + 1e-6f);
}

std::vector<Detection> CNTCore::performInference(const cv::Mat &image) const {
    const cv::Mat processedImage = ImageOperator::preprocess(image, input_size_);
    const std::vector<float> blobImage = ImageOperator::toBlob(processedImage);
    float* output_tensor = cnt_axis_->infer(blobImage);
    return cnt_axis_->postProcess(output_tensor);
}

CNTCore::CNTCore() {
    cnt_axis_ = new CNTAxis("./models/axis.onnx");
}

CNTCore::CNTCore(const std::string& model_path) {
    cnt_axis_ = new CNTAxis(model_path);
}

CNTCore::~CNTCore() {
    delete cnt_axis_;
}

std::vector<Vehicle> CNTCore::getAxis(const cv::Mat &frame) const {
    std::vector<Vehicle> vehicles = {};
    std::vector<Axis> axis = {};

    auto detections = this->performInference(frame);

    for (const auto& det : detections) {
        if (det.classId == 0 && det.confidence > 0.5f) {
            vehicles.push_back({det.bbox, det.confidence});
        } else if (det.classId == 1 && det.confidence > 0.25f) {
            axis.push_back({det.bbox, det.confidence});
        }
    }

    this->associateAxisToVehicle(vehicles, axis);

    return vehicles;
}

void CNTCore::associateAxisToVehicle(std::vector<Vehicle>& vehicles, const std::vector<Axis>& axis) {
    for (const auto& a : axis) {
        const cv::Rect& wb = a.bbox;
        cv::Point2f center(wb.x + wb.width / 2.0f, wb.y + wb.height / 2.0f);

        int best_vehicle = -1;
        float best_score = -1.0f;

        for (size_t i = 0; i < vehicles.size(); ++i) {
            const cv::Rect& vb = vehicles[i].bbox;

            if (vb.contains(center)) {
                if (center.y > (vb.y - 0.7 * vb.height)) {
                    float vehicle_area = static_cast<float>(vb.width * vb.height);
                    float iou_val = calculateIou(wb, vb);
                    float score = iou_val + 1.0f / (vehicle_area + 1e-6f);

                    if (score > best_score) {
                        best_score = score;
                        best_vehicle = static_cast<int>(i);
                    }
                }
            }
        }

        if (best_vehicle != -1) {
            vehicles[best_vehicle].axis.push_back(a);
        }
    }

    for (auto& v : vehicles) {
        v.numAxis = static_cast<int>(v.axis.size());
    }
}


