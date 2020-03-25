#ifndef NOS3_SAMPLEDATAPROVIDER_HPP
#define NOS3_SAMPLEDATAPROVIDER_HPP

// Library Includes
#include <boost/property_tree/xml_parser.hpp>

// NOS Engine Includes
#include <ItcLogger/Logger.hpp>

// Simulator Includes
#include <sample_data_point.hpp>

namespace Nos3
{
    class SampleDataProvider : public SimIDataProvider
    {
    public:
        // Constructors
        SampleDataProvider(const boost::property_tree::ptree& config);

        // Deconstructors
        ~SampleDataProvider(void);

        // Accessors
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        mutable double _request_count;
    };
}

#endif
