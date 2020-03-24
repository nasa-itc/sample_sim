#include <sample_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    SampleDataPoint::SampleDataPoint(void)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Empty constructor executed");
    }

    SampleDataPoint::SampleDataPoint(uint32_t data)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Defined constructor executed");
        _sample_data = data * 2;
    }

    SampleDataPoint::SampleDataPoint(const boost::shared_ptr<Sim42DataPoint> dp)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  42 Constructor executed");
    }

    SampleDataPoint::~SampleDataPoint(void)
    {
        sim_logger->trace("SampleDataPoint::~SampleDataPoint:  Destructor executed");
    }

    std::string SampleDataPoint::to_string(void) const
    {
        sim_logger->trace("SampleDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Sample Data Point: ";
        ss << std::setprecision(std::numeric_limits<uint32_t>::digits10); // Full uint32_t precision
        ss << " Sample Data: "
           << _sample_data;

        return ss.str();
    }
}
