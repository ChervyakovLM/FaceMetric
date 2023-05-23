#include "in_out_I.h"

shared_ptr<in_out_desc_type> read_input_search(const string& file, uint desc_size, const string& log_prefix)
{
    return read_input_match_search(file, desc_size, false, log_prefix);
}

void write_output_search_ranks(const string& output_dir, const vector<pair<uint, vector<float>>>& matches_true_ranks, const vector<float>& matches_false)
{
    for(size_t i =0; i < matches_true_ranks.size() - 1; i++)
        write_output_match_search(output_dir + "/matches_true_r" + to_string(matches_true_ranks[i].first) + ".bin", matches_true_ranks[i].second);

    write_output_match_search(output_dir + "/matches_true.bin", matches_true_ranks.back().second);
    write_output_match_search(output_dir + "/matches_false.bin", matches_false);
}

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
