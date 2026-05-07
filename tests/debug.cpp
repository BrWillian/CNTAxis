//
// Created by willian on 10/12/25.
//
#include "../include/CNTCore.h"
#include <iostream>

int main(int argc, char *argv []) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <image.jpg>" << std::endl;
        return 1;
    }

    CNTCore *cntd = new CNTCore("./model.onnx");

    std::cout<<"Model loaded"<<std::endl;

    cv::Mat inputImage = cv::imread(argv[1]);

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    auto results = cntd->getAxis(inputImage);

    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

    for (auto &res: results) {
        std::cout<<"Veiculo"<<res.bbox<<std::endl;
        std::cout<<"N de eixos: "<<res.numAxis<<std::endl;

        cv::rectangle(inputImage, res.bbox, cv::Scalar(0, 0, 255), 1);
        cv::putText(inputImage, "Num eixos: "+ std::to_string(res.numAxis), cv::Size(res.bbox.x + 5, res.bbox.y +20), cv::FONT_HERSHEY_SIMPLEX,
            0.5, cv::Scalar(255, 0, 0), 1);

        for (auto &axis: res.axis) {
            cv::rectangle(inputImage, axis.bbox, cv::Scalar(0, 0, 255), 1);
            cv::putText(inputImage, "Eixo", cv::Size(axis.bbox.x + 5, axis.bbox.y +20), cv::FONT_HERSHEY_SIMPLEX,
                0.5, cv::Scalar(0, 255, 0), 1);
        }

        cv::imshow("Window", inputImage);
        cv::waitKey(0);

        std::cout<<"Tempo: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()<<"ms"<<std::endl;
    }
    delete cntd;
}