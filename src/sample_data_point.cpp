#include <sample_data_point.hpp>

#include <ItcLogger/Logger.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    SampleDataPoint::SampleDataPoint(void) : SimIDataPoint()
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Constructor executed");
    }

    SampleDataPoint::~SampleDataPoint(void)
    {
        sim_logger->trace("SampleDataPoint::~SampleDataPoint:  Destructor executed");
    }

    std::string SampleDataPoint::to_string(void) const
    {
        sim_logger->info("SampleDataPoint::to_string:  Executed");
        return "A SampleDataPoint";
    }
}
