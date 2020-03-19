#ifndef NOS3_SAMPLEDATAPOINT_HPP
#define NOS3_SAMPLEDATAPOINT_HPP

#include <sim_i_data_point.hpp>

namespace Nos3
{
    class SampleDataPoint : public SimIDataPoint
    {
    public:
        SampleDataPoint(void);
        ~SampleDataPoint(void);
        std::string to_string(void) const;
    };
}

#endif
