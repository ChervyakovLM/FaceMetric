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
        std::shared_ptr<uint8_t> &data
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
    Verification,
    Init_I,
    Identification
};

enum class ReturnCode {
    Success = 0,
    ConfigError,
    RefuseInput,
    ExtractError,
    ParseError,
    TemplateCreationError,
    VerifTemplateError,
    FaceDetectionError,
    NumDataError,
    TemplateFormatError,
    InitDirError,
    InputLocationError,
    MemoryError,
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
        return (s << "Either or both of the input templates were result of "
                "failed feature extraction");
    case ReturnCode::FaceDetectionError:
        return (s << "Unable to detect a face in the image");
    case ReturnCode::NumDataError:
        return (s << "Number of input images not supported");
    case ReturnCode::TemplateFormatError:
        return (s << "Template file is an incorrect format or defective");
    case ReturnCode::InitDirError:
        return (s << "An operation on the init directory failed");
    case ReturnCode::InputLocationError:
        return (s << "Cannot locate the input data - the input files or names "
                "seem incorrect");
    case ReturnCode::MemoryError:
        return (s << "Memory allocation failed (e.g. out of memory)");
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

typedef struct Candidate {
    bool isAssigned;

    std::string templateId;

    double similarityScore;

    Candidate() :
        isAssigned{false},
        templateId{""},
        similarityScore{0.0}
        {}

    Candidate(
        bool isAssigned,
        std::string templateId,
        double similarityScore) :
        isAssigned{isAssigned},
        templateId{templateId},
        similarityScore{similarityScore}
        {}
} Candidate;

}
