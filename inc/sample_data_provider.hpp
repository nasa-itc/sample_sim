#ifndef NOS3_SAMPLEDATAPROVIDER_HPP
#define NOS3_SAMPLEDATAPROVIDER_HPP

#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class SampleDataProvider : public SimIDataProvider
    {
    public:
        SampleDataProvider(const boost::property_tree::ptree& config);
        ~SampleDataProvider(void);
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;
    };
}

#endif
