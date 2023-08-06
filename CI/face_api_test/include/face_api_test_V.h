#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace FACEAPITEST {
typedef struct Image {
    uint16_t width;
    uint16_t height;
    uint8_t depth;
    std::shared_ptr<uint8_t> data;

    Image() :
        width{0},
        height{0},
        depth{24}
        {}

    Image(
        uint16_t width,
        uint16_t height,
        uint8_t depth,
        const std::shared_ptr<uint8_t> &data
        ) :
        width{width},
        height{height},
        depth{depth},
        data{data}
        {}

    size_t
    size() const { return (width * height * (depth / 8)); }
} Image;

using Multiface = std::vector<Image>;

enum class TemplateRole {
    Init_V,
    Verification
};

enum class ReturnCode {
    Success = 0,
    ConfigError,
    RefuseInput,
    ExtractError,
    ParseError,
    TemplateCreationError,
    VerifTemplateError,
    NumDataError,
    TemplateFormatError,
    NotImplemented,
    VendorError
};

inline std::ostream&
operator<<(
    std::ostream &s,
    const ReturnCode &rc)
{
    switch (rc) {
    case ReturnCode::Success:
        return (s << "Success");
    case ReturnCode::ConfigError:
        return (s << "Error reading configuration files");
    case ReturnCode::RefuseInput:
        return (s << "Elective refusal to process the input");
    case ReturnCode::ExtractError:
        return (s << "Involuntary failure to process the image");
    case ReturnCode::ParseError:
        return (s << "Cannot parse the input data");
    case ReturnCode::TemplateCreationError:
        return (s << "Elective refusal to produce a template");
    case ReturnCode::VerifTemplateError:
        return (s << "Either/both input templates were result of failed feature extraction");
    case ReturnCode::NumDataError:
        return (s << "Number of input images not supported");
    case ReturnCode::TemplateFormatError:
        return (s << "Template file is an incorrect format or defective");
    case ReturnCode::NotImplemented:
        return (s << "Function is not implemented");
    case ReturnCode::VendorError:
        return (s << "Vendor-defined error");
    default:
        return (s << "Undefined error");
    }
}

typedef struct ReturnStatus {
    ReturnCode code;
    std::string info;

    ReturnStatus() {}

    ReturnStatus(
        const ReturnCode code,
        const std::string &info = ""
        ) :
        code{code},
        info{info}
        {}
} ReturnStatus;

typedef struct EyePair
{
    bool isLeftAssigned;
    bool isRightAssigned;
    uint16_t xleft;
    uint16_t yleft;
    uint16_t xright;
    uint16_t yright;

    EyePair() :
        isLeftAssigned{false},
        isRightAssigned{false},
        xleft{0},
        yleft{0},
        xright{0},
        yright{0}
        {}

    EyePair(
        bool isLeftAssigned,
        bool isRightAssigned,
        uint16_t xleft,
        uint16_t yleft,
        uint16_t xright,
        uint16_t yright
        ) :
        isLeftAssigned{isLeftAssigned},
        isRightAssigned{isRightAssigned},
        xleft{xleft},
        yleft{yleft},
        xright{xright},
        yright{yright}
        {}
} EyePair;

class Interface {
public:
    virtual ~Interface() {}

    /*!
     * \brief Initialize the face recognition API.
     *
     * \param configDir The path to the configuration directory containing the necessary files for initializing the face recognition API.
     *
     * \return A ReturnStatus object indicating the status of the initialization process.
     */
    virtual ReturnStatus
    initialize(const std::string &configDir) = 0;

    /*!
     * \brief Create a template from multiple face images.
     *
     * \param faces The multiple face images from which to create the template.
     * \param role The role of the template to be created (e.g., TemplateRole::Init_V, TemplateRole::Verification, etc.).
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
        std::vector<EyePair> &eyeCoordinates,
        std::vector<double> &quality) = 0;

    /*!
     * \brief Create a face template from the input face images.
     *
     * \param faces The Multiface object containing the input face images.
     * \param role The template role.
     * \param templ Output vector to store the created face template.
     * \param eyeCoordinates Output vector to store the eye coordinates of the detected face(s).
     * \param quality Output vector to store the quality values of the detected face(s).
     *
     * \return A ReturnStatus object indicating the status of the face template creation process.
     */
    virtual ReturnStatus
    matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &initTemplate,
        double &similarity) = 0;

    /*!
     * \brief Fine-tuning the face recognition model using the provided configuration and save the trained model to the specified directory.
     *
     * \param configDir The directory containing the configuration files required for training the face recognition model.
     * \param trainedConfigDir The directory where the trained face recognition model will be saved.
     *
     * \return A ReturnStatus object indicating the status of the training process.
     */
    virtual ReturnStatus
    train(
        const std::string &configDir,
        const std::string &trainedConfigDir) = 0;

    /*!
     * \brief Get the implementation of the Interface.
     *
     * \return A shared pointer to the implementation of the Interface.
     */
    static std::shared_ptr<Interface>
    getImplementation();
};
}
