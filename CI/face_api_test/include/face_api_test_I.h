#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "face_api_test_structs.h"

namespace FACEAPITEST {
class IdentInterface {
public:
    virtual ~IdentInterface() {}

    virtual ReturnStatus
    initializeTemplateCreation(
        const std::string &configDir,
        TemplateRole role) = 0;

    virtual ReturnStatus
    createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates) = 0;

    virtual ReturnStatus
    finalizeInit(
    	const std::string &configDir,
        const std::string &initDir,
        const std::string &edbName,
        const std::string &edbManifestName) = 0;

    virtual ReturnStatus
    initializeIdentification(
        const std::string &configDir,
        const std::string &initDir) = 0;

    virtual ReturnStatus
    identifyTemplate(
        const std::vector<uint8_t> &idTemplate,
        const uint32_t candidateListLength,
        std::vector<Candidate> &candidateList,
        bool &decision) = 0;

    virtual ReturnStatus
    galleryInsertID(
        const std::vector<uint8_t> &templ,
        const std::string &id) = 0;

    virtual ReturnStatus
    galleryDeleteID(
        const std::string &id) = 0;

    static std::shared_ptr<IdentInterface>
    getImplementation();
};
}
