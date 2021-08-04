#ifndef _JOINTS_DRIVER_HPP
#define _JOINTS_DRIVER_HPP

#include <ros/ros.h>
#include <ttl_driver/ttl_interface_core.hpp>
#include <can_driver/can_driver_core.hpp>

#include <memory>
#include <map>
#include <string>

namespace joint_driver
{
    class JointDriver {
    public:
        JointDriver(ros::NodeHandle &nh);
        ~JointDriver();

        bool haveCan() const;
        bool haveTtl() const;

        // getters
        std::shared_ptr<ttl_driver::TtlInterfaceCore> getTtlDriverCore() const;
        std::shared_ptr<can_driver::CanDriverCore> getCanDriverCore() const;
        std::shared_ptr<common::model::IDriverCore> getProtocolOfMotor(std::string name) const;
    private:
        std::shared_ptr<ttl_driver::TtlInterfaceCore> _ttlDriverCore;
        std::shared_ptr<can_driver::CanDriverCore> _canDriverCore;

        std::map<std::string, std::shared_ptr<common::model::IDriverCore>> _m_name_proto; 
        
        bool _haveCan;
        bool _haveTtl;
        void init(ros::NodeHandle &nh);
    };
}   // joint_driver

#endif // _JOINTS_DRIVER_HPP