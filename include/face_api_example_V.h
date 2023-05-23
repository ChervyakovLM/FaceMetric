#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "face_api_test_V.h"

namespace FACEAPITEST_V {
    class FaceApiExampleV : public FACEAPITEST::Interface {
public:

    FaceApiExampleV();
    ~FaceApiExampleV() override;

    FACEAPITEST::ReturnStatus
    initialize(const std::string &configDir) override;

    FACEAPITEST::ReturnStatus
    createTemplate(
            const FACEAPITEST::Multiface &faces,
            FACEAPITEST::TemplateRole role,
            std::vector<uint8_t> &templ,
            std::vector<FACEAPITEST::EyePair> &eyeCoordinates,
            std::vector<double> &quality) override;

    FACEAPITEST::ReturnStatus
    matchTemplates(
            const std::vector<uint8_t> &verifTemplate,
            const std::vector<uint8_t> &initTemplate,
            double &similarity) override;

    FACEAPITEST::ReturnStatus
    train(
        const std::string &configDir,
        const std::string &trainedConfigDir) override;

    static std::shared_ptr<FACEAPITEST::Interface>
    getImplementation();

private:
    std::string configDir;
    static const int featureVectorSize{4};
    cv::Ptr<cv::FaceDetectorYN> detector_;
    cv::Ptr<cv::FaceRecognizerSF> face_recognizer_;
};
}
