#ifndef NOS3_SAMPLEHARDWAREMODEL_HPP
#define NOS3_SAMPLEHARDWAREMODEL_HPP

#include <sim_i_hardware_model.hpp>
#include <Client/Bus.hpp>
#include <Uart/Client/Uart.hpp>

#include <atomic>

namespace Nos3
{
    class SampleHardwareModel : public SimIHardwareModel
    {
    public:
        SampleHardwareModel(const boost::property_tree::ptree& config);
        ~SampleHardwareModel(void);
        void run(void);
        void uart_read_callback(const uint8_t *buf, size_t len);
        void command_callback(NosEngine::Common::Message msg);
    private:
        std::atomic<bool>                       _keep_running;
        SimIDataProvider*                       _sdp;
        std::unique_ptr<NosEngine::Client::Bus> _time_bus;
        std::unique_ptr<NosEngine::Uart::Uart>  _uart_connection;
    };
}

#endif
