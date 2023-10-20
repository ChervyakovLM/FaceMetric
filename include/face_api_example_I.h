#pragma once

#include <map>
#include "face_api_test_I.h"
#include "in_out_I.h"

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

namespace FACEAPITEST_I {
    class FaceApiExampleI : public FACEAPITEST::IdentInterface {
public:

    FaceApiExampleI();
    ~FaceApiExampleI() override;

    /*!
     * \brief Initializes the face template creation process.
     *
     * \param configDir The directory where the ONNX model files and other configuration files are located.
     * \param role The template role to be used during template creation.
     *
     * \return A ReturnStatus object indicating the success or failure of the initialization process.
     */
    FACEAPITEST::ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir,
        FACEAPITEST::TemplateRole role) override;

    /*!
     * \brief Creates a face template from the input images using the face detector and face recognizer.
     *
     * \param images A vector of Multiface objects containing the input face images.
     * \param role The template role to be used during template creation.
     * \param templ A vector to store the extracted facial features (face template).
     * \param eyeCoordinates A vector to store the eye coordinates of the detected face.
     *
     * \return A ReturnStatus object indicating the success or failure of the template creation process.
     */
    FACEAPITEST::ReturnStatus
    createTemplate(
        const FACEAPITEST::Multiface &faces,
        FACEAPITEST::TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<FACEAPITEST::EyePair> &eyeCoordinates) override;

    /*!
     * \brief Finalizes the initialization of the face recognition system.
     *
     * \param configDir The path to the configuration directory containing model files for face detection and recognition.
     * \param initDir The path to the initialization directory.
     * \param edbName The path to the enrolled database (edb) file.
     * \param edbManifestName The path to the edb manifest file.
     * \param outDir The path to write files containing templates.
     *
     * \return A ReturnStatus object indicating the success or failure of the initialization process.
     */
    FACEAPITEST::ReturnStatus
    finalizeInit(
        const std::string &configDir,
        const std::string &initDir,
        const std::string &edbName,
        const std::string &edbManifestName,
        const std::string &outDir) override;

    /*!
     * \brief Initializes the face identification system.
     *
     * \param configDir The path to the configuration directory containing model files for face detection and recognition.
     * \param initDir The path to the initialization directory.
     * \param outDir The path to read files containing templates.
     *
     * \return A ReturnStatus object indicating the success or failure of the initialization process.
     */
    FACEAPITEST::ReturnStatus
    initializeIdentification(
        const std::string &configDir,
        const std::string &initDir,
        const std::string &outDir) override;

    /*!
     * \brief Identifies the template against the gallery of enrolled templates.
     *
     * \param idTemplate The input template to be identified.
     * \param candidateListLength The number of top candidates to be returned in the candidateList.
     * \param candidateList A vector of Candidate objects representing the top candidates with their similarity scores.
     * \param decision A boolean variable indicating the success of the identification process.
     *
     * \return A ReturnStatus object indicating the success or failure of the identification process.
     */
    FACEAPITEST::ReturnStatus
    identifyTemplate(
        const std::vector<uint8_t> &idTemplate,
        const uint32_t candidateListLength,
        std::vector<FACEAPITEST::Candidate> &candidateList,
        bool &decision) override;

    /*!
     * \brief Inserts a template into the gallery with the specified ID.
     *
     * \param templ The template data to be inserted into the gallery.
     * \param id The ID associated with the template in the gallery.
     *
     * \return A ReturnStatus object indicating the success or failure of the insertion process.
     */
    FACEAPITEST::ReturnStatus
    galleryInsertID(
        const std::vector<uint8_t> &templ,
        const std::string &id) override;

    /*!
     * \brief Deletes a template from the gallery with the specified ID.
     *
     * \param id The ID of the template to be deleted from the gallery.
     *
     * \return A ReturnStatus object indicating the success or failure of the deletion process.
     */
    FACEAPITEST::ReturnStatus
    galleryDeleteID(
        const std::string &id) override;

    /*!
     * \brief Returns a shared pointer to the implementation of the IdentInterface.
     *
     * \return A shared pointer to the implementation of the IdentInterface.
     */
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
