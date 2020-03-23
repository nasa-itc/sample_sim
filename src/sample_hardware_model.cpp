#include <sample_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(SampleHardwareModel,"SAMPLE");

    extern ItcLogger::Logger *sim_logger;

    SampleHardwareModel::SampleHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _keep_running(true)
    {
        // The sim logger prints to both the terminal and to a file
        sim_logger->trace("SampleHardwareModel::SampleHardwareModel:  Constructor executing");

        // Initialize configuration values to hard coded defaults
        std::string time_bus_name = "command";
        std::string bus_name = "usart_29";
        int node_port = 0;
        _counter = 0;
        _data = 0xBAADC0DE;
        _millisecond_stream_delay = 1000;

        // Get the time connection string
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");

        // Loop through the configuration file
        if (config.get_child_optional("hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) 
            {
                // Find the time information
                if (v.second.get("type", "").compare("time") == 0) 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                }

                // Find the uart information
                if (v.second.get("type", "").compare("usart") == 0) 
                {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                }

                // Find the periodic information
                if (v.second.get("type", "").compare("period") == 0) 
                {
                    _millisecond_stream_delay = v.second.get("ms-period", _millisecond_stream_delay);
                }
            }
        }

        // Reset time connection
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));

        // Add callback for streaming data
        _time_bus->add_time_tick_callback(std::bind(&SampleHardwareModel::send_periodic_data, this, std::placeholders::_1));
        
        // Reset and open uart
        _uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "sample_sim"), connection_string,
            bus_name));
        _uart_connection->open(node_port);
        _uart_connection->set_read_callback(
            std::bind(&SampleHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));

        // Setup the data provider
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "SAMPLE_PROVIDER");
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
            //dp = _sdp->get_data_point();
            sleep(5);
        }
    }


    void SampleHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Here's how to get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("SampleHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "SampleHardwareModel::command_callback:  INVALID COMMAND! (Try STOP SAMPLE)";
        boost::to_upper(command);
        if (command.compare("STOP SAMPLE") == 0) 
        {
            _keep_running = false;
            response = "SampleHardwareModel::command_callback:  STOPPING SAMPLE";
        }

        // Here's how to send a reply
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }


    void SampleHardwareModel::send_periodic_data(NosEngine::Common::SimTime time)
    {
        std::vector<uint8_t> data;

        double abs_time = _absolute_start_time + (double(time * _sim_microseconds_per_tick)) / 1000000.0;

        for (std::map<std::string, boost::tuple<double, double>>::iterator it = _periodic_logs.begin(); it != _periodic_logs.end(); it++) 
        {
            boost::tuple<double, double> value = it->second;

            double prev_time = boost::tuples::get<0>(value);

            double next_time = prev_time + _millisecond_stream_delay - (_sim_microseconds_per_tick / 1000000.0) / 2; // within half a tick time period

            if (next_time < abs_time) 
            {   // Time to send more data
                it->second = boost::tuple<double, double>(abs_time, _millisecond_stream_delay);
                std::map<std::string, get_log_data_func>::iterator search = _get_log_data_map.find(it->first);
                if (search != _get_log_data_map.end()) 
                {
                    get_log_data_func f = search->second;
                }
            }
        }
    }


    void SampleHardwareModel::uart_read_callback(const uint8_t *buf, size_t len)
    {
        // Retrieve data and log received data in man readable format
        std::vector<uint8_t> in_data(buf, buf + len);
        sim_logger->debug("SampleHardwareModel::uart_read_callback:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        // Check if message is incorrect size
        if (in_data.size() != 9)
        {
            sim_logger->debug("SampleHardwareModel::uart_read_callback:  Invalid command size of %d received!", in_data.size());
            return;
        }

        // Check header - 0xDEAD
        if ((in_data[0] != 0xDE) || (in_data[1] !=0xAD))
        {
            sim_logger->debug("SampleHardwareModel::uart_read_callback: Header incorrect!");
            return;
        }

        // Check trailer - 0xBEEF
        if ((in_data[7] != 0xBE) || (in_data[8] !=0xEF))
        {
            sim_logger->debug("SampleHardwareModel::uart_read_callback: Trailer incorrect!");
            return;
        }

        // Process command type
        switch (in_data[2])
        {
            case 1:
                sim_logger->debug("SampleHardwareModel::uart_read_callback: Configuration command received!");
                _millisecond_stream_delay = (in_data[3] << 24) +
                                            (in_data[4] << 16) +
                                            (in_data[5] << 8 ) +
                                            (in_data[6]);
                break;

            case 2:
                sim_logger->debug("SampleHardwareModel::uart_read_callback: Other command received!");
                break;

            default:
                sim_logger->debug("SampleHardwareModel::uart_read_callback: Unused command received!");
                break;
        }

        // Prepare to echo back valid command
        std::vector<uint8_t> out_data = in_data;

        // Log reply data in man readable format and ship the message bytes off
        sim_logger->debug("SampleHardwareModel::uart_read_callback:  REPLY   %s\n",
            SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
        _uart_connection->write(&out_data[0], out_data.size());
    }
}
