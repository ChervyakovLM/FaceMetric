#pragma once

#include <semaphore.h>
#include <fcntl.h>

#include <map>
#include <memory>
#include <vector>
#include <fstream>

#include "utils.h"

using namespace std;

typedef vector< vector<pair<vector<string>, int>> > input_list_type;
typedef vector<pair<int, vector<uint8_t>>> in_out_desc_type;
typedef vector<vector<string>> fail_detect_type;
typedef pair<vector<float>, vector<float>> matches_type;

/*!
 * \brief Read input for extraction and create shared_ptr to input_list_type.
 *
 * \param file The file path containing the input data.
 * \param count_proc The number of extraction processes.
 * \param count_templ Output parameter to store the count of templates read.
 *
 * \return A shared_ptr to input_list_type containing the read input data.
 */
shared_ptr<input_list_type> read_input_extract(const string& file, uint count_proc, size_t& count_templ);

template<typename T_EyePair>
/*!
 * \brief Write extraction output and related information to files.
 *
 * \param output The in_out_desc_type containing the extraction output data.
 * \param file_desc The file path to write the descriptor data.
 * \param semaphore_name The name of the semaphore to lock during writing.
 * \param input_list A shared_ptr to input_list_type containing the input data (not used in this function).
 * \param fork_index The index of the forked process (not used in this function).
 * \param extra_output A vector of tuples containing additional output data (not used in this function).
 * \param file_extra The file path to write the additional output data (not used in this function).
 * \param fail_detect The fail_detect_type containing the failed detection data (not used in this function).
 * \param file_fail The file path to write the failed detection data (not used in this function).
 * \param debug_flag A boolean flag indicating whether to enable debug output (not used in this function).
 * \param file_debug The file path to write the debug output (not used in this function).
 * \param desc_size The descriptor size (not used in this function).
 */
void write_output_extract(const in_out_desc_type& output, const string& file_desc, const string& semaphore_name, shared_ptr<const input_list_type> input_list, size_t fork_index,
                  const vector< tuple<vector<string>, vector<T_EyePair>, vector<double>> >& extra_output, const string& file_extra, const fail_detect_type& fail_detect, const string& file_fail, bool debug_flag, const string& file_debug, uint desc_size);

/*!
 * \brief Write a manifest file based on the descriptors read from the desc_file.
 *
 * \param out_file The file path to write the manifest data.
 * \param desc_file The file path containing the descriptor data.
 * \param desc_size The descriptor size.
 */
void write_manifest(const string& out_file, const string& desc_file, uint desc_size);

/*!
 * \brief Read input for match or search and create shared_ptr to in_out_desc_type.
 *
 * \param file The file path containing the input data.
 * \param desc_size The descriptor size.
 * \param match_log A boolean flag indicating whether to log match count (not used in this function).
 * \param log_prefix The log prefix to use in case of logging (not used in this function).
 * \param counters_file The file path to write the count of descriptors and refusals (not used in this function).
 *
 * \return A shared_ptr to in_out_desc_type containing the read input data.
 */
shared_ptr<in_out_desc_type> read_input_match_search(const string& file, uint desc_size, bool match_log, const string& log_prefix = "", const string& counters_file = "");

/*!
 * \brief Write match or search output to a binary file.
 *
 * \param file The file path to write the output data.
 * \param matches A vector of floats containing the match or search output data.
 */
void write_output_match_search(const string& file, const vector<float>& matches);

/*!
 * \brief Read input for ROC and create shared_ptr to matches_type.
 *
 * \param file_true The file path containing true positive match data.
 * \param file_false The file path containing false positive match data.
 *
 * \return A shared_ptr to matches_type containing the read input data.
 */
shared_ptr<matches_type> read_input_ROC_tpir(const string& file_true, const string& file_false);

/*!
 * \brief Write ROC or TPIR output to a file and log the result.
 *
 * \param file The file path to write the output data.
 * \param fprs A vector of integers containing false positive rates.
 * \param tprs A vector of floats containing true positive rates.
 * \param prefixes A pair of strings representing prefixes for the output.
 * \param rank An integer representing the rank value (-1 for general results).
 */
void write_output_ROC_tpir(const string& file, const vector<int>& fprs, const vector<float>& tprs, const pair<string, string>& prefixes, int rank = -1);


//-----------------------------------------------------------------------Template Implementation-----------------------------------------------------------------------------------


class linux_scoped_mutex
{
private:
    sem_t* m_sem = nullptr;

public:
    linux_scoped_mutex(const string& name)
    {
        m_sem = sem_open(name.c_str(), O_CREAT, 0777, 1);
        sem_wait(m_sem);
    }

    ~linux_scoped_mutex()
    {
        sem_post(m_sem);
        sem_close(m_sem);
    }

    static void remove(const string& name)
    {
        sem_unlink(name.c_str());
    }
};

template<typename T_EyePair>
/*!
 * \brief Write extraction output and related information to files.
 *
 * \param output The in_out_desc_type containing the extraction output data.
 * \param file_desc The file path to write the descriptor data.
 * \param semaphore_name The name of the semaphore to lock during writing.
 * \param input_list A shared_ptr to input_list_type containing the input data (not used in this function).
 * \param fork_index The index of the forked process (not used in this function).
 * \param extra_output A vector of tuples containing additional output data (not used in this function).
 * \param file_extra The file path to write the additional output data (not used in this function).
 * \param fail_detect The fail_detect_type containing the failed detection data (not used in this function).
 * \param file_fail The file path to write the failed detection data (not used in this function).
 * \param debug_flag A boolean flag indicating whether to enable debug output (not used in this function).
 * \param file_debug The file path to write the debug output (not used in this function).
 * \param desc_size The descriptor size (not used in this function).
 */
void write_output_extract(const in_out_desc_type& output, const string& file_desc, const string& semaphore_name, shared_ptr<const input_list_type> input_list, size_t fork_index,
                  const vector< tuple<vector<string>, vector<T_EyePair>, vector<double>> >& extra_output, const string& file_extra, const fail_detect_type& fail_detect, const string& file_fail, bool debug_flag, const string& file_debug, uint desc_size)
{
    linux_scoped_mutex mtx(semaphore_name);
    {
        if(output.size() != (*input_list)[fork_index].size())
            throw runtime_error("invalid output size");

        FILE* desc_bin_file = fopen(file_desc.c_str(), "r+");

        if(!desc_bin_file)
            throw runtime_error("failed to open " + file_desc);

        size_t write_pos = 0;
        for(size_t i = 0; i < fork_index; i++)
            write_pos += (*input_list)[i].size() * (sizeof(int) + desc_size);
        fseek(desc_bin_file, static_cast<long>(write_pos), SEEK_SET);

        for(const auto& desc : output)
        {
            fwrite(&desc.first, sizeof(desc.first), 1, desc_bin_file);
            fwrite(desc.second.data(), desc.second.size() * sizeof(uint8_t), 1, desc_bin_file);
        }

        fclose(desc_bin_file);
        desc_bin_file = nullptr;

        if(debug_flag)
        {
            unique_ptr<ofstream> debug_info_stream = open_file_or_die<ofstream>(file_debug, ofstream::app);

            for(size_t i = 0; i < output.size(); i++)
            {
                *debug_info_stream << output[i].first << " ";

                for(const string& path : (*input_list)[fork_index][i].first)
                    *debug_info_stream << path << " ";

                for(uint8_t val : output[i].second)
                    *debug_info_stream << static_cast<int>(val) << " ";

                *debug_info_stream << endl;
            }
        }

        if(!extra_output.empty())
        {
            unique_ptr<ofstream> extract_info_stream = open_file_or_die<ofstream>(file_extra, ofstream::app);

            for(const auto& line : extra_output)
            {
                for(const string& path : get<0>(line))
                    *extract_info_stream << path << " ";

                for(const auto& eye_pair : get<1>(line))
                    *extract_info_stream << "{" << eye_pair.isLeftAssigned << " " << eye_pair.isRightAssigned << " " << eye_pair.xleft << " " << eye_pair.yleft << " " << eye_pair.xright << " " << eye_pair.yright << "} ";

                for(double val : get<2>(line))
                    *extract_info_stream << val << " ";

                *extract_info_stream << endl;
            }
        }

        if(!fail_detect.empty())
        {
            unique_ptr<ofstream> fail_detect_stream = open_file_or_die<ofstream>(file_fail, ofstream::app);

            for(const auto& line : fail_detect)
            {
                for(const string& path : line)
                    *fail_detect_stream << path << " ";

                *fail_detect_stream << endl;
            }
        }
    }
}
