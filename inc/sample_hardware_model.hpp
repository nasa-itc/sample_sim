#ifndef NOS3_SAMPLEHARDWAREMODEL_HPP
#define NOS3_SAMPLEHARDWAREMODEL_HPP

// Library Includes
#include <atomic>
#include <map>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>

// NOS Engine Includes
#include <ItcLogger/Logger.hpp>
#include <Client/Bus.hpp>
#include <Uart/Client/Uart.hpp>

// Simulator Includes
#include <sample_42_data_provider.hpp>
#include <sample_data_point.hpp>
#include <sim_i_hardware_model.hpp>

namespace Nos3
{
    class SampleHardwareModel : public SimIHardwareModel
    {
    public:
        SampleHardwareModel(const boost::property_tree::ptree& config);
        ~SampleHardwareModel(void);
        void run(void);
        void command_callback(NosEngine::Common::Message msg);
        std::vector<uint8_t> determine_response_for_request(const std::vector<uint8_t>& in_data);
        void send_periodic_data(NosEngine::Common::SimTime time);
        void stream_data(const SampleDataPoint& data_point, std::vector<uint8_t>& out_data);
        void uart_read_callback(const uint8_t *buf, size_t len);
    
    private:
        std::atomic<bool>                       _keep_running;
        SimIDataProvider*                       _sample_dp;
        std::unique_ptr<NosEngine::Client::Bus> _time_bus;

        // Hardware Protocol
        std::unique_ptr<NosEngine::Uart::Uart>  _uart_connection;

        // Hardware Configuration
        std::uint32_t                           _counter;
        double                                  _init_time_seconds;
        std::uint32_t                           _millisecond_stream_delay;
        double                                  _second_stream_delay;

        // Streaming
        double                                  _next_time;
        std::vector<uint8_t>                    _streaming_data;

    };
}

#endif
