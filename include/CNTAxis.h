#ifndef CNTAXIS_LIBRARY_H
#define CNTAXIS_LIBRARY_H

#include <onnxruntime/onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include "../meta/Types.h"
#include <vector>
#include "ONNXModel.h"

class CNTAxis final : public ONNXModel {
public:
    explicit CNTAxis(const std::string &model_path);
    explicit CNTAxis(const unsigned char model_weights[], const unsigned int model_weights_size);

    ~CNTAxis() override;

    void setClasses(const std::vector<std::string> &classes);
    std::vector<std::string> getClasses() const;
};

#endif // CNTAXIS_LIBRARY_H