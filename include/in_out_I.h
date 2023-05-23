#pragma once

#include "in_out.h"

shared_ptr<in_out_desc_type> read_input_search(const string& file, uint desc_size, const string& log_prefix = "");
void write_output_search_ranks(const string& output_dir, const vector<pair<uint, vector<float>>>& matches_true_ranks, const vector<float>& matches_false);
vector<string> read_input_remove(const string& remove_file);
