#include <glog/logging.h>

#include "utils_V.h"
#include "face_api_test_V.h"
#include "face_api_V.h"
#include "face_api.h"
#include "timing.h"

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

        if(system(("mkdir -p " + output_dir + "/logs").c_str()))
            throw runtime_error("creating output dir failed");

        FLAGS_log_dir = output_dir + "/logs";

        print_all(params);

        shared_ptr<Interface> face_api_ptr = Interface::getImplementation();

        timing timer;

        LOG(INFO) << "initialize start...";
        timer.start();
        ReturnStatus status = face_api_ptr->initialize(get_abs(params["config"], params));
        auto interval = timer.stop();
        if(status.code != ReturnCode::Success)
            throw runtime_error("initialize failed, status: " + errcode_to_string(status.code));
        LOG(INFO) << "initialize done, time - " << duration_to_string(duration<double, sec_t>(interval), 2);

        if(get_param<bool>(params["do_extract"]))
            FACEAPI_extract(face_api_ptr, params, output_dir);

        if(get_param<bool>(params["do_match"]))
            FACEAPI_match(face_api_ptr, params, output_dir);

        if(get_param<bool>(params["do_ROC"]))
            FACEAPI_ROC(output_dir);
    }

    catch(const exception& e)
    {
        LOG(ERROR) << e.what();
        return 1;
    }

    return 0;
}





























































