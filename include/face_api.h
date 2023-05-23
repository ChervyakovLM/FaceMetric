#pragma once

#include <FreeImage.h>

#include "utils.h"
#include "in_out.h"

using namespace std;

template<typename T_ReturnCode>
string errcode_to_string(const T_ReturnCode& code);

template<typename T_FACEAPI, bool create_manifest_flag>
void FACEAPI_extract_template(shared_ptr<typename T_FACEAPI::face_api_type> face_api_ptr, params_type& params, const string& output_dir, const string& list_name);


//-----------------------------------------------------------------------Template Implementation-----------------------------------------------------------------------------------


template<typename T_ReturnCode>
string errcode_to_string(const T_ReturnCode& code)
{
    stringstream string_stream;
    string_stream << code;
    return string_stream.str();
}

template<typename T_FACEAPI, bool create_manifest_flag>
void FACEAPI_extract_template(shared_ptr<typename T_FACEAPI::face_api_type> face_api_ptr, params_type& params, const string& output_dir, const string& list_name)
{
    const string list_file = get_abs(params[list_name], params);
    const string file_long_prefix =  output_dir + "/" + get_filename(list_file);

    LOG(INFO) << "createTemplate start for " + list_file + "...";

    {
        unique_ptr<ofstream> desc_stream = open_file_or_die<ofstream>(file_long_prefix + ".bin");

        if(get_param<bool>(params["extract_info"]))
            unique_ptr<ofstream> extract_info_stream = open_file_or_die<ofstream>(file_long_prefix + "_info.txt");

        unique_ptr<ofstream> fail_detect_stream = open_file_or_die<ofstream>(file_long_prefix + "_fail.txt");

        if(get_param<bool>(params["debug_info"]))
            unique_ptr<ofstream> debug_info_stream = open_file_or_die<ofstream>(file_long_prefix + "_debug_info.txt");
    }

    uint count_proc = get_param<uint>(params["count_proc"]);
    uint desc_size = get_param<uint>(params["desc_size"]);

    size_t count_templ = 0;
    auto input_list = read_input_extract(list_file, count_proc, count_templ);

    string extract_prefix = get_abs(params["extract_prefix"], params) + "/";
    bool gray_flag = get_param<bool>(params["grayscale"]);

    timing timer(true);

    in_out_desc_type output_desc;
    vector< tuple<vector<string>, vector<typename T_FACEAPI::EyePair>, vector<double>> > extra_output;
    fail_detect_type fail_detect;

    size_t counter = 0;
    FreeImage_Initialise();

    string semaphore_name = "/FACEAPI_extract_" + to_string(getpid());
    linux_scoped_mutex::remove(semaphore_name);

    size_t fork_index = 0;
    for(size_t i = 0; i < count_proc - 1; i++)
    {
        if(fork() == 0)
        {
            fork_index = i + 1;
            break;
        }
    }

    if(fork_index < input_list->size())
    {
        size_t refusal_count = 0;
        for(auto& batch_extract_list : (*input_list)[fork_index])
        {
            typename T_FACEAPI::Multiface template_images;
            for(const string& path : batch_extract_list.first)
            {
                size_t bitmap_W = 0, bitmap_H = 0;
                shared_ptr<uint8_t> bitmap = get_bitmap(extract_prefix + path, gray_flag, bitmap_W, bitmap_H);
                template_images.emplace_back(static_cast<uint16_t>(bitmap_W), static_cast<uint16_t>(bitmap_H), gray_flag ? 8 : 24, bitmap);
            }

            vector<uint8_t> descriptor;
            vector<typename T_FACEAPI::EyePair> eyeCoordinates;
            vector<double> quality;

            timer.start();
            typename T_FACEAPI::ReturnStatus status = createTemplateParam(face_api_ptr, template_images, T_FACEAPI::TemplateRole::Init_V, descriptor, eyeCoordinates, quality);
            timer.stop();

            if(status.code == T_FACEAPI::ReturnCode::RefuseInput)
            {
                batch_extract_list.second *= -1;
                descriptor.assign(desc_size, 0);
                refusal_count++;

                fail_detect.push_back(batch_extract_list.first);
            }
            else
            {
                if(status.code != T_FACEAPI::ReturnCode::Success)
                {
                    string images_paths;
                    for(const string& path: batch_extract_list.first)
                        images_paths += path + " ";

                    throw runtime_error("createTemplate failed, status: " + errcode_to_string(status.code)
                                             + ", template images: " + images_paths);
                }
                else
                {
                    if(descriptor.size() != desc_size)
                        throw runtime_error("wrong descriptor size: " + to_string(descriptor.size()) + " vs " + to_string(desc_size));
                }
            }

            if(get_param<bool>(params["extract_info"]))
                extra_output.push_back(make_tuple(batch_extract_list.first, eyeCoordinates, quality));

            output_desc.push_back({batch_extract_list.second, move(descriptor)});

            counter++;
            if(counter % 100 == 0)
                LOG(INFO) << "proc " << fork_index << " - extract " << counter << " descriptors";
        }
        FreeImage_DeInitialise();

        write_output_extract(output_desc, file_long_prefix + ".bin", semaphore_name, input_list, fork_index, extra_output,  file_long_prefix + "_info.txt",
                    fail_detect, file_long_prefix + "_fail.txt", get_param<bool>(params["debug_info"]), file_long_prefix + "_debug_info.txt", desc_size);

        LOG(INFO) << "proc " << fork_index << " - extract count: " << counter;

        if(refusal_count)
            LOG(WARNING) << "proc " << fork_index << " - REFUSAL count: " << refusal_count;

        LOG(INFO) << "proc " << fork_index << " - createTemplate done, average time - " << duration_to_string(duration<double, milli>(timer.get_average()));
        if(get_param<bool>(params["extra_timings"]))
            log_extended_info(timing::extended_info_cast<double, milli>(timer.get_extended_info(get_param<uint>(params["percentile"]) / 100.f)), static_cast<int>(fork_index));
    }

    if(fork_index != 0)
        exit(0);

    wait_all_forks();

    if(create_manifest_flag)
        write_manifest(output_dir + "/manifest.txt", file_long_prefix + ".bin", desc_size);
}
