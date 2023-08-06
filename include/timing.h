#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace std::chrono;

/* usage:
 *
    cout << duration_to_string(interval) << endl;
    cout << duration_to_string(duration<double, micro>(interval), 0) << endl;
    cout << duration_to_string(duration<double, milli>(interval), 1) << endl;
    cout << duration_to_string(duration<double, sec_t>(interval), 3) << endl;
    cout << duration_to_string(duration<double, min_t>(interval)) << endl;
*/

class timing
{

public:
    template<typename T_time>
    struct extended_info_type
    {
        float percentile;
        T_time percentile_val;
        T_time min_val;
        T_time max_val;
        T_time std_dev;
    };

    timing(bool extended = false);
    void start();
    nanoseconds stop();
    nanoseconds get_average();
    extended_info_type<nanoseconds> get_extended_info(float percentile);

    template<typename T_counter_type, typename T_ratio>
    static extended_info_type<duration<T_counter_type, T_ratio>> extended_info_cast(const extended_info_type<nanoseconds>& info);

private:
    high_resolution_clock::time_point m_tstart;
    nanoseconds m_acc;
    uint64_t m_call_counter;
    vector<nanoseconds> m_values;
    bool m_extended;
};

typedef ratio<1, 1> sec_t;
typedef ratio<60, 1> min_t;
typedef ratio<3600, 1> hours_t;

template<typename T_counter_type, intmax_t num, intmax_t den>
string duration_to_string(const duration<T_counter_type, ratio<num, den>>& interval, int precision = -1);


//-----------------------------------------------------------------------Template Implementation-----------------------------------------------------------------------------------


template<typename T_counter_type, typename T_ratio>
/*!
 * \brief Convert extended_info_type<nanoseconds> to extended_info_type<duration<T_counter_type, T_ratio>>.
 *
 * \param info The extended_info_type<nanoseconds> object to convert.
 *
 * \return The converted extended_info_type<duration<T_counter_type, T_ratio>> object.
 */
timing::extended_info_type<duration<T_counter_type, T_ratio>> timing::extended_info_cast(const extended_info_type<nanoseconds>& info)
{
    extended_info_type<duration<T_counter_type, T_ratio>> retval;

    retval.percentile = info.percentile;
    retval.percentile_val = duration<T_counter_type, T_ratio>(info.percentile_val);
    retval.min_val = duration<T_counter_type, T_ratio>(info.min_val);
    retval.max_val = duration<T_counter_type, T_ratio>(info.max_val);
    retval.std_dev = duration<T_counter_type, T_ratio>(info.std_dev);

    return retval;
}

template<typename T_counter_type, intmax_t num, intmax_t den>
/*!
 * \brief Convert a duration to a string representation with a specified precision.
 *
 * \param interval The duration to convert to a string.
 * \param precision The desired precision for the string representation.
 *
 * \return A string representation of the duration with the specified precision and interval type.
 */
string duration_to_string(const duration<T_counter_type, ratio<num, den>>& interval, int precision)
{
    string interval_type;

    switch (den)
    {

    case 1:
        switch (num)
        {

        case 1:
            interval_type = "seconds";
            break;

        case 60:
            interval_type = "minutes";
            break;

        case 3600:
            interval_type = "hours";
            break;

        default:
            interval_type = "unknown type";
            break;
        }
        break;

    case 1000:
        interval_type = "milliseconds";
        break;

    case 1000 * 1000:
        interval_type = "microseconds";
        break;

    case 1000 * 1000 * 1000:
        interval_type = "nanoseconds";
        break;

    default:
        interval_type = "unknown type";
        break;
    }

    stringstream buf;

    if(interval.count() < 0)
        buf << "none";
    else
    {
        if(precision >= 0)
            buf << fixed << setprecision(precision);
        buf << interval.count();
    }

    buf << " " << interval_type;

    return buf.str();
}































