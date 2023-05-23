#pragma once

#include <map>
#include "face_api_test_I.h"

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

namespace FACEAPITEST_I {
    class FaceApiExampleI : public FACEAPITEST::IdentInterface {
public:

    FaceApiExampleI();
    ~FaceApiExampleI() override;

    FACEAPITEST::ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir,
        FACEAPITEST::TemplateRole role) override;

    FACEAPITEST::ReturnStatus
    createTemplate(
        const FACEAPITEST::Multiface &faces,
        FACEAPITEST::TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<FACEAPITEST::EyePair> &eyeCoordinates) override;

    FACEAPITEST::ReturnStatus
    finalizeInit(
        const std::string &configDir,
        const std::string &initDir,
        const std::string &edbName,
        const std::string &edbManifestName) override;

    FACEAPITEST::ReturnStatus
    initializeIdentification(
        const std::string &configDir,
        const std::string &initDir) override;

    FACEAPITEST::ReturnStatus
    identifyTemplate(
        const std::vector<uint8_t> &idTemplate,
        const uint32_t candidateListLength,
        std::vector<FACEAPITEST::Candidate> &candidateList,
        bool &decision) override;

    FACEAPITEST::ReturnStatus
    galleryInsertID(
        const std::vector<uint8_t> &templ,
        const std::string &id) override;

    FACEAPITEST::ReturnStatus
    galleryDeleteID(
        const std::string &id) override;

    static std::shared_ptr<FACEAPITEST::IdentInterface>
    getImplementation();

private:
    std::map<std::string, std::vector<uint8_t>> templates;
    cv::Ptr<cv::FaceDetectorYN> detector_;
    cv::Ptr<cv::FaceRecognizerSF> face_recognizer_;

    const std::string edb{"mei.edb"};
    const std::string manifest{"mei.manifest"};
};
}
