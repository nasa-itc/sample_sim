#include <sample_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(SampleHardwareModel,"SAMPLE");

    extern ItcLogger::Logger *sim_logger;

    SampleHardwareModel::SampleHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _keep_running(true)
    {
        // The sim logger prints to both the terminal and to a file
        sim_logger->trace("SampleHardwareModel::SampleHardwareModel:  Constructor executing");

        // Initialize configuration values to hard coded defaults in case configuration file is incomplete
        std::string time_bus_name = "command";
        std::string bus_name = "usart_29";
        int node_port = 29;
        _counter = 0;
        _data = 0xBAADC0DE;
        _init_time_seconds = 5.0;
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
                    sim_logger->info("SampleHardwareModel::SampleHardwareModel:  Found time info!");
                }
            }
        }

        // Loop through the configuration file
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
            {
                // Find the uart information
                if (v.second.get("type", "").compare("usart") == 0) 
                {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    sim_logger->info("SampleHardwareModel::SampleHardwareModel:  Found uart info!");
                }

                // Find the initialization time information
                if (v.second.get("type", "").compare("period") == 0) 
                {
                    _init_time_seconds = v.second.get("init-time-seconds", _init_time_seconds);                    
                    _millisecond_stream_delay = v.second.get("ms-period", _millisecond_stream_delay);
                    sim_logger->info("SampleHardwareModel::SampleHardwareModel:  Found period info!");
                }
            }
        }

        // Reset time connection
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));

        // Add callback for streaming data
        _time_bus->add_time_tick_callback(std::bind(&SampleHardwareModel::send_periodic_data, this, std::placeholders::_1));

        // Capture functions used to generate streaming data
        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("SAMPLE_STREAM", &SampleHardwareModel::get_streaming_data));

        // Add type of streaming data to list to be called
        _periodic_logs.insert(std::map<std::string, boost::tuple<double, double>>::value_type("SAMPLE_STREAM", boost::tuple<double, double>(_absolute_start_time + _init_time_seconds, double(_millisecond_stream_delay) / 1000)));
        
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
        
        // Clean up the data provider we got
        delete _sdp;

        // Reset the time bus so the unique pointer does not try to delete the hub
        _time_bus.reset(); 
        //Do not destroy the time node, the bus will do it 

        // Close the uart
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
        double second_stream_delay = double(_millisecond_stream_delay) / 1000;
        const boost::shared_ptr<SampleDataPoint> data_point =
            boost::dynamic_pointer_cast<SampleDataPoint>(_sdp->get_data_point());

        sim_logger->info("SampleHardwareModel::send_periodic_data");

        // Loop through each periodic log and check if it's time to send
        for (std::map<std::string, boost::tuple<double, double>>::iterator it = _periodic_logs.begin(); it != _periodic_logs.end(); it++) 
        {
            boost::tuple<double, double> value = it->second;
            double prev_time = boost::tuples::get<0>(value);
            // Check within half a tick time period
            double next_time = prev_time + second_stream_delay - (_sim_microseconds_per_tick / 1000000.0) / 2;

            if (next_time < abs_time) 
            {   
                // Time to send more data
                sim_logger->info("Time to send more data!");
                it->second = boost::tuple<double, double>(abs_time, second_stream_delay);
                std::map<std::string, get_log_data_func>::iterator search = _get_log_data_map.find(it->first);
                if (search != _get_log_data_map.end())
                {
                    // Call function specified in list
                    get_log_data_func f = search->second;
                    (this->*f)(*data_point, data);
                }
            }
        }
    }


    void SampleHardwareModel::get_streaming_data(const SampleDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        // Header
        out_data.push_back(0xAD);
        out_data.push_back(0xDE);

        // Payload
        out_data.push_back(_counter);
        out_data.push_back(_data);

        // Trailer
        out_data.push_back(0xEF);
        out_data.push_back(0xBE);

        // Log reply data in man readable format and ship the message bytes off
        sim_logger->debug("SampleHardwareModel::uart_read_callback:  REPLY   %s\n",
            SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
        _uart_connection->write(&out_data[0], out_data.size());
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
        if ((in_data[1] != 0xDE) || (in_data[0] !=0xAD))
        {
            sim_logger->debug("SampleHardwareModel::uart_read_callback: Header incorrect!");
            return;
        }

        // Check trailer - 0xBEEF
        if ((in_data[8] != 0xBE) || (in_data[7] !=0xEF))
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
                sim_logger->debug("SampleHardwareModel::uart_read_callback: New millisecond stream delay of %d", _millisecond_stream_delay);
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
