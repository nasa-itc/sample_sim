#ifndef NOS3_SAMPLE42DATAPROVIDER_HPP
#define NOS3_SAMPLE42DATAPROVIDER_HPP

// Library Includes
#include <boost/property_tree/xml_parser.hpp>

// NOS Engine Includes
#include <ItcLogger/Logger.hpp>

// Simulator Includes
#include <sample_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    class Sample42DataProvider : public SimData42SocketProvider
    {
    public:
        Sample42DataProvider(const boost::property_tree::ptree& config);
        ~Sample42DataProvider(void);
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;
    };
}

#endif
