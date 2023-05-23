#include <glog/logging.h>

#include "in_out_V.h"
#include "utils_V.h"

shared_ptr<in_out_desc_type> read_input_match(const string& file, uint desc_size, const string& counters_file)
{
    return read_input_match_search(file, desc_size, true, "", counters_file);
}


























































