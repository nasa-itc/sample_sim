#ifndef NOS3_SAMPLEDATAPOINT_HPP
#define NOS3_SAMPLEDATAPOINT_HPP

#include <sim_i_data_point.hpp>

namespace Nos3
{
    class SampleDataPoint : public SimIDataPoint
    {
    public:
        SampleDataPoint(void);
        SampleDataPoint(const std::vector<double>& bvb);
        ~SampleDataPoint(void);
        std::string to_string(void) const;

        double get_bvb_x(void) const {return _bvb[0];}
        double get_bvb_y(void) const {return _bvb[1];}
        double get_bvb_z(void) const {return _bvb[2];} 
    
    private:
        std::vector<double> _bvb;
    };
}

#endif
