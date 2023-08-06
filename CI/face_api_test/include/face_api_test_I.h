#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "face_api_test_structs.h"

namespace FACEAPITEST {
class IdentInterface {
public:
    virtual ~IdentInterface() {}

    /*!
     * \brief Initialize the template creation process.
     *
     * \param configDir The directory containing the configuration files needed for the template creation process.
     * \param role The role of the template to be created (e.g., TemplateRole::Init_I, TemplateRole::Identification, etc.).
     *
     * \return The return status of the initialization process (e.g., success or failure).
     */
    virtual ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir,
        TemplateRole role) = 0;

    /*!
     * \brief Create a template from multiple face images.
     *
     * \param faces The multiple face images from which to create the template.
     * \param role The role of the template to be created (e.g., TemplateRole::Init_I, TemplateRole::Identification, etc.).
     * \param templ The output vector that will store the created template data.
     * \param eyeCoordinates The output vector that will store the eye coordinates for each face image in the template (if applicable).
     *
     * \return The return status of the template creation process (e.g., success or failure).
     */
    virtual ReturnStatus
    createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates) = 0;

    /*!
     * \brief Finalize the initialization of the face recognition API.
     *
     * \param configDir The directory path where the configuration files are located.
     * \param initDir The directory path where the initialization files are located.
     * \param edbName The name of the embedded database file.
     * \param edbManifestName The name of the manifest file for the embedded database.
     *
     * \return The return status of the finalization process (e.g., success or failure).
     */
    virtual ReturnStatus
    finalizeInit(
    	const std::string &configDir,
        const std::string &initDir,
        const std::string &edbName,
        const std::string &edbManifestName) = 0;

    /*!
     * \brief Initialize the face identification process.
     *
     * \param configDir The directory path where the configuration files are located.
     * \param initDir The directory path where the initialization files are located.
     *
     * \return The return status of the initialization process (e.g., success or failure).
     */
    virtual ReturnStatus
    initializeIdentification(
        const std::string &configDir,
        const std::string &initDir) = 0;

    /*!
     * \brief Identify a face template against a gallery of templates.
     *
     * \param idTemplate The input face template to be identified.
     * \param candidateListLength The maximum number of candidates to be returned in the candidate list.
     * \param candidateList A vector that will be filled with potential candidate matches.
     * \param decision A boolean flag that will be set to true if a match is found, otherwise false.
     *
     * \return The return status of the identification process (e.g., success or failure).
     */
    virtual ReturnStatus
    identifyTemplate(
        const std::vector<uint8_t> &idTemplate,
        const uint32_t candidateListLength,
        std::vector<Candidate> &candidateList,
        bool &decision) = 0;

    /*!
     * \brief Insert a face template into the gallery with an associated ID.
     *
     * \param templ The face template data to be inserted into the gallery.
     * \param id The unique ID associated with the inserted template.
     *
     * \return The return status of the insertion process (e.g., success or failure).
     */
    virtual ReturnStatus
    galleryInsertID(
        const std::vector<uint8_t> &templ,
        const std::string &id) = 0;

    /*!
     * \brief Delete a face template from the gallery using its associated ID.
     *
     * \param id The unique ID of the face template to be deleted from the gallery.
     *
     * \return The return status of the deletion process (e.g., success or failure).
     */
    virtual ReturnStatus
    galleryDeleteID(
        const std::string &id) = 0;

    /*!
     * \brief Get the implementation of the IdentInterface.
     *
     * \return A shared pointer to the implementation of the IdentInterface.
     */
    static std::shared_ptr<IdentInterface>
    getImplementation();
};
}
