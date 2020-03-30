#include <sample_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    SampleDataPoint::SampleDataPoint(void)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Empty constructor executed");
    }

    SampleDataPoint::SampleDataPoint(double data)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  Defined constructor executed");

        // Option to do calculations on provided data at this point
        _sample_data.push_back(data * 2);
    }

    SampleDataPoint::SampleDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp)
    {
        sim_logger->trace("SampleDataPoint::SampleDataPoint:  42 Constructor executed");

        // Declare 42 telemetry string prefix
        // 42 variables defined in `42/Include/42types.h`
        // 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
        std::ostringstream MatchString;
        MatchString << "SC[" << spacecraft << "].svb = ";
        size_t MSsize = MatchString.str().size();

        // Parse 42 telemetry
        std::vector<std::string> lines = dp->get_lines();
        try 
        {
            for (int i = 0; i < lines.size(); i++) 
            {
                // Compare prefix
                if (lines[i].compare(0, MSsize, MatchString.str()) == 0) 
                {
                    size_t found = lines[i].find_first_of("=");
                    // Parse line
                    std::istringstream iss(lines[i].substr(found+1, lines[i].size()-found-1));
                    _sample_data.clear();
                    for (std::string s; iss >> s; )
                    {
                        _sample_data.push_back(std::stod(s));
                    }
                    sim_logger->trace("SampleDataPoint::SampleDataPoint:  Parsed svb = %f %f %f", _sample_data[0], _sample_data[1], _sample_data[2]);
                }
            }
        } 
        catch(const std::exception& e) 
        {
            sim_logger->error("SampleDataPoint::SampleDataPoint:  Parsing exception %s", e.what());
        }
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
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " Sample Data: "
           << _sample_data[0];

        return ss.str();
    }
}
