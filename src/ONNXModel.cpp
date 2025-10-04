//
// Created by willian on 10/4/25.
//

#include "../include/ONNXModel.h"

ONNXModel::ONNXModel(const std::string& model_path)
    : session_(nullptr), env_(ORT_LOGGING_LEVEL_WARNING, "OnnxRuntime"), memory_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {

    Ort::SessionOptions session_options;
    session_options.SetGraphOptimizationLevel(ORT_ENABLE_EXTENDED);
    session_options.SetLogSeverityLevel(3);

    session_ = Ort::Session(env_, model_path.c_str(), session_options);

    initializeIO();
}

ONNXModel::ONNXModel(const unsigned char model_weights[], const unsigned int model_weights_size): session_(nullptr), env_(ORT_LOGGING_LEVEL_WARNING, "OnnxRuntime"), memory_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)) {
    Ort::SessionOptions session_options;
    session_options.SetGraphOptimizationLevel(ORT_ENABLE_EXTENDED);
    session_options.SetLogSeverityLevel(3);

    session_ = Ort::Session(env_, model_weights, model_weights_size, session_options);

    initializeIO();
}

ONNXModel::~ONNXModel() = default;

float* ONNXModel::infer(const std::vector<float> &input_data) {
    if (input_data.empty()) {
        throw std::runtime_error("Input data size does not match the model's expected input size.");
    }

    Ort::Value input_tensor = createTensor(input_data, input_shape_);

    auto output_tensor = session_.Run(Ort::RunOptions{nullptr}, input_node_names_.data(),
                                       &input_tensor, 1, output_node_names_.data(), 1);

    float* output_data = output_tensor.front().GetTensorMutableData<float>();

    cv::Mat rawData = cv::Mat(static_cast<int>(output_shape_[1]), static_cast<int>(output_shape_[2]), CV_32F, output_data).t();

    return reinterpret_cast<float *>(rawData.data);
}

std::vector<Detection> ONNXModel::postProcess(float *input_data) const {
    std::vector<int> class_ids{};
    std::vector<float> confidences{};
    std::vector<cv::Rect> bboxes{};

    for (size_t i = 0; i < this->output_shape_[2]; ++i) {
        float* classesScores = input_data + 4;
        cv::Mat scores(1, this->classes.size(), CV_32FC1, classesScores);
        cv::Point class_id;
        double maxClassScore;
        cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

        if (maxClassScore > this->confThreshold) {
            confidences.push_back(maxClassScore);
            class_ids.push_back(class_id.x);
            const float x = input_data[0];
            const float y = input_data[1];
            const float w = input_data[2];
            const float h = input_data[3];

            int top, left;

            if (ImageOperator::input_size.width >= ImageOperator::input_size.height) {
                top = static_cast<int>((y - 0.5 * h) * ImageOperator::resize_scales - ImageOperator::padding * ImageOperator::resize_scales);
                left = static_cast<int>((x - 0.5 * w) * ImageOperator::resize_scales);
            }else {
                left = static_cast<int>((x - 0.5 * w) * ImageOperator::resize_scales - ImageOperator::padding * ImageOperator::resize_scales);
                top = static_cast<int>((y - 0.5 * h) * ImageOperator::resize_scales);
            }

            const int width = static_cast<int>(w * ImageOperator::resize_scales);
            const int height = static_cast<int>(h * ImageOperator::resize_scales);

            bboxes.emplace_back(left, top, width, height);
        }

        input_data += this->output_shape_[1];
    }
    std::vector<int> nmsResult;
    std::vector<Detection> oResult;
    cv::dnn::NMSBoxes(bboxes, confidences, this->confThreshold, this->iouThreshold, nmsResult);
    for (const int idx : nmsResult)
    {
        Detection result;
        result.classId = class_ids[idx];
        result.confidence = confidences[idx];
        result.bbox = bboxes[idx];
        oResult.push_back(result);
    }

    return oResult;
}

void ONNXModel::WarmUpSession() {
    try {
        std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

        std::vector<float> input_data(input_shape_[1] * input_shape_[2] * input_shape_[3], 1.0f);

        auto output = this->infer(input_data);

        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

        std::cout<<"Model warmup time: "<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<"ms"<<std::endl;

    }catch (const std::exception &e) {
        std::cerr<<e.what()<<std::endl;
    }
}

void ONNXModel::initializeIO() {
    // Get Input Size
    size_t num_input_nodes = session_.GetInputCount();
    Ort::AllocatorWithDefaultOptions allocator;
    for (size_t i = 0; i < num_input_nodes; i++) {
        Ort::AllocatedStringPtr input_node_name = session_.GetInputNameAllocated(i, allocator);
        input_node_names_.emplace_back(input_node_name.release());
        auto type_info = session_.GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        input_shape_ = tensor_info.GetShape();
    }

    // Get Output Size
    size_t num_output_nodes = session_.GetOutputCount();
    for (size_t i = 0; i < num_output_nodes; i++) {
        Ort::AllocatedStringPtr output_node_name = session_.GetOutputNameAllocated(i, allocator);
        output_node_names_.emplace_back(output_node_name.release());
        auto type_info = session_.GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();
        output_shape_ = tensor_info.GetShape();
    }

    WarmUpSession();
}

Ort::Value ONNXModel::createTensor(const std::vector<float> &data, const std::vector<int64_t> &dimensions) const {
    return Ort::Value::CreateTensor<float>(memory_info_, const_cast<float*>(data.data()), data.size(), dimensions.data(), dimensions.size());
}