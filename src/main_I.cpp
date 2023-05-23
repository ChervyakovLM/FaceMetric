#include <glog/logging.h>

#include "utils_I.h"
#include "face_api_test_I.h"
#include "face_api_I.h"
#include "face_api.h"

using namespace std;
using namespace FACEAPITEST;

int main(int argc, char* argv[])
{
    try
    {
        google::InitGoogleLogging(argv[0]);
        google::InstallFailureSignalHandler();
        FLAGS_stderrthreshold = 0;
        FLAGS_colorlogtostderr = true;

        params_type params = parse_cmd_line(argc, argv);

        string split_dir = get_param<string>(params["split"]);
        string output_dir = split_dir + "/output";

        if(system(("mkdir -p " + output_dir + "/logs").c_str()) ||
           system(("mkdir -p " + output_dir + "/enroll").c_str()))
            throw runtime_error("creating output dir failed");

        FLAGS_log_dir = output_dir + "/logs";

        print_all(params);

        shared_ptr<IdentInterface> face_api_ptr = IdentInterface::getImplementation();

        timing timer;

        if(get_param<bool>(params["do_extract"]))
        {
            LOG(INFO) << "initializeTemplateCreation start...";
            timer.start();
            ReturnStatus status = face_api_ptr->initializeTemplateCreation(get_abs(params["config"], params), TemplateRole::Init_I);
            auto interval = timer.stop();
            if(status.code != ReturnCode::Success)
                throw runtime_error("initializeTemplateCreation failed, status: " + errcode_to_string(status.code));
            LOG(INFO) << "initializeTemplateCreation done, time - " << duration_to_string(duration<double, sec_t>(interval), 2);

            FACEAPI_extract(face_api_ptr, params, output_dir);
        }

        if(get_param<bool>(params["do_graph"]))
        {
            LOG(INFO) << "finalizeEnrollment start...";
            timer.start();
            ReturnStatus status = face_api_ptr->finalizeInit(get_abs(params["config"], params), output_dir + "/enroll", output_dir + "/" + get_filename(get_abs(params["db_list"], params)) + ".bin",
                                                               output_dir + "/manifest.txt");
            auto interval = timer.stop();
            if(status.code != ReturnCode::Success)
                throw runtime_error("finalizeEnrollment failed, status: " + errcode_to_string(status.code));
            LOG(INFO) << "finalizeEnrollment done, time - " << duration_to_string(duration<double, sec_t>(interval), 2);
        }

        if(get_param<bool>(params["do_insert"]) || get_param<bool>(params["do_remove"]) || get_param<bool>(params["do_search"]))
        {
            LOG(INFO) << "initializeIdentification start...";
            timer.start();
            ReturnStatus status = face_api_ptr->initializeIdentification(get_abs(params["config"], params), output_dir + "/enroll");
            auto interval = timer.stop();
            if(status.code != ReturnCode::Success)
                throw runtime_error("initializeIdentification failed, status: " + errcode_to_string(status.code));
            LOG(INFO) << "initializeIdentification done, time - " << duration_to_string(duration<double, sec_t>(interval), 2);
        }

        if(get_param<bool>(params["do_insert"]))
            FACEAPI::insert(face_api_ptr, params, output_dir);

        if(get_param<bool>(params["do_remove"]))
            FACEAPI::remove(face_api_ptr, params);

        if(get_param<bool>(params["do_search"]))
            FACEAPI::search(face_api_ptr, params, output_dir);

        if(get_param<bool>(params["do_tpir"]))
            FACEAPI::tpir(output_dir);
    }

    catch(const exception& e)
    {
        LOG(ERROR) << e.what();
        return 1;
    }

    return 0;
}





























































