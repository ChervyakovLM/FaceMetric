#pragma once

#include <unordered_map>
#include <memory>
#include <numeric>
#include <cmath>

#include <tclap/CmdLine.h>
#include <glog/logging.h>

#include "timing.h"

#define Q(str) #str
#define QUOTES(str) Q(str)

using namespace std;

typedef unordered_map<string, shared_ptr<TCLAP::Arg>> params_type;

template<typename T_val>
/*!
 * \brief Get the value of a parameter from a shared_ptr to a TCLAP::Arg object.
 *
 * \param arg A shared_ptr to a TCLAP::Arg object representing the parameter.
 *
 * \return The value of the parameter of type 'T_val'.
 */
T_val get_param(shared_ptr<TCLAP::Arg> arg);

template<typename T_stream_type, typename ...Args>
/*!
 * \brief Open a file or throw an exception if the file cannot be opened.
 *
 * \param file The name or path of the file to open.
 * \param args Optional arguments to pass to the constructor of 'T_stream_type'.
 *
 * \return A unique_ptr to the opened stream object of type 'T_stream_type'.
 */
unique_ptr<T_stream_type> open_file_or_die(const string& file, const Args&... args);

/*!
 * \brief Get the absolute file path from a parameter value using a map of parameters.
 *
 * \param arg A shared_ptr to a TCLAP::Arg object representing the parameter.
 * \param params A map of parameters containing the necessary information for path conversion.
 *
 * \return The absolute file path corresponding to the parameter value.
 */
string get_abs(shared_ptr<TCLAP::Arg> arg, params_type& params);

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
shared_ptr<uint8_t> get_bitmap(const string& file, bool gray_flag, size_t& width, size_t& height);

/*!
 * \brief Wait for all child processes to complete and check for errors.
 */
void wait_all_forks();

template<typename T_time>
/*!
 * \brief Log extended timing information with optional fork index.
 *
 * \param The extended timing information to log.
 * \param The optional index of the forked process (if applicable).
 */
void log_extended_info(const timing::extended_info_type<T_time>& info, int fork_index = -1);

template<typename T_val>
/*!
 * \brief Convert a value to a string representation with a specified precision.
 *
 * \param The value to convert to a string.
 * \param The desired precision for the string representation.
 *
 * \return A string representation of the value with the specified precision.
 */
string to_string_form(T_val val, int prec);

/*!
 * \brief Print parameters with optional filters and warning flag.
 *
 * \param params The map of parameters to print.
 * \param header The header text for the printed output.
 * \param print_default Flag to indicate whether to print parameters with default values.
 * \param print_changed Flag to indicate whether to print parameters with changed values.
 * \param warn_flag Flag to indicate whether to log the output with a warning severity level.
 */
void print_params(const params_type& params, const string& header, bool print_default, bool print_changed, bool warn_flag);

/*!
 * \brief Print pipeline stages based on their corresponding parameters.
 *
 * \param params The map of parameters that control each pipeline stage.
 * \param stages The vector of pipeline stage names in their execution order.
 */
void print_pipeline(params_type& params, const vector<string>& stages);

/*!
 * \brief Extract the filename from a given file path.
 *
 * \param file The input file path from which the filename needs to be extracted.
 *
 * \return The extracted filename without the path and extension.
 */
string get_filename(const string& file);

/*!
 * \brief Check if a file exists and issue a warning if not found.
 *
 * \param list_name The name of the parameter containing the file path to check.
 * \param params The map of parameters containing the file path parameter.
 *
 * \return 'true' if the file exists, 'false' otherwise.
 */
bool check_list_warn(const string& list_name, params_type& params);

/*!
 * \brief Check and modify a vector to ensure the median value lies within a specified range.
 *
 * \param arr The input vector of float values to check and possibly modify.
 * \param min_max_values A pair of float values representing the minimum and maximum allowable median range.
 */
void check_median_modify(vector<float>& arr, pair<float, float> min_max_values);

template<typename score_type>
/*!
 * \brief Calculate True Positive Rates (TPRs) at specified False Positive Rates (FPRs) for a binary classification model.
 *
 * \param matches_true A vector of score_type representing true positive scores.
 * \param matches_false A vector of score_type representing false positive scores (will be modified by the function).
 * \param fprs A vector of integers representing the desired False Positive Rates (FPRs) in descending order.
 *
 * \return A vector of score_type containing the True Positive Rates (TPRs) corresponding to the given FPRs.
 */
vector<score_type> fastROC(const vector<score_type>& matches_true, vector<score_type>& matches_false, const vector<int>& fprs);


//-----------------------------------------------------------------------Template Implementation-----------------------------------------------------------------------------------


template<typename T_val>
/*!
 * \brief Get the value of a parameter from a shared_ptr to a TCLAP::Arg object.
 *
 * \param arg A shared_ptr to a TCLAP::Arg object representing the parameter.
 *
 * \return The value of the parameter of type 'T_val'.
 */
T_val get_param(shared_ptr<TCLAP::Arg> arg)
{
    return dynamic_cast<TCLAP::ValueArg<T_val>*>(arg.get())->getValue();
}

template<typename T_stream_type, typename ...Args>
/*!
 * \brief Open a file or throw an exception if the file cannot be opened.
 *
 * \param file The name or path of the file to open.
 * \param args Optional arguments to pass to the constructor of 'T_stream_type'.
 *
 * \return A unique_ptr to the opened stream object of type 'T_stream_type'.
 */
unique_ptr<T_stream_type> open_file_or_die(const string& file, const Args&... args)
{
    unique_ptr<T_stream_type> stream_ptr(new T_stream_type(file, args...));
    if(stream_ptr->is_open())
        return stream_ptr;
    else
        throw runtime_error("failed to open " + file);
}

template<typename T_time>
/*!
 * \brief Log extended timing information with an optional fork index.
 *
 * \param info The timing::extended_info_type object containing the extended timing information.
 * \param fork_index The optional index of the forked process (if applicable).
 */
void log_extended_info(const timing::extended_info_type<T_time>& info, int fork_index)
{
    stringstream buf;
    buf << endl;

    if(fork_index >= 0)
        buf << "proc " << fork_index << " - ";

    buf << "extended timings:" << endl;

    buf << "\t" << info.percentile <<"-th percentile: " << duration_to_string(info.percentile_val) << endl;
    buf << "\tmin: " << duration_to_string(info.min_val) << endl;
    buf << "\tmax: " << duration_to_string(info.max_val) << endl;
    buf << "\tstd_dev: " << duration_to_string(info.std_dev) << endl;

    buf << endl << endl;

    LOG(INFO) << buf.rdbuf();
}

template<typename T_val>
/*!
 * \brief Convert a value to a string representation with a specified precision.
 *
 * \param val The value to convert to a string.
 * \param prec The desired precision for the string representation.
 *
 * \return A string representation of the value with the specified precision.
 */
string to_string_form(T_val val, int prec)
{
    stringstream buf;
    buf << fixed << setprecision(prec) << val;
    return buf.str();
}

template<typename score_type>
/*!
 * \brief Calculate True Positive Rates (TPRs) at specified False Positive Rates (FPRs) for binary classification.
 *
 * \param matches_true A vector of score_type representing true positive scores.
 * \param matches_false A vector of score_type representing false positive scores (will be modified by the function).
 * \param fprs A vector of integers representing the desired False Positive Rates (FPRs) in ascending order.
 *
 * \return A vector of score_type containing the True Positive Rates (TPRs) corresponding to the given FPRs.
 */
vector<score_type> fastROC(const vector<score_type>& matches_true, vector<score_type>& matches_false, const vector<int>& fprs)
{
    vector<size_t> fprs_ids(fprs.size());
    iota(fprs_ids.begin(), fprs_ids.end(), 0);
    sort(fprs_ids.begin(), fprs_ids.end(), [&fprs] (size_t id1, size_t id2) {return fprs[id1] < fprs[id2];});

    vector<score_type> score_ths(fprs.size());
    size_t false_thresh_prev = 0;
    for(size_t fpr_id: fprs_ids)
    {
        size_t false_thresh_cur = static_cast<size_t>(matches_false.size() * pow(10, fprs[fpr_id] * -1));

        if(!false_thresh_cur)
        {
            score_ths[fpr_id] = -1;
            continue;
        }

        auto end_it = false_thresh_prev ? (matches_false.begin() + static_cast<long>(false_thresh_prev - 1)) : matches_false.end();

        nth_element(matches_false.begin(), matches_false.begin() + static_cast<long>(false_thresh_cur - 1), end_it, greater<score_type>());
        score_ths[fpr_id] = matches_false[false_thresh_cur - 1];

        false_thresh_prev = false_thresh_cur;
    }

    size_t score_ths_size = score_ths.size();
    vector<size_t> counters(score_ths_size, 0);
    for(score_type score : matches_true)
    {
        for(size_t i = 0; i < score_ths_size; i++)
            if(score > score_ths[i])
                counters[i]++;
    }

    score_type tpr_step = static_cast<score_type>(1.0/matches_true.size());
    vector<score_type> tprs;
    for(size_t i = 0; i < counters.size(); i++)
        tprs.push_back(score_ths[i] < 0 ? -1 : counters[i] * tpr_step);

    return tprs;
}






































