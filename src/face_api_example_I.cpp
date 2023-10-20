#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "face_api_example_I.h"

using namespace std;
using namespace FACEAPITEST;
using namespace FACEAPITEST_I;
using namespace cv;

FaceApiExampleI::FaceApiExampleI() {}

FaceApiExampleI::~FaceApiExampleI() {}

/*!
 * \brief initializeTemplateCreation - Initializes the face template creation process.
 *
 * \param configDir The directory where the ONNX model files and other configuration files are located.
 * \param role The template role to be used during template creation.
 *
 * \return A ReturnStatus object indicating the success or failure of the initialization process.
 */
ReturnStatus
FaceApiExampleI::initializeTemplateCreation(
    const std::string &configDir,
    TemplateRole role)
{
    detector_ = FaceDetectorYN::create(configDir + "/face_detection_yunet_2022mar.onnx", "", {320, 320});
    face_recognizer_ = FaceRecognizerSF::create(configDir + "/face_recognition_sface_2021dec.onnx", "");

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief createTemplate - Creates a face template from the input images using the face detector and face recognizer.
 *
 * \param images A vector of Multiface objects containing the input face images.
 * \param role The template role to be used during template creation.
 * \param templ A vector to store the extracted facial features (face template).
 * \param eyeCoordinates A vector to store the eye coordinates of the detected face.
 *
 * \return A ReturnStatus object indicating the success or failure of the template creation process.
 */
ReturnStatus
FaceApiExampleI::createTemplate(
    const Multiface &images,
    TemplateRole role,
    std::vector<uint8_t> &templ,
    std::vector<EyePair> &eyeCoordinates)
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

/*!
 * \brief finalizeInit - Finalizes the initialization of the face recognition system.
 *
 * \param configDir The path to the configuration directory containing model files for face detection and recognition.
 * \param initDir The path to the initialization directory.
 * \param edbName The path to the enrolled database (edb) file.
 * \param edbManifestName The path to the edb manifest file.
 * \param outDir The path to write files containing templates.
 *
 * \return A ReturnStatus object indicating the success or failure of the initialization process.
 */
ReturnStatus
FaceApiExampleI::finalizeInit(
    const std::string &configDir,
    const std::string &initDir,
    const std::string &edbName,
    const std::string &edbManifestName,
    const std::string &outDir)
{
    fstream file(edbManifestName);
    FILE* edb = fopen(edbName.c_str(), "rb");
    string id;
    int size, offset;
    size_t nread;

    while (!file.eof())
    {
        file >> id;
        file >> size;
        file >> offset;
        fseek(edb, offset, SEEK_SET);
        vector<uint8_t> buf(size);
        nread = fread(buf.data(), 1, size, edb);
        assert(nread == size);
        galleryInsertID(buf, id);
    }
    fclose(edb);

    WriteFiles(outDir, this->templates);

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief initializeIdentification - Initializes the face identification system.
 *
 * \param configDir The path to the configuration directory containing model files for face detection and recognition.
 * \param initDir The path to the initialization directory.
 * \param outDir The path to read files containing templates.
 *
 * \return A ReturnStatus object indicating the success or failure of the initialization process.
 */
ReturnStatus
FaceApiExampleI::initializeIdentification(
    const std::string &configDir,
    const std::string &initDir,
    const std::string &outDir)
{
    if (!detector_)
        detector_ = FaceDetectorYN::create(configDir + "/face_detection_yunet_2022mar.onnx", "", {320, 320});
    if (!face_recognizer_)
        face_recognizer_ = FaceRecognizerSF::create(configDir + "/face_recognition_sface_2021dec.onnx", "");

    ReadTextBinaryFromFiles(outDir, templates);

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief identifyTemplate - Identifies the template against the gallery of enrolled templates.
 *
 * \param idTemplate The input template to be identified.
 * \param candidateListLength The number of top candidates to be returned in the candidateList.
 * \param candidateList A vector of Candidate objects representing the top candidates with their similarity scores.
 * \param decision A boolean variable indicating the success of the identification process.
 *
 * \return A ReturnStatus object indicating the success or failure of the identification process.
 */
ReturnStatus
FaceApiExampleI::identifyTemplate(
    const std::vector<uint8_t> &idTemplate,
    const uint32_t candidateListLength,
    std::vector<Candidate> &candidateList,
    bool &decision)
{
    map<float, Candidate> hash;
    uint8_t* f = const_cast<uint8_t*>(idTemplate.data());
    Mat verif(1, 128, CV_32F, reinterpret_cast<float*>(f));

    for (const auto& t : templates)
    {
        uint8_t* e = const_cast<uint8_t*>(t.second.data());
        Mat enroll(1, 128, CV_32F, reinterpret_cast<float*>(e));
        double similarity = face_recognizer_->match(verif, enroll, FaceRecognizerSF::DisType::FR_COSINE);
        hash[similarity] = Candidate(true, t.first, similarity);
    }

    for (auto it = hash.rbegin(); it != hash.rend(); ++it)
    {
        candidateList.push_back(it->second);
        if (candidateList.size() == candidateListLength)
            break;
    }

    decision = true;

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief galleryInsertID - Inserts a template into the gallery with the specified ID.
 *
 * \param templ The template data to be inserted into the gallery.
 * \param id The ID associated with the template in the gallery.
 *
 * \return A ReturnStatus object indicating the success or failure of the insertion process.
 */
ReturnStatus
FaceApiExampleI::galleryInsertID(
    const std::vector<uint8_t> &templ,
    const std::string &id)
{
    this->templates.insert(std::make_pair(id, templ));

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief galleryDeleteID - Deletes a template from the gallery with the specified ID.
 *
 * \param id The ID of the template to be deleted from the gallery.
 *
 * \return A ReturnStatus object indicating the success or failure of the deletion process.
 */
ReturnStatus
FaceApiExampleI::galleryDeleteID(
    const std::string &id)
{
    this->templates.erase(id);

    return ReturnStatus(ReturnCode::Success);
}

/*!
 * \brief getImplementation - Returns a shared pointer to the implementation of the IdentInterface.
 *
 * \return A shared pointer to the implementation of the IdentInterface.
 */
std::shared_ptr<IdentInterface>
IdentInterface::getImplementation()
{
    return make_shared<FaceApiExampleI>();
}

