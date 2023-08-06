#pragma once

#include "face_api_test_V.h"
#include "utils_V.h"

using namespace std;
using namespace FACEAPITEST;

/*!
 * \brief Call the FACEAPI_extract_template function to perform face extraction.
 *
 * \param face_api_ptr A shared_ptr to the Interface representing the FACEAPI object.
 * \param params A reference to the params_type containing the extraction parameters.
 * \param output_dir The output directory where extracted data will be stored.
 */
void FACEAPI_extract(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir);

/*!
 * \brief Call the FACEAPI_match function to perform face matching.
 *
 * \param face_api_ptr A shared_ptr to the Interface representing the FACEAPI object.
 * \param params A reference to the params_type containing the matching parameters.
 * \param output_dir The output directory where the matching results will be stored.
 */
void FACEAPI_match(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir);

/*!
 * \brief Call the FACEAPI_ROC function to calculate the ROC curve for face matching.
 *
 * \param output_dir The output directory where the matching results are stored.
 */
void FACEAPI_ROC(const string& output_dir);

/*!
 * \brief Create a face template with additional parameters.
 *
 * \param face_api_ptr A shared pointer to the Interface class.
 * \param faces The Multiface object containing multiple face images.
 * \param role The TemplateRole indicating the role of the template.
 * \param templ Output parameter for the created face template.
 * \param eyeCoordinates Output parameter for the eye coordinates of each face in the Multiface.
 * \param quality Output parameter for the quality of each face in the Multiface.
 *
 * \return The ReturnStatus indicating the success or failure of the template creation.
 */
ReturnStatus createTemplateParam(shared_ptr<Interface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double> &quality);
