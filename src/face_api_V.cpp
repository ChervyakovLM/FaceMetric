#include <sstream>
#include <fstream>

#include <glog/logging.h>

#include "face_api_V.h"
#include "timing.h"
#include "in_out_V.h"
#include "face_api.h"

/*!
 * \brief Call the FACEAPI_extract_template function to perform face extraction.
 *
 * \param face_api_ptr A shared_ptr to the Interface representing the FACEAPI object.
 * \param params A reference to the params_type containing the extraction parameters.
 * \param output_dir The output directory where extracted data will be stored.
 */
void FACEAPI_extract(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir)
{
    struct verif_traits
    {
        using face_api_type = Interface;
        using ReturnStatus = FACEAPITEST::ReturnStatus;
        using Image = FACEAPITEST::Image;
        using EyePair = FACEAPITEST::EyePair;
        using Multiface = FACEAPITEST::Multiface;
        using TemplateRole = FACEAPITEST::TemplateRole;
        using ReturnCode = FACEAPITEST::ReturnCode;
    };

    FACEAPI_extract_template<verif_traits, false>(face_api_ptr, params, output_dir, "extract_list");
}

/*!
 * \brief Call the FACEAPI_match function to perform face matching.
 *
 * \param face_api_ptr A shared_ptr to the Interface representing the FACEAPI object.
 * \param params A reference to the params_type containing the matching parameters.
 * \param output_dir The output directory where the matching results will be stored.
 */
void FACEAPI_match(shared_ptr<Interface> face_api_ptr, params_type& params, const string& output_dir)
{
    LOG(INFO) << "matchTemplates start...";

    auto descriptors = read_input_match(output_dir + "/" + get_filename(get_abs(params["extract_list"], params)) + ".bin", get_param<uint>(params["desc_size"]), output_dir + "/" + "counters.txt");

    vector<float> matches_true, matches_false;

    timing timer(true);

    const int log_step = 1000 * 1000;
    size_t counter = 0;
    size_t skip_match_count = 0;

    size_t desc_size = descriptors->size();

    bool match_debug_flag = get_param<bool>(params["debug_info"]);

    unique_ptr<ofstream> match_log;
    if(match_debug_flag)
        match_log = open_file_or_die<ofstream>(output_dir + "/match.txt");

    for(size_t i = 0; i < desc_size - 1; i++)
        for(size_t j = i + 1; j < desc_size; j++)
        {
            bool skip_match = false;

            int id_i = (*descriptors)[i].first;
            int id_j = (*descriptors)[j].first;

            if(id_i == 0 || id_j == 0)
                throw logic_error("can not matching, found image without label");

            if(id_i < 0)
            {
                id_i *= -1;
                skip_match = true;
            }

            if(id_j < 0)
            {
                id_j *= -1;
                skip_match = true;
            }

            double similarity = 0;
            if(!skip_match)
            {
                timer.start();
                ReturnStatus status = face_api_ptr->matchTemplates((*descriptors)[i].second, (*descriptors)[j].second, similarity);
                timer.stop();

                if(status.code != ReturnCode::Success)
                    throw runtime_error("matchTemplates failed, status: " + errcode_to_string(status.code));
            }
            else
                skip_match_count++;


            if(id_i == id_j)
                matches_true.push_back(static_cast<float>(similarity));
            else
                matches_false.push_back(static_cast<float>(similarity));

            if(match_debug_flag)
                *match_log << i << " " << id_i << " " << j << " " << id_j << " " << to_string_form(similarity, 7) << endl;

            counter++;

            if(counter % (10 * log_step) == 0)
                LOG(INFO) << "match " << counter/log_step << "M descriptor pairs";
        }

    LOG(INFO) << "all matches count: " << counter;
    LOG(INFO) << "matches true: " << matches_true.size();
    LOG(INFO) << "matches false: " << matches_false.size();
    LOG(INFO) << "skip matches: " << skip_match_count;

    write_output_match_search(output_dir + "/matches_true.bin", matches_true);
    write_output_match_search(output_dir + "/matches_false.bin", matches_false);

    //check_median_modify(matches_true, {0.9f, 1.0f});
    //check_median_modify(matches_false, {0.0f, 0.1f});
    check_median_modify(matches_true, {0.363f, 1.0f});
    check_median_modify(matches_false, {0.0f, 0.362f});

    LOG(INFO) << "matchTemplates done, average time - " << duration_to_string(timer.get_average());
    if(get_param<bool>(params["extra_timings"]))
        log_extended_info(timer.get_extended_info(get_param<uint>(params["percentile"]) / 100.f));
}

/*!
 * \brief Call the FACEAPI_ROC function to calculate the ROC curve for face matching.
 *
 * \param output_dir The output directory where the matching results are stored.
 */
void FACEAPI_ROC(const string& output_dir)
{
    LOG(INFO) << "calc ROC start...";

    timing timer;

    timer.start();
    auto matches = read_input_ROC_tpir(output_dir + "/matches_true.bin", output_dir + "/matches_false.bin");

    vector<int> fprs {4, 5, 6, 7, 8};
    vector<float> tprs = fastROC(matches->first, matches->second, fprs);

    write_output_ROC_tpir(output_dir + "/ROC.txt", fprs, tprs, {"fpr", "tpr"});
    auto interval = timer.stop();

    LOG(INFO) << "calc ROC done, time - " << duration_to_string(duration<double, sec_t>(interval), 2);
}

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
ReturnStatus createTemplateParam(shared_ptr<Interface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double> &quality)
{
    return face_api_ptr->createTemplate(faces, role, templ, eyeCoordinates, quality);
}









































