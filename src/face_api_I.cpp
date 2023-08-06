#include "face_api_I.h"
#include "face_api.h"
#include "in_out_I.h"

constexpr int FACEAPI::mc_ranks[];

/*!
 * \brief Performs the extraction stage for the FACEAPI identification.
 *
 * \param face_api_ptr A shared_ptr to an instance of IdentInterface representing the FACEAPI for identification.
 * \param params A reference to a params_type representing the input parameters.
 * \param output_dir A string representing the output directory where the results will be stored.
 */
void FACEAPI_extract(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir)
{
    struct ident_traits
    {
        using face_api_type = IdentInterface;
        using ReturnStatus = FACEAPITEST::ReturnStatus;
        using Image = FACEAPITEST::Image;
        using EyePair = FACEAPITEST::EyePair;
        using Multiface = FACEAPITEST::Multiface;
        using TemplateRole = FACEAPITEST::TemplateRole;
        using ReturnCode = FACEAPITEST::ReturnCode;
    };

    if(check_list_warn("db_list", params))
        FACEAPI_extract_template<ident_traits, true>(face_api_ptr, params, output_dir, "db_list");

    if(check_list_warn("mate_list", params))
        FACEAPI_extract_template<ident_traits, false>(face_api_ptr, params, output_dir, "mate_list");

    if(check_list_warn("nonmate_list", params))
        FACEAPI_extract_template<ident_traits, false>(face_api_ptr, params, output_dir, "nonmate_list");

    if(check_list_warn("insert_list", params))
        FACEAPI_extract_template<ident_traits, false>(face_api_ptr, params, output_dir, "insert_list");
}

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
ReturnStatus createTemplateParam(shared_ptr<IdentInterface> face_api_ptr, const Multiface &faces, TemplateRole role, vector<uint8_t> &templ, vector<EyePair> &eyeCoordinates, vector<double>&)
{
    return face_api_ptr->createTemplate(faces, role, templ, eyeCoordinates);
}

int string_id_to_annot_id(const string& string_id)
{
    if(string_id == "none")
        return -1;

    size_t pos = string_id.find_first_of('_') + 1;
    return stoi(string_id.substr(pos, string_id.length() - pos));
}

/*!
 * \brief Performs face template identification using the given IdentInterface instance.
 *
 * \param face_api_ptr A shared_ptr to an instance of IdentInterface representing the FACEAPI for identification.
 * \param params A reference to a params_type object representing the input parameters for the search.
 * \param output_dir A constant reference to a string representing the output directory where the results will be stored.
 */
void FACEAPI::search(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir)
{
    LOG(INFO) << "identifyTemplate start...";

    uint desc_size = get_param<uint>(params["desc_size"]);

    vector<pair< shared_ptr<in_out_desc_type>, bool >> arrs_desc;
    arrs_desc.push_back({read_input_search(output_dir + "/" + get_filename(get_abs(params["mate_list"], params)) + ".bin", desc_size, "mate"), true});
    arrs_desc.push_back({read_input_search(output_dir + "/" + get_filename(get_abs(params["nonmate_list"], params)) + ".bin", desc_size, "nonmate"), false});

    vector<uint> ranks(begin(mc_ranks), end(mc_ranks));
    uint nearest_count = get_param<uint>(params["nearest_count"]);

    for(uint rank : ranks)
        if(rank >= nearest_count)
            throw logic_error("rank is greater or equal than nearest_count");

    ranks.push_back(nearest_count);

    vector<float> matches_false;
    vector<pair<uint, vector<float>>> matches_true_ranks;
    for(uint rank : ranks)
        matches_true_ranks.emplace_back(rank, vector<float>());

    unique_ptr<ofstream> search_log;
    if(get_param<bool>(params["debug_info"]))
        search_log = open_file_or_die<ofstream>(output_dir + "/search.txt");

    unique_ptr<ofstream> extra_log;
    if(get_param<bool>(params["search_info"]))
        extra_log = open_file_or_die<ofstream>(output_dir + "/search_extra.txt");

    timing timer(true);

    size_t counter = 0;
    size_t skip_queries = 0;
    for(auto& arr_desc : arrs_desc)
    {
        for(auto& desc : *arr_desc.first)
        {
            if(desc.first == 0)
                throw logic_error("can not do search, found image without label");

            vector<Candidate> candidateList;
            bool decision;

            if(desc.first < 0)
            {
                desc.first *= -1;
                candidateList.assign(nearest_count, {true, "none", 0});
                decision = false;
                skip_queries++;
            }
            else
            {
                timer.start();
                ReturnStatus status = face_api_ptr->identifyTemplate(desc.second, nearest_count, candidateList, decision);
                timer.stop();

                if(status.code != ReturnCode::Success)
                    throw runtime_error("identifyTemplate failed, status: " + errcode_to_string(status.code));
            }

            if(arr_desc.second)
            {
                for(auto& matches_true_rank : matches_true_ranks)
                {
                    if(matches_true_rank.first > candidateList.size())
                        throw runtime_error("too short candidateList size");

                    float score = 0;
                    for(size_t i = 0; i < matches_true_rank.first; i++)
                    {
                        if(desc.first == string_id_to_annot_id(candidateList[i].templateId))
                        {
                            score = static_cast<float>(candidateList[i].similarityScore);
                            break;
                        }
                    }

                    matches_true_rank.second.push_back(score);
                }
            }
            else
                matches_false.push_back(static_cast<float>(candidateList.front().similarityScore));

            if(search_log)
            {
                *search_log << "{" << desc.first << ", " << arr_desc.second << "} = ";
                for(const Candidate& cand : candidateList)
                    *search_log << "{" << cand.templateId << ", " << cand.similarityScore << "}, ";
                *search_log << endl;
            }

            if(extra_log)
                *extra_log << decision << " ";

            counter++;

            if(counter % 1000 == 0)
                LOG(INFO) << "process " << counter << " queries";
        }
    }

    LOG(INFO) << "general queries count: " << counter;

    if(skip_queries)
        LOG(WARNING) << "skip queries count: " << skip_queries;

    write_output_search_ranks(output_dir, matches_true_ranks, matches_false);

    ////for(auto& arr : matches_true_ranks)
    ////    check_median_modify(arr.second, {0.9f, 1.0f});
    ////check_median_modify(matches_false, {0.0f, 0.9f});
    ////for(auto& arr : matches_true_ranks)
    ////    check_median_modify(arr.second, {0.363f, 1.0f});
    ////check_median_modify(matches_false, {0.0f, 0.362f});


    LOG(INFO) << "identifyTemplate done, average time - " << duration_to_string(duration<double, milli>(timer.get_average()), 2);
    if(get_param<bool>(params["extra_timings"]))
        log_extended_info(timing::extended_info_cast<double, milli>(timer.get_extended_info(get_param<uint>(params["percentile"]) / 100.f)));
}

/*!
 * \brief Calculates the True Positive Identification Rate (TPIR) for different ranks.
 *
 * \param output_dir A constant reference to a string representing the output directory where the results will be stored.
 */
void FACEAPI::tpir(const string& output_dir)
{
    LOG(INFO) << "calc TPIRs start...";

    timing timer;

    timer.start();

    vector<uint> ranks(begin(mc_ranks), end(mc_ranks));
    ranks.insert(ranks.begin(), 0);

    for(uint rank : ranks)
    {
        string postfix;
        if(rank)
            postfix = "_r" + to_string(rank);

        auto matches = read_input_ROC_tpir(output_dir + "/matches_true" + postfix + ".bin", output_dir + "/matches_false.bin");

        vector<int> fpirs {1, 2, 3};
        vector<float> tpirs = fastROC(matches->first, matches->second, fpirs);

        write_output_ROC_tpir(output_dir + "/tpirs" + postfix + ".txt", fpirs, tpirs, {"fpir", "tpir"}, static_cast<int>(rank));
    }

    auto interval = timer.stop();
    LOG(INFO) << "calc TPIR done, time - " << duration_to_string(duration<double, milli>(interval), 2);
}

/*!
 * \brief Inserts face descriptors into the gallery.
 *
 * \param face_api_ptr A shared pointer to an IdentInterface object representing the face API.
 * \param params A reference to a params_type object containing input parameters.
 * \param output_dir A constant reference to a string representing the output directory.
 */
void FACEAPI::insert(shared_ptr<IdentInterface> face_api_ptr, params_type& params, const string& output_dir)
{
    LOG(INFO) << "galleryInsertID start...";

    uint desc_size = get_param<uint>(params["desc_size"]);

    auto descriptors_ins = read_input_search(output_dir + "/" + get_filename(get_abs(params["insert_list"], params)) + ".bin", desc_size, "insert");
    auto descriptors_db = read_input_search(output_dir + "/" + get_filename(get_abs(params["db_list"], params)) + ".bin", desc_size, "db");

    timing timer(true);

    static size_t counter_st = 0;
    size_t counter = 0;
    for(const auto& desc : *descriptors_ins)
    {
        timer.start();
        ReturnStatus status = face_api_ptr->galleryInsertID(desc.second, to_string(descriptors_db->size() + counter_st) + "_" + to_string(desc.first));
        timer.stop();

        if(status.code != ReturnCode::Success)
            throw runtime_error("galleryInsertID failed, status: " + errcode_to_string(status.code));

        counter++;
        counter_st++;

        if(counter % 1000 == 0)
            LOG(INFO) << "insert " << counter << " descriptors";
    }

    LOG(INFO) << "base, size after insert: " << descriptors_db->size() + counter_st;
    LOG(INFO) << "galleryInsertID done, average time - " << duration_to_string(duration<double, milli>(timer.get_average()), 2);
    if(get_param<bool>(params["extra_timings"]))
        log_extended_info(timing::extended_info_cast<double, milli>(timer.get_extended_info(get_param<uint>(params["percentile"]) / 100.f)));
}

/*!
 * \brief Removes face descriptors from the gallery.
 *
 * \param face_api_ptr A shared pointer to an IdentInterface object representing the face API.
 * \param params A reference to a params_type object containing input parameters.
 */
void FACEAPI::remove(shared_ptr<IdentInterface> face_api_ptr, params_type& params)
{
    LOG(INFO) << "galleryDeleteID start...";

    vector<string> remove_list = read_input_remove(get_abs(params["remove_list"], params));

    timing timer(true);
    size_t counter = 0;
    for(const string& id_str : remove_list)
    {
        timer.start();
        ReturnStatus status = face_api_ptr->galleryDeleteID(id_str);
        timer.stop();

        if(status.code != ReturnCode::Success)
            throw runtime_error("galleryDeleteID failed, id: " + id_str + ", status - " + errcode_to_string(status.code));

        counter++;

        if(counter % 1000 == 0)
            LOG(INFO) << "remove " << counter << " descriptors";
    }

    LOG(INFO) << "galleryDeleteID done, average time - " << duration_to_string(duration<double, micro>(timer.get_average()), 2);
    if(get_param<bool>(params["extra_timings"]))
        log_extended_info(timing::extended_info_cast<double, micro>(timer.get_extended_info(get_param<uint>(params["percentile"]) / 100.f)));
}










































