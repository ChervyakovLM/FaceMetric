#include <thread>

#include "utils_V.h"

params_type parse_cmd_line(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("checkFACEAPI Verification", '=');

    params_type params;

    params["split"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "split", "path to split directory", true, "", "string"));
    params["config"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "config", "path to FaceEngine config directory", false, "input/config", "string"));
    params["extract_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "extract_list", "path to extract list file", false, "input/extract.txt", "string"));
    params["extract_prefix"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "extract_prefix", "path to images directory", false, "input/images", "string"));
    params["grayscale"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "grayscale", "open images as grayscale", false, false, "bool"));
    params["count_proc"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "count_proc", "count extract processes", false, thread::hardware_concurrency(), "unsigned int"));
    params["extra_timings"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "extra_timings", "print extra timings: percentile, min, max, std_dev", false, false, "bool"));
    params["extract_info"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "extract_info", "logging additional extract results: eyes, quality, etc", false, false, "bool"));
    params["debug_info"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "debug_info", "logging debug output", false, false, "bool"));
    params["desc_size"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "desc_size", "descriptor size", false, 512, "unsigned int"));
    params["percentile"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "percentile", "percentile in %", false, 90, "unsigned int"));

    params["do_extract"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_extract", "do extract stage", false, true, "bool"));
    params["do_match"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_match", "do match stage", false, true, "bool"));
    params["do_ROC"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_ROC", "do calc ROC stage", false, true, "bool"));

    for(const auto& el : params)
        cmd.add(*el.second);

    cmd.parse(argc, argv);

    return params;
}

void print_all(params_type& params)
{
    LOG(INFO) << "commit: " << QUOTES(COMMIT_MESSAGE);
    print_params(params, "default options", true, false, false);
    print_params(params, "changed options", false, true, true);
    print_pipeline(params, {"extract", "match", "ROC"});
}





























