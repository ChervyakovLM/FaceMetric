#include <thread>

#include "utils_I.h"

/*!
 * \brief Parse command-line arguments and retrieve parameters.
 *
 * \param argc The number of command-line arguments.
 * \param argv An array of character pointers containing the command-line arguments.
 *
 * \return A map of parameters with their associated command-line arguments.
 */
params_type parse_cmd_line(int argc, char* argv[])
{
    TCLAP::CmdLine cmd("checkFACEAPI Identification", '=');

    params_type params;

    params["split"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "split", "path to split directory", true, "", "string"));
    params["config"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "config", "path to FaceEngine config directory", false, "input/config", "string"));

    params["db_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "db_list", "path to db list file", false, "input/db.txt", "string"));
    params["mate_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "mate_list", "path to mate queries list file", false, "input/mate.txt", "string"));
    params["nonmate_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "nonmate_list", "path to nonmate queries list file", false, "input/nonmate.txt", "string"));

    params["insert_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "insert_list", "path to insert list file", false, "input/insert.txt", "string"));
    params["remove_list"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "remove_list", "path to remove list file", false, "input/remove.txt", "string"));

    params["extract_prefix"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<string>("", "extract_prefix", "path to images directory", false, "input/images", "string"));
    params["grayscale"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "grayscale", "open images as grayscale", false, false, "bool"));
    params["count_proc"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "count_proc", "count extract processes", false, thread::hardware_concurrency(), "unsigned int"));
    params["extra_timings"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "extra_timings", "print extra timings: percentile, min, max, std_dev", false, false, "bool"));
    params["extract_info"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "extract_info", "logging additional extract results: eyes, quality, etc", false, false, "bool"));
    params["debug_info"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "debug_info", "logging debug output", false, false, "bool"));
    params["desc_size"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "desc_size", "descriptor size", false, 512, "unsigned int"));
    params["percentile"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "percentile", "percentile in %", false, 90, "unsigned int"));

    params["nearest_count"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<uint>("", "nearest_count", "nearest count", false, 100, "unsigned int"));
    params["search_info"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "search_info", "logging additional search results: decision", false, false, "bool"));

    params["do_extract"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_extract", "do extract stage", false, true, "bool"));
    params["do_graph"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_graph", "do create graph stage", false, true, "bool"));
    params["do_insert"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_insert", "do insert stage", false, true, "bool"));
    params["do_remove"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_remove", "do remove stage", false, true, "bool"));
    params["do_search"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_search", "do search stage", false, true, "bool"));
    params["do_tpir"] = shared_ptr<TCLAP::Arg>(new TCLAP::ValueArg<bool>("", "do_tpir", "do calc TPIR/FPIR stage", false, true, "bool"));

    for(const auto& el : params)
        cmd.add(*el.second);

    cmd.parse(argc, argv);

    return params;
}

/*!
 * \brief Print all program information and options.
 *
 * \param params The map of parameters containing the program options.
 */
void print_all(params_type& params)
{
    LOG(INFO) << "commit: " << QUOTES(COMMIT_MESSAGE);
    print_params(params, "default options", true, false, false);
    print_params(params, "changed options", false, true, true);
    print_pipeline(params, {"extract", "graph", "insert", "remove", "search", "tpir"});
}
