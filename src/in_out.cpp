#include "in_out.h"

class list_processor
{

private:
    shared_ptr<input_list_type> m_list;
    size_t m_backet_size;
    size_t m_extended_count;

public:
    list_processor(shared_ptr<input_list_type> list, size_t size, uint count_proc) : m_list(list)
    {
        m_backet_size = size / count_proc;
        m_extended_count = size % count_proc;
    }

    void add(const pair<vector<string>, int>& el)
    {
        size_t curr_backet_size = 0;

        if(m_list->size() <= m_extended_count)
            curr_backet_size = m_backet_size + 1;
        else
            curr_backet_size = m_backet_size;

        if(m_list->empty() || m_list->back().size() == curr_backet_size)
            m_list->emplace_back();

        m_list->back().push_back(el);
    }
};

shared_ptr<input_list_type> read_input_extract(const string& file, uint count_proc, size_t& count_templ)
{
    unique_ptr<ifstream> input_stream = open_file_or_die<ifstream>(file);

    unordered_map<int, int> keys;
    unordered_multimap<int, string> list_agg;
    vector<pair<int, string>> list;

    while(true)
    {
        string line;
        getline(*input_stream, line);

        if(input_stream->fail())
            break;

        stringstream line_stream(line);

        string path;
        int class_id = 0, template_id = 0;

        line_stream >> path >> class_id >> template_id;

        if(path.empty())
            break;

        if(class_id < 0)
             throw logic_error("find negative class id in labels: " + to_string(class_id));
        if(template_id < 0)
             throw logic_error("find negative template id in labels: " + to_string(template_id));

        if(template_id)
        {
            if(keys.count(template_id))
            {
                if(keys[template_id] != class_id)
                    throw logic_error("find template with different class ids");
            }
            else
                keys[template_id] = class_id;

            list_agg.emplace(template_id, move(path));
        }
        else
            list.emplace_back(class_id, move(path));
    }

    auto retval = make_shared<input_list_type>();

    list_processor processor(retval, keys.size() + list.size(), count_proc);

    for(const auto& one_pair : list)
        processor.add({{one_pair.second}, one_pair.first});

    for(const auto& key : keys)
    {
        auto range = list_agg.equal_range(key.first);

        vector<string> paths;
        for(auto it = range.first; it != range.second; it++)
            paths.push_back(it->second);

        processor.add({paths, key.second});
    }

    LOG(INFO) << "count images: " << list_agg.size() + list.size();
    LOG(INFO) << "count templates: " << keys.size() + list.size();
    LOG(INFO) << "count proc: " << count_proc;
    LOG(INFO) << "templates per proc: " << (retval->empty() ? 0 : (*retval)[0].size());

    if(retval->empty())
        throw runtime_error("empty input list in file: " + file);

    count_templ = keys.size() + list.size();

    return retval;
}

void write_manifest(const string& out_file, const string& desc_file, uint desc_size)
{
    shared_ptr<in_out_desc_type> descriptors = read_input_match_search(desc_file, desc_size, false, "db");
    unique_ptr<ofstream> manifest_stream = open_file_or_die<ofstream>(out_file);

    size_t counter = 0;
    for(const auto& desc : *descriptors)
    {
        if(desc.first >= 0)
            *manifest_stream << to_string(counter) + "_" + to_string(desc.first) << " " << desc_size << " " << sizeof(int) + counter * (desc_size + sizeof(int)) << endl;

        counter++;
    }
}

shared_ptr<in_out_desc_type> read_input_match_search(const string& file, uint desc_size, bool match_log, const string& log_prefix, const string& counters_file)
{
    unique_ptr<ifstream> desc_binstream = open_file_or_die<ifstream>(file, ifstream::binary);

    auto descriptors = make_shared<in_out_desc_type>();

    size_t refusal_count = 0;
    while(true)
    {
        descriptors->emplace_back();
        auto& desc = descriptors->back();
        desc.second.resize(desc_size);

        desc_binstream->read(reinterpret_cast<char*>(&desc.first), sizeof(int));
        desc_binstream->read(reinterpret_cast<char*>(desc.second.data()), desc_size);

        if(desc_binstream->fail())
        {
            descriptors->pop_back();
            break;
        }

        if(desc.first < 0)
            refusal_count++;
    }

    size_t desc_count = descriptors->size();

    if(!desc_count)
        throw runtime_error("empty descriptors file: " + file);

    LOG(INFO) << (log_prefix.empty() ? "" : log_prefix + " ") << "descriptors count: " << desc_count;
    if(match_log)
        LOG(INFO) << "match count: " << desc_count * (desc_count - 1) / 2;
    LOG(INFO) << (log_prefix.empty() ? "" : log_prefix + " ") << "REFUSAL count: " << refusal_count;

    if(!counters_file.empty())
    {
         unique_ptr<ofstream> counters_stream = open_file_or_die<ofstream>(counters_file);
         *counters_stream << desc_count << endl << refusal_count;
    }

    return descriptors;
}

void write_output_match_search(const string& file, const vector<float>& matches)
{
    unique_ptr<ofstream> matches_stream = open_file_or_die<ofstream>(file, ofstream::binary);
    matches_stream->write(reinterpret_cast<const char*>(matches.data()), static_cast<long>(matches.size() * sizeof(float)));
}

void match_data_to_vector(const string& file, vector<float>& matches)
{
    unique_ptr<ifstream> matches_stream = open_file_or_die<ifstream>(file, ifstream::binary | ifstream::ate);
    size_t file_size = static_cast<size_t>(matches_stream->tellg());
    matches.resize(file_size/sizeof(float));
    matches_stream->seekg(0);
    matches_stream->read(reinterpret_cast<char*>(matches.data()), static_cast<long>(file_size));
}

shared_ptr<matches_type> read_input_ROC_tpir(const string& file_true, const string& file_false)
{
    auto matches = make_shared<matches_type>();

    match_data_to_vector(file_true, matches->first);
    match_data_to_vector(file_false, matches->second);

    return matches;
}

void write_output_ROC_tpir(const string& file, const vector<int>& fprs, const vector<float>& tprs, const pair<string, string>& prefixes, int rank)
{
    if(fprs.size() != tprs.size())
        throw runtime_error("wrong calc ROC/TPIR param");

    stringstream buf;

    unique_ptr<ofstream> acc_stream = open_file_or_die<ofstream>(file);

    if(rank != -1)
    {
        if(rank)
            buf << "Rank " << rank << ":";
        else
            buf << "General:";
    }
    buf << endl;

    for(size_t i = 0; i < fprs.size(); i++)
    {
        string tpr_str = tprs[i] < 0 ? "none" : to_string_form(tprs[i], 3);
        buf << prefixes.first << " - 10^" << fprs[i] * -1 << ", " << prefixes.second << " - " << tpr_str << endl;
        *acc_stream << fprs[i] * -1 << " " << tpr_str << endl;
    }

    buf << endl;

    LOG(INFO) << buf.rdbuf();
}









































