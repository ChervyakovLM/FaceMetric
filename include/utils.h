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
T_val get_param(shared_ptr<TCLAP::Arg> arg);

template<typename T_stream_type, typename ...Args>
unique_ptr<T_stream_type> open_file_or_die(const string& file, const Args&... args);

string get_abs(shared_ptr<TCLAP::Arg> arg, params_type& params);

shared_ptr<uint8_t> get_bitmap(const string& file, bool gray_flag, size_t& width, size_t& height);

void wait_all_forks();

template<typename T_time>
void log_extended_info(const timing::extended_info_type<T_time>& info, int fork_index = -1);

template<typename T_val>
string to_string_form(T_val val, int prec);

void print_params(const params_type& params, const string& header, bool print_default, bool print_changed, bool warn_flag);
void print_pipeline(params_type& params, const vector<string>& stages);

string get_filename(const string& file);
bool check_list_warn(const string& list_name, params_type& params);
void check_median_modify(vector<float>& arr, pair<float, float> min_max_values);

template<typename score_type>
vector<score_type> fastROC(const vector<score_type>& matches_true, vector<score_type>& matches_false, const vector<int>& fprs);


//-----------------------------------------------------------------------Template Implementation-----------------------------------------------------------------------------------


template<typename T_val>
T_val get_param(shared_ptr<TCLAP::Arg> arg)
{
    return dynamic_cast<TCLAP::ValueArg<T_val>*>(arg.get())->getValue();
}

template<typename T_stream_type, typename ...Args>
unique_ptr<T_stream_type> open_file_or_die(const string& file, const Args&... args)
{
    unique_ptr<T_stream_type> stream_ptr(new T_stream_type(file, args...));
    if(stream_ptr->is_open())
        return stream_ptr;
    else
        throw runtime_error("failed to open " + file);
}

template<typename T_time>
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
string to_string_form(T_val val, int prec)
{
    stringstream buf;
    buf << fixed << setprecision(prec) << val;
    return buf.str();
}

template<typename score_type>
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






































