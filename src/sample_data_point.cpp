#include <sample_data_point.hpp>

#include <ItcLogger/Logger.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    SampleDataPoint::SampleDataPoint(int16_t spacecraft, int16_t imu, const boost::shared_ptr<Sim42DataPoint> dp) : _sc(spacecraft), _imu(imu), _dp(*dp), _not_parsed(true), SimIDataPoint()
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Constructor executed");
        _bvb.resize(3);
    }

    SampleDataPoint::~SampleDataPoint(void)
    {
        sim_logger->trace("SampleDataPoint::~SampleDataPoint:  Destructor executed");
    }

    std::string SampleDataPoint::to_string(void) const
    {
        sim_logger->info("SampleDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setprecision(4) << std::setfill(' ');
        ss << "Sample Data Point: "
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " Linear bvb: "
           << _bvb[0] << ","
           << _bvb[1] << ","
           << _bvb[2] ;

        return ss.str();
    }
}
