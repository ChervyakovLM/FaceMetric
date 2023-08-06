#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "face_api_test_V.h"

namespace FACEAPITEST_V {
    class FaceApiExampleV : public FACEAPITEST::Interface {
public:

    FaceApiExampleV();
    ~FaceApiExampleV() override;

    /*!
     * \brief Initialize the face detection and face recognition models.
     *
     * \param configDir The path to the directory containing the ONNX model files for face detection and face recognition.
     *
     * \return A `ReturnStatus` object indicating the success or failure of the initialization.
     */
    FACEAPITEST::ReturnStatus
    initialize(const std::string &configDir) override;

    /*!
     * \brief Create a face template from input images using face detection and face recognition models.
     *
     * \param images The `Multiface` object containing the input images.
     * \param role The role of the template.
     * \param templ The output vector to store the created face template.
     * \param eyeCoordinates The output vector to store the eye coordinates of the detected face.
     * \param quality The output vector to store the quality of the detected face.
     *
     * \return A `ReturnStatus` object indicating the success or failure of the template creation.
     */
    FACEAPITEST::ReturnStatus
    createTemplate(
            const FACEAPITEST::Multiface &faces,
            FACEAPITEST::TemplateRole role,
            std::vector<uint8_t> &templ,
            std::vector<FACEAPITEST::EyePair> &eyeCoordinates,
            std::vector<double> &quality) override;

    /*!
     * \brief Match two face templates to calculate their similarity score.
     *
     * \param verifTemplate The vector containing the verification face template.
     * \param enrollTemplate The vector containing the enrollment face template.
     * \param similarity The output variable to store the similarity score between the two templates.
     *
     * \return A `ReturnStatus` object indicating the success or failure of the template matching.
     */
    FACEAPITEST::ReturnStatus
    matchTemplates(
            const std::vector<uint8_t> &verifTemplate,
            const std::vector<uint8_t> &initTemplate,
            double &similarity) override;

    /*!
     * \brief Train the face recognition model.
     *
     * \param configDir The directory containing the configuration files for the face recognition model.
     * \param trainedConfigDir The directory where the trained model configuration will be stored.
     *
     * \return A `ReturnStatus` object indicating the success of the training process.
     */
    FACEAPITEST::ReturnStatus
    train(
        const std::string &configDir,
        const std::string &trainedConfigDir) override;

    /*!
     * \brief Get the implementation of the face recognition API.
     *
     * \return A shared pointer to the implementation of the face recognition API.
     */
    static std::shared_ptr<FACEAPITEST::Interface>
    getImplementation();

private:
    std::string configDir;
    static const int featureVectorSize{4};
    cv::Ptr<cv::FaceDetectorYN> detector_;
    cv::Ptr<cv::FaceRecognizerSF> face_recognizer_;
};
}
