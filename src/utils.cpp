#include <sys/wait.h>

#include <fstream>

#include <FreeImage.h>

#include "utils.h"

string param_to_string(shared_ptr<TCLAP::Arg> param)
{
    {
        auto param_ptr = dynamic_cast<TCLAP::ValueArg<string>*>(param.get());

        if(param_ptr)
            return param_ptr->getValue();
    }

    {
        auto param_ptr = dynamic_cast<TCLAP::ValueArg<bool>*>(param.get());

        if(param_ptr)
            return to_string(param_ptr->getValue());
    }

    {
        auto param_ptr = dynamic_cast<TCLAP::ValueArg<uint>*>(param.get());

        if(param_ptr)
            return to_string(param_ptr->getValue());
    }

    throw runtime_error("unsupported param type");
}

void print_params(const params_type& params, const string& header, bool print_default, bool print_changed, bool warn_flag)
{
    stringstream buf;

    buf << endl << header << ":" << endl;

    for(const auto& el : params)
    {
        if((print_changed && el.second->isSet()) || (print_default && !el.second->isSet()))
            buf << "\t" << el.first << " - " << param_to_string(el.second) << endl;
    }

    buf << endl << endl;

    (warn_flag ? LOG(WARNING) : LOG(INFO)) << buf.rdbuf();
}

void print_pipeline(params_type& params, const vector<string>& stages)
{
    stringstream buf;
    buf << endl << "pipeline:" << endl;

    int counter = 1;
    for(const string& stage : stages)
    {
        if(get_param<bool>(params["do_" + stage]))
            buf << "\t" << counter++ << ". " << stage << endl;
    }

    buf << endl << endl;

    LOG(INFO) << buf.rdbuf();
}

string relative_to_abs(const string& original, params_type& params)
{
    string gen_path = original;

    if(original.length())
        if(original[0] != '/')
            gen_path = get_param<string>(params["split"]) + "/" + original;

    return gen_path;
}

string get_abs(shared_ptr<TCLAP::Arg> arg, params_type& params)
{
    return relative_to_abs(get_param<string>(arg), params);
}

shared_ptr<uint8_t> get_bitmap(const string& file, bool gray_flag, size_t& width, size_t& height)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType(file.c_str());
    int flags = 0;
    if(fif == FIF_JPEG)
        flags = JPEG_ACCURATE;

    FIBITMAP* fibitmap  = FreeImage_Load(fif, file.c_str(), flags);

    if(fibitmap == nullptr)
    {
        FreeImage_DeInitialise();
        throw runtime_error("FreeImage: failed to open image " + file);
    }

    size_t channels = 0;
    if(gray_flag)
    {
        FIBITMAP* fibitmap_gray = FreeImage_ConvertToGreyscale(fibitmap);
        FreeImage_Unload(fibitmap);
        fibitmap = fibitmap_gray;
        channels = 1;
    }
    else
        channels = 3;

    FreeImage_FlipVertical(fibitmap);

    width = FreeImage_GetWidth(fibitmap);
    height = FreeImage_GetHeight(fibitmap);
    size_t bitmap_size = width * height * channels;

    shared_ptr<uint8_t> data(new uint8_t[bitmap_size], default_delete<uint8_t[]>());
    memcpy(data.get(), FreeImage_GetBits(fibitmap), bitmap_size * sizeof(uint8_t));

    FreeImage_Unload(fibitmap);

    return data;
}

void wait_all_forks()
{
    int status;
    pid_t pid_proc;
    vector<pid_t> err_proc;
    while((pid_proc = wait(&status)) > 0)
    {
        if(WEXITSTATUS(status) != 0)
            err_proc.push_back(pid_proc);
    }

    if(!err_proc.empty())
    {
        stringstream err_str;
        err_str << "Errors in child processes on extract stage, Err PIDs: ";

        for(size_t i = 0; i < err_proc.size(); i++)
        {
            err_str << err_proc[i];
            if(i < err_proc.size() - 1)
                err_str << ", ";
        }

        throw runtime_error(err_str.str());
    }
}

string get_filename(const string& file)
{
    size_t begin_pos = file.rfind('/');
    size_t end_pos = file.rfind('.');

    if(begin_pos >= end_pos)
        return string();

    return file.substr(begin_pos + 1, end_pos - begin_pos - 1);
}

bool check_list_warn(const string& list_name, params_type& params)
{
    string filename = get_abs(params[list_name], params);
    ifstream file(filename);

    if(file.is_open())
        return true;
    else
    {
        LOG(WARNING) << "failed to open " << filename << ", skip extract " << list_name;
        return false;
    }
}

void check_median_modify(vector<float>& arr, pair<float, float> min_max_values)
{
    size_t pos = arr.size() / 2;
    nth_element(arr.begin(), arr.begin() + static_cast<long>(pos), arr.end());

    if(!(arr[pos] >= min_max_values.first && arr[pos] <= min_max_values.second))
        throw runtime_error("Wrong similarity median: " + to_string_form(min_max_values.first, 2) + " <= " + to_string_form(arr[pos], 2) + " <= " + to_string_form(min_max_values.second, 2));
}









































