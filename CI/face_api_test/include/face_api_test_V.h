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

    // initialization of the algorithm for calculating biometric templates
    virtual ReturnStatus
    initialize(const std::string &configDir) = 0;

    // template calculation
    virtual ReturnStatus
    createTemplate(
        const Multiface &faces,
        TemplateRole role,
        std::vector<uint8_t> &templ,
        std::vector<EyePair> &eyeCoordinates,
        std::vector<double> &quality) = 0;

    // templates matching
    virtual ReturnStatus
    matchTemplates(
        const std::vector<uint8_t> &verifTemplate,
        const std::vector<uint8_t> &initTemplate,
        double &similarity) = 0;

    // fine-tuning the algorithm for calculating biometric templates
    virtual ReturnStatus
    train(
        const std::string &configDir,
        const std::string &trainedConfigDir) = 0;

    // get a pointer to the implementation
    static std::shared_ptr<Interface>
    getImplementation();
};
}
