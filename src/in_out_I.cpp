#include "in_out_I.h"

/*!
 * \brief Read input for search and create shared_ptr to in_out_desc_type.
 *
 * \param file The file path containing the input data.
 * \param desc_size The descriptor size.
 * \param log_prefix The log prefix to use in case of logging.
 *
 * \return A shared_ptr to in_out_desc_type containing the read input data.
 */
shared_ptr<in_out_desc_type> read_input_search(const string& file, uint desc_size, const string& log_prefix)
{
    return read_input_match_search(file, desc_size, false, log_prefix);
}

/*!
 * \brief Write search ranks output to files in the specified directory.
 *
 * \param output_dir The directory path to write the output files.
 * \param matches_true_ranks A vector of pairs representing true positive ranks and their corresponding data.
 * \param matches_false A vector containing false positive data.
 */
void write_output_search_ranks(const string& output_dir, const vector<pair<uint, vector<float>>>& matches_true_ranks, const vector<float>& matches_false)
{
    for(size_t i =0; i < matches_true_ranks.size() - 1; i++)
        write_output_match_search(output_dir + "/matches_true_r" + to_string(matches_true_ranks[i].first) + ".bin", matches_true_ranks[i].second);

    write_output_match_search(output_dir + "/matches_true.bin", matches_true_ranks.back().second);
    write_output_match_search(output_dir + "/matches_false.bin", matches_false);
}

/*!
 * \brief Read input from the remove_file and return a vector of strings.
 *
 * \param remove_file The file path containing the data to read.
 *
 * \return A vector of strings containing the read data from the remove_file.
 */
vector<string> read_input_remove(const string& remove_file)
{
    unique_ptr<ifstream> stream = open_file_or_die<ifstream>(remove_file);

    string id_str;
    vector<string> remove_list;
    while(*stream >> id_str)
    {
        remove_list.push_back(move(id_str));
    }

    LOG(INFO) << "remove count: " << remove_list.size();

    return remove_list;
}
