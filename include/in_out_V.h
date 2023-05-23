#pragma once

#include <string>

#include "utils.h"
#include "in_out.h"

using namespace std;

shared_ptr<in_out_desc_type> read_input_match(const string& file, uint desc_size, const string& counters_file = "");
