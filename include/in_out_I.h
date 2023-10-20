#pragma once

#include "in_out.h"

/*!
 * \brief Read input for search and create shared_ptr to in_out_desc_type.
 *
 * \param file The file path containing the input data.
 * \param desc_size The descriptor size.
 * \param log_prefix The log prefix to use in case of logging.
 *
 * \return A shared_ptr to in_out_desc_type containing the read input data.
 */
shared_ptr<in_out_desc_type> read_input_search(const string& file, uint desc_size, const string& log_prefix = "");

/*!
 * \brief Write search ranks output to files in the specified directory.
 *
 * \param output_dir The directory path to write the output files.
 * \param matches_true_ranks A vector of pairs representing true positive ranks and their corresponding data.
 * \param matches_false A vector containing false positive data.
 */
void write_output_search_ranks(const string& output_dir, const vector<pair<uint, vector<float>>>& matches_true_ranks, const vector<float>& matches_false);

/*!
 * \brief Read input from the remove_file and return a vector of strings.
 *
 * \param remove_file The file path containing the data to read.
 *
 * \return A vector of strings containing the read data from the remove_file.
 */
vector<string> read_input_remove(const string& remove_file);

/*!
 * \brief Write std::map to 2 files.
 *
 * \param output_dir The file path containing the files to write.
 * \param templates The map of templates.
 */
void WriteFiles(const std::string& output_dir, const std::map<std::string, std::vector<uint8_t>>& templates);

/*!
 * \brief Read 2 files to std::map.
 *
 * \param input_dir The file path containing the files to read.
 * \param templates The map of templates.
 */
void ReadTextBinaryFromFiles(const std::string &input_dir, std::map<std::string, std::vector<uint8_t>>& templates);
