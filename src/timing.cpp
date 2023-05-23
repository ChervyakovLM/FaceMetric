#include <cmath>
#include <algorithm>
#include <numeric>

#include "timing.h"

timing::timing(bool extended) : m_acc(0), m_call_counter(0), m_extended(extended)
{

}

void timing::start()
{
    m_tstart = high_resolution_clock::now();
}

nanoseconds timing::stop()
{
    const high_resolution_clock::time_point tstop = high_resolution_clock::now();
    const auto interval = tstop - m_tstart;

    m_acc += interval;
    m_call_counter++;

    if(m_extended)
        m_values.push_back(interval);

    return interval;
}

nanoseconds timing::get_average()
{
    if(m_call_counter)
    {
        const auto average = m_acc / m_call_counter;
        m_acc = nanoseconds(0);
        m_call_counter = 0;
        return average;
    }
    else
        return nanoseconds(-1);
}

timing::extended_info_type<nanoseconds> timing::get_extended_info(float percentile)
{
    if(m_extended && m_values.size() > 1 && (percentile >= 0 && percentile <= 1))
    {
        extended_info_type<nanoseconds> info;

        info.percentile = percentile;

        const size_t pos = static_cast<size_t>(ceil(m_values.size() * percentile));
        nth_element(m_values.begin(), m_values.begin() + static_cast<long>(pos - 1), m_values.end());

        info.percentile_val = m_values[pos - 1];

        const auto minmax_pair = minmax_element(m_values.begin(), m_values.end());
        info.min_val = *(minmax_pair.first);
        info.max_val = *(minmax_pair.second);

        const double mean_val = static_cast<double>(accumulate(m_values.begin(), m_values.end(), nanoseconds(0)).count()) / m_values.size();
        double sum = 0;
        for(const auto& val : m_values)
        {
            const double diff = val.count() - mean_val;
            sum += diff * diff;
        }
        info.std_dev = nanoseconds(static_cast<long>(sqrt(sum / (m_values.size() - 1))));

        m_values.clear();
        return info;
    }
    else
        return {0, nanoseconds(-1), nanoseconds(-1), nanoseconds(-1), nanoseconds(-1)};
}
