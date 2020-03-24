#ifndef NOS3_SAMPLEDATAPOINT_HPP
#define NOS3_SAMPLEDATAPOINT_HPP

// Library Includes
#include <boost/shared_ptr.hpp>

// NOS Engine Includes
#include <ItcLogger/Logger.hpp>

// Simulator Includes
#include <sim_i_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    class SampleDataPoint : public SimIDataPoint
    {
    public:
        // Constructors
        SampleDataPoint(void);
        SampleDataPoint(std::uint32_t data);
        SampleDataPoint(const boost::shared_ptr<Sim42DataPoint> dp);

        // Deconstructors
        ~SampleDataPoint(void);

        // Accessors
        std::string to_string(void) const;
        uint32_t    get_sample_data(void) const {return _sample_data;}
    
    private:
        uint32_t    _sample_data;
    };
}

#endif
