#include <sample_hardware_model.hpp>
#include <sample_data_provider.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(SampleHardwareModel,"FOOHARDWARE");

    extern ItcLogger::Logger *sim_logger;

    SampleHardwareModel::SampleHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _keep_running(true)
    {
        sim_logger->trace("SampleHardwareModel::SampleHardwareModel:  Constructor executing");

        // Here's how to write out the config data passed
        //std::ostringstream oss;
        //write_xml(oss, config);
        //sim_logger->info("SampleHardwareModel::SampleHardwareModel:  "
        //    "configuration:\n%s", oss.str().c_str());

        // Here's how to get a time node to get time from
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");

        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) 
            {
                if (v.second.get("type", "").compare("time") == 0) 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    break;
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));

        // Here's how to get a UART node to communicate with
        std::string bus_name = "usart_0";
        int node_port = 0;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
            {
                if (v.second.get("type", "").compare("usart") == 0) 
                {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break;
                }
            }
        }
        _uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "foosim"), connection_string,
            bus_name));
        _uart_connection->open(node_port);
        _uart_connection->set_read_callback(
            std::bind(&SampleHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));

        // Here's how to get a data provider
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "BARPROVIDER");
        _sdp = SimDataProviderFactory::Instance().Create(dp_name, config);

        sim_logger->trace("SampleHardwareModel::SampleHardwareModel:  Time node, UART node, data provider created; constructor exiting");
    }

    SampleHardwareModel::~SampleHardwareModel(void)
    {
        sim_logger->trace("SampleHardwareModel::SampleHardwareModel:  Destructor executing");
        delete _sdp; // Clean up the data provider we got
        _time_bus.reset(); // Must reset the time bus so the unique pointer does not try to delete the hub.  Do not destroy the time node, the bus will do it
        _uart_connection->close();
    }

    void SampleHardwareModel::run(void)
    {
        int i = 0;
        boost::shared_ptr<SimIDataPoint> dp;
        while(_keep_running) 
        {
            sim_logger->info("SampleHardwareModel::run:  Loop count %d, time %f", i++,
                _absolute_start_time + (double(_time_bus->get_time() * _sim_microseconds_per_tick)) / 1000000.0);
            dp = _sdp->get_data_point();
            sleep(5);
        }
    }

    void SampleHardwareModel::uart_read_callback(const uint8_t *buf, size_t len)
    {
        // Get the data out of the message bytes
        std::vector<uint8_t> in_data(buf, buf + len);
        sim_logger->debug("SampleHardwareModel::uart_read_callback:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data in a man readable format

        // Figure out how to respond (possibly based on the in_data)
        std::vector<uint8_t> out_data = in_data; // Just echo

        // Ship the message bytes off
        sim_logger->debug("SampleHardwareModel::uart_read_callback:  REPLY   %s\n",
            SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str()); // log data in a man readable format

        _uart_connection->write(&out_data[0], out_data.size());
    }

    void SampleHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Here's how to get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("SampleHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "SampleHardwareModel::command_callback:  INVALID COMMAND! (Try STOP FOOSIM)";
        boost::to_upper(command);
        if (command.compare("STOP FOOSIM") == 0) 
        {
            _keep_running = false;
            response = "SampleHardwareModel::command_callback:  STOPPING FOOSIM";
        }

        // Here's how to send a reply
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }
}
