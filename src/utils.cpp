#include <sys/wait.h>

#include <fstream>

#include <FreeImage.h>

#include "utils.h"

/*!
 * \brief Converts a parameter to its string representation.
 *
 * \param param The shared pointer to the parameter to be converted.
 *
 * \return The string representation of the parameter value.
 *
 * \throws runtime_error if the parameter type is not supported.
 */
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

/*!
 * \brief Print parameters with optional filters and warning flag.
 *
 * \param params The map of parameters to print.
 * \param header The header text for the printed output.
 * \param print_default Flag to indicate whether to print parameters with default values.
 * \param print_changed Flag to indicate whether to print parameters with changed values.
 * \param warn_flag Flag to indicate whether to log the output with a warning severity level.
 */
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

/*!
 * \brief Print pipeline stages based on their corresponding parameters.
 *
 * \param params The map of parameters that control each pipeline stage.
 * \param stages The vector of pipeline stage names in their execution order.
 */
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

/*!
 * \brief Converts a relative file path to an absolute file path based on the 'split' parameter.
 *
 * \param original The relative file path that needs to be converted to an absolute path.
 * \param params The dictionary containing parameters, including the 'split' parameter used as the base directory.
 *
 * \return The absolute file path as a string.
 */
string relative_to_abs(const string& original, params_type& params)
{
    string gen_path = original;

    if(original.length())
        if(original[0] != '/')
            gen_path = get_param<string>(params["split"]) + "/" + original;

    return gen_path;
}

/*!
 * \brief Get the absolute file path from a parameter value using a map of parameters.
 *
 * \param arg A shared_ptr to a TCLAP::Arg object representing the parameter.
 * \param params A map of parameters containing the necessary information for path conversion.
 *
 * \return The absolute file path corresponding to the parameter value.
 */
string get_abs(shared_ptr<TCLAP::Arg> arg, params_type& params)
{
    return relative_to_abs(get_param<string>(arg), params);
}

/*!
 * \brief Load and extract bitmap data from an image file.
 *
 * \param file The file path of the image to load.
 * \param gray_flag Flag to indicate whether to convert the image to grayscale.
 * \param width Reference to store the width of the loaded bitmap.
 * \param height Reference to store the height of the loaded bitmap.
 *
 * \return A shared_ptr to the loaded bitmap data as an array of uint8_t.
 */
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

/*!
 * \brief Wait for all child processes to complete and check for errors.
 */
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

/*!
 * \brief Extract the filename from a given file path.
 *
 * \param file The input file path from which the filename needs to be extracted.
 *
 * \return The extracted filename without the path and extension.
 */
string get_filename(const string& file)
{
    size_t begin_pos = file.rfind('/');
    size_t end_pos = file.rfind('.');

    if(begin_pos >= end_pos)
        return string();

    return file.substr(begin_pos + 1, end_pos - begin_pos - 1);
}

/*!
 * \brief Check if a file exists and issue a warning if not found.
 *
 * \param list_name The name of the parameter containing the file path to check.
 * \param params The map of parameters containing the file path parameter.
 *
 * \return 'true' if the file exists, 'false' otherwise.
 */
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

/*!
 * \brief Check and modify a vector to ensure the median value lies within a specified range.
 *
 * \param arr The input vector of float values to check and possibly modify.
 * \param min_max_values A pair of float values representing the minimum and maximum allowable median range.
 */
void check_median_modify(vector<float>& arr, pair<float, float> min_max_values)
{
    size_t pos = arr.size() / 2;
    nth_element(arr.begin(), arr.begin() + static_cast<long>(pos), arr.end());

    if(!(arr[pos] >= min_max_values.first && arr[pos] <= min_max_values.second))
        throw runtime_error("Wrong similarity median: " + to_string_form(min_max_values.first, 2) + " <= " + to_string_form(arr[pos], 2) + " <= " + to_string_form(min_max_values.second, 2));
}









































