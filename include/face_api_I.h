#pragma once

#include "face_api_test_I.h"
#include "utils_I.h"

using namespace std;
using namespace FACEAPITEST;

/*!
 * \brief Performs the extraction stage for the FACEAPI identification.
 *
 * \param face_api_ptr A shared_ptr to an instance of IdentInterface representing the FACEAPI for identification.
 * \param params A reference to a params_type representing the input parameters.
 * \param output_dir A string representing the output directory where the results will be stored.
 */
void FACEAPI_extract(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);

/*!
 * \brief Calls the createTemplate function of the IdentInterface to create a template.
 *
 * \param face_api_ptr A shared_ptr to an instance of IdentInterface representing the FACEAPI for identification.
 * \param faces A constant reference to a Multiface object representing the input faces for template creation.
 * \param role A TemplateRole representing the role of the template.
 * \param templ A reference to a vector of uint8_t where the created template will be stored.
 * \param eyeCoordinates A reference to a vector of EyePair where the eye coordinates of the template will be stored.
 * \param quality A reference to a vector of double where the quality of the template will be stored (Not used in the function).
 *
 * \return A ReturnStatus indicating the status of the template creation.
 */
ReturnStatus createTemplateParam(shared_ptr<IdentInterface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double>&);

class FACEAPI
{
public:
    /*!
     * \brief Performs face template identification using the given IdentInterface instance.
     *
     * \param face_api_ptr A shared_ptr to an instance of IdentInterface representing the FACEAPI for identification.
     * \param params A reference to a params_type object representing the input parameters for the search.
     * \param output_dir A constant reference to a string representing the output directory where the results will be stored.
     */
    static void search(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);

    /*!
     * \brief Calculates the True Positive Identification Rate (TPIR) for different ranks.
     *
     * \param output_dir A constant reference to a string representing the output directory where the results will be stored.
     */
    static void tpir(const string& output_dir);

    /*!
     * \brief Inserts face descriptors into the gallery.
     *
     * \param face_api_ptr A shared pointer to an IdentInterface object representing the face API.
     * \param params A reference to a params_type object containing input parameters.
     * \param output_dir A constant reference to a string representing the output directory.
     */
    static void insert(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir);

    /*!
     * \brief Removes face descriptors from the gallery.
     *
     * \param face_api_ptr A shared pointer to an IdentInterface object representing the face API.
     * \param params A reference to a params_type object containing input parameters.
     */
    static void remove(shared_ptr<IdentInterface> face_api_ptr, params_type& params);

private:
    constexpr static int mc_ranks[] = {1, 5, 20};
};
