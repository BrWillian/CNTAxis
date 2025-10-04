#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H

#include <onnxruntime/onnxruntime_cxx_api.h>
#include <opencv4/opencv2/opencv.hpp>
#include "../meta/types.h"
#include <vector>



class CNTAxis {
public:
    explicit CNTAxis(const std::string& model_path);

    explicit CNTAxis(const unsigned char model_weights[], const unsigned int model_weights_size);

    virtual ~CNTAxis();

    float* infer(const std::vector<float>& input_data);

    std::vector<Detection> postProcess(float* input_data) const;

    virtual void WarmUpSession();

protected:
    Ort::Session session_;
    Ort::Env env_;
    Ort::MemoryInfo memory_info_;

    std::vector<int64_t> input_shape_;
    std::vector<int64_t> output_shape_;

    std::vector<const char*> input_node_names_;
    std::vector<const char*> output_node_names_;

    std::vector<std::string> classes {};

    float confThreshold = 0.1;
    float iouThreshold = 0.5;

    void initializeIO();

    Ort::Value createTensor(const std::vector<float>& data, const std::vector<int64_t>& dimensions) const;
};


#endif // UNTITLED_LIBRARY_H