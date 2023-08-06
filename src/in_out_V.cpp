#include <glog/logging.h>

#include "in_out_V.h"
#include "utils_V.h"

/*!
 * \brief Read input for match and create shared_ptr to in_out_desc_type.
 *
 * \param file The file path containing the input data.
 * \param desc_size The descriptor size.
 * \param counters_file The file path to the counters data (not used in this function).
 * \return A shared_ptr to in_out_desc_type containing the read input data.
 */
shared_ptr<in_out_desc_type> read_input_match(const string& file, uint desc_size, const string& counters_file)
{
    return read_input_match_search(file, desc_size, true, "", counters_file);
}


























































