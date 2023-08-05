#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "face_api_test_structs.h"

namespace FACEAPITEST {
class IdentInterface {
public:
    virtual ~IdentInterface() {}

    // initialization of the algorithm for calculating biometric templates
    virtual ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir,
        TemplateRole role) = 0;

    // template calculation
    virtual ReturnStatus
    createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates) = 0;

    // create an index from all templates
    virtual ReturnStatus
    finalizeInit(
    	const std::string &configDir,
        const std::string &initDir,
        const std::string &edbName,
        const std::string &edbManifestName) = 0;

    // initialization of the index search algorithm
    virtual ReturnStatus
    initializeIdentification(
        const std::string &configDir,
        const std::string &initDir) = 0;

    // search by index
    virtual ReturnStatus
    identifyTemplate(
        const std::vector<uint8_t> &idTemplate,
        const uint32_t candidateListLength,
        std::vector<Candidate> &candidateList,
        bool &decision) = 0;

    // adding a template to the index
    virtual ReturnStatus
    galleryInsertID(
        const std::vector<uint8_t> &templ,
        const std::string &id) = 0;

    // remove the template from the index
    virtual ReturnStatus
    galleryDeleteID(
        const std::string &id) = 0;

    // get a pointer to the implementation
    static std::shared_ptr<IdentInterface>
    getImplementation();
};
}
