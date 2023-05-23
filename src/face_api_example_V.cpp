#include <cstring>
#include <cstdlib>

#include "face_api_example_V.h"

using namespace std;
using namespace FACEAPITEST;
using namespace FACEAPITEST_V;
using namespace cv;

FaceApiExampleV::FaceApiExampleV() {}

FaceApiExampleV::~FaceApiExampleV() {}

ReturnStatus
FaceApiExampleV::initialize(const std::string &configDir)
{
    detector_ = FaceDetectorYN::create(configDir + "/face_detection_yunet_2022mar.onnx", "", {320, 320});
    face_recognizer_ = FaceRecognizerSF::create(configDir + "/face_recognition_sface_2021dec.onnx", "");

    return ReturnStatus(ReturnCode::Success);
}

ReturnStatus
FaceApiExampleV::createTemplate(
        const Multiface &images,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates,
        std::vector<double> &quality)
{
    assert(images.size() >= 1);
    auto face = images[0];
    assert(face.depth == 24);

    Mat image(face.height, face.width, CV_8UC3, face.data.get());
    Mat faces, aligned_face, feature;

    cvtColor(image, image, COLOR_RGB2BGR);
    resize(image, image, {320,320});

    detector_->detect(image, faces);
    if (faces.rows >= 1)
    {
        //find max s
        size_t max_i = 0;
        float max_s = 0;
        for (size_t i = 0; i < faces.rows; i++)
        {
            float cur_s = faces.at<float>(i,2) * faces.at<float>(i,3);
            if (max_s < cur_s)
            {
                max_s = cur_s;
                max_i = i;
            }
        }

        face_recognizer_->alignCrop(image, faces.row(max_i), aligned_face);
        face_recognizer_->feature(aligned_face, feature);

        templ.resize(512);
        memcpy(templ.data(), feature.row(0).data, 512);

        return ReturnStatus(ReturnCode::Success);
    }

    return ReturnStatus(ReturnCode::RefuseInput);
}

ReturnStatus
FaceApiExampleV::matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &enrollTemplate,
        double &similarity)
{
    uint8_t* f = const_cast<uint8_t*>(verifTemplate.data());
    uint8_t* e = const_cast<uint8_t*>(enrollTemplate.data());

    Mat verif(1, 128, CV_32F, reinterpret_cast<float*>(f));
    Mat enroll(1, 128, CV_32F, reinterpret_cast<float*>(e));
    similarity = face_recognizer_->match(verif, enroll, FaceRecognizerSF::DisType::FR_COSINE);

    return ReturnStatus(ReturnCode::Success);
}

FACEAPITEST::ReturnStatus
FaceApiExampleV::train(
    const std::string &configDir,
    const std::string &trainedConfigDir)
{
    return ReturnStatus(ReturnCode::Success);
}

std::shared_ptr<Interface>
Interface::getImplementation()
{
    return std::make_shared<FaceApiExampleV>();
}






