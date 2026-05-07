//
// Created by willian on 12/23/24.
//


#include "../meta/C_Wrapper.h"
#include "../generated/Version.h"

cntaxis_t *C_CNTAXISCREATE() {
    cntaxis_t* objwrapper;

    auto cntcore = new CNTCore();
    auto imageContainer = new cv::Mat();
    auto vehiclesContainer = new std::vector<Vehicle>();
    auto strResult = new std::stringstream;

    objwrapper = (__typeof__(objwrapper)) malloc(sizeof(*objwrapper));

    objwrapper->cntCore = cntcore;
    objwrapper->image = imageContainer;
    objwrapper->vehicles = vehiclesContainer;
    objwrapper->ss = strResult;

    return objwrapper;
}
void CDECL C_CNTAXISDELETE(cntaxis_t* cntaxis){
    if(cntaxis == nullptr){
        std::cerr<<"[ERROR] Received invalid pointer"<<std::endl;
        return;
    }
    delete static_cast<CNTCore*>(cntaxis->cntCore);
    delete static_cast<std::vector<Vehicle>*>(cntaxis->vehicles);
    delete static_cast<cv::Mat*>(cntaxis->image);
    delete static_cast<std::stringstream*>(cntaxis->ss);
    free(cntaxis);
}
std::string Serialize(const cntaxis_t* cntaxis, const std::vector<Vehicle>& res) {
    if (cntaxis == nullptr || cntaxis->ss == nullptr) {
        return "[]";
    }
    cntaxis->ss->str("");
    *cntaxis->ss << std::boolalpha;
    *cntaxis->ss << "[";
    for (size_t i = 0; i < res.size(); ++i) {
        const auto&[bbox, confidence, numAxis, axis] = res[i];

        *cntaxis->ss << "{";
        *cntaxis->ss << "\"numAxis\":\"" << numAxis << "\",";
        *cntaxis->ss << "\"conf\":" << confidence << ",";
        *cntaxis->ss << "\"bbox\":{";
        *cntaxis->ss << "\"x\":" << bbox.x << ",";
        *cntaxis->ss << "\"y\":" << bbox.y << ",";
        *cntaxis->ss << "\"w\":" << bbox.width << ",";
        *cntaxis->ss << "\"h\":" << bbox.height;
        *cntaxis->ss << "},";
        *cntaxis->ss << "\"Axis\":[";

        for (size_t j = 0; j < axis.size(); ++j) {
            const auto&[bbox, confidence] = axis[j];
            *cntaxis->ss << "{";
            *cntaxis->ss << "\"conf\":\"" << confidence << "\",";
            *cntaxis->ss << "\"bbox\":{";
            *cntaxis->ss << "\"x\":" << bbox.x << ",";
            *cntaxis->ss << "\"y\":" << bbox.y << ",";
            *cntaxis->ss << "\"w\":" << bbox.width << ",";
            *cntaxis->ss << "\"h\":" << bbox.height;
            *cntaxis->ss << "}";
            *cntaxis->ss << "}";

            if (j != axis.size() - 1) {
                *cntaxis->ss << ",";
            }
        }

        *cntaxis->ss << "]";
        *cntaxis->ss << "}";

        if (i != res.size() - 1) {
            *cntaxis->ss << ",";
        }
    }
    *cntaxis->ss << "]";

    return cntaxis->ss->str();
}
const char* CDECL C_CNTAXISINFERENCE(const cntaxis_t* cntaxis, unsigned char* imgData, const int imgSize) {
    if (cntaxis == nullptr || cntaxis->cntCore == nullptr || cntaxis->image == nullptr || cntaxis->vehicles == nullptr) {
        std::cerr << "[ERROR] Received invalid pointer" << std::endl;
        return strdup("[]");
    }
    if (imgData == nullptr || imgSize <= 0) {
        std::cerr << "[ERROR] Received invalid image data" << std::endl;
        return strdup("[]");
    }

    const std::vector<uchar> data(imgData, imgData + imgSize);
    *cntaxis->image = cv::imdecode(cv::Mat(data), -1);

    if (cntaxis->image->empty()) {
        std::cerr << "[ERROR] Failed to decode image" << std::endl;
        return strdup("[]");
    }

    try {
        *cntaxis->vehicles =  cntaxis->cntCore->getAxis(*cntaxis->image);
    }catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return strdup("[]");
    }

    return strdup(Serialize(cntaxis, *cntaxis->vehicles).c_str());
}
const char* CDECL C_CNTAXISVERSION(){
    return CNTAXIS_VERSION "-" GIT_BRANCH "-" GIT_COMMIT_HASH;
}
// CPP FUNCTIONS
std::string CDECL CPP_CNTAXISINFERENCE(const cntaxis_t* cntaxis, cv::Mat& img) {
    if (cntaxis == nullptr || cntaxis->cntCore == nullptr || cntaxis->ss == nullptr) {
        std::cerr << "[ERROR] Received invalid pointer" << std::endl;
        return "[]";
    }

    if (img.empty()) {
        std::cerr << "[ERROR] Failed to read image" << std::endl;
        return "[]";
    }
    std::vector<Vehicle> vehicles;
    try {
        vehicles =  cntaxis->cntCore->getAxis(img);
    }catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return "[]";
    }

    return strdup(Serialize(cntaxis, vehicles).c_str());
}
std::string CDECL CPP_CNTAXISVERSION(){
    return CNTAXIS_VERSION "-" GIT_BRANCH "-" GIT_COMMIT_HASH;
}
