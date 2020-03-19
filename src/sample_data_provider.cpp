#include <sample_data_provider.hpp>
#include <sample_data_point.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(SampleDataProvider,"BARPROVIDER");

    extern ItcLogger::Logger *sim_logger;

    SampleDataProvider::SampleDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("SampleDataProvider::SampleDataProvider:  Constructor executed");

        std::ostringstream oss;
        write_xml(oss, config);
        sim_logger->info("SampleDataProvider::SampleDataProvider:  "
            "configuration:\n%s", oss.str().c_str());

        sim_logger->trace("SampleDataProvider::SampleDataProvider:  Constructor exiting");
    }

    SampleDataProvider::~SampleDataProvider(void)
    {
        sim_logger->trace("SampleDataProvider::~SampleDataProvider:  Destructor executed");
    }

    boost::shared_ptr<SimIDataPoint> SampleDataProvider::get_data_point(void) const
    {
        sim_logger->info("SampleDataProvider::get_data_point:  Executed");

        SampleDataPoint *msdp = new SampleDataPoint();
        return boost::shared_ptr<SimIDataPoint>(msdp);
    }
}
