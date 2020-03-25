#ifndef NOS3_SAMPLEDATAPOINT_HPP
#define NOS3_SAMPLEDATAPOINT_HPP

// Library Includes
#include <boost/shared_ptr.hpp>
#include <string>

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
        SampleDataPoint(double data);
        SampleDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        // Deconstructors
        ~SampleDataPoint(void);

        // Accessors
        std::string to_string(void) const;
        double      get_sample_data(void) const {return _sample_data[0];}
    
    private:
        mutable std::vector<double> _sample_data;
    };
}

#endif
