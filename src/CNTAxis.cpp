#include "../include/CNTAxis.h"

CNTAxis::CNTAxis(const std::string &model_path) : ONNXModel(model_path){
    this->classes = {"vehicle", "axis"};
}
CNTAxis::CNTAxis(const unsigned char model_weights[], const unsigned int model_weights_size) : ONNXModel(model_weights, model_weights_size){
    this->classes = {"vehicle", "axis"};
}
CNTAxis::~CNTAxis() = default;

void CNTAxis::setClasses(const std::vector<std::string> &classes) {
    this->classes = classes;
}
std::vector<std::string> CNTAxis::getClasses() const {
    return this->classes;
}