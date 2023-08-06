#pragma once

#include "utils.h"

using namespace std;

/*!
 * \brief Parse command-line arguments and retrieve parameters.
 *
 * \param argc The number of command-line arguments.
 * \param argv An array of character pointers containing the command-line arguments.
 *
 * \return A map of parameters with their associated command-line arguments.
 */
params_type parse_cmd_line(int argc, char* argv[]);

/*!
 * \brief Print all program information and options.
 *
 * \param params The map of parameters containing the program options.
 */
void print_all(params_type& params);
