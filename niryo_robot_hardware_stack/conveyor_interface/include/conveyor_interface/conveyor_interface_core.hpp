/*
conveyor_interface_interface_core.hpp
Copyright (C) 2020 Niryo
All rights reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http:// www.gnu.org/licenses/>.
*/

#ifndef CONVEYOR_INTERFACE_CORE_HPP
#define CONVEYOR_INTERFACE_CORE_HPP

// c++
#include <memory>
#include <vector>

// ros
#include <ros/ros.h>

// niryo
#include "can_driver/can_interface_core.hpp"
#include "ttl_driver/ttl_interface_core.hpp"

#include "conveyor_interface/SetConveyor.h"
#include "conveyor_interface/ControlConveyor.h"
#include "conveyor_interface/ConveyorFeedbackArray.h"
#include "niryo_robot_msgs/CommandStatus.h"

#include "common/model/i_interface_core.hpp"

namespace conveyor_interface
{

/**
 * @brief The ConveyorInterfaceCore class
 */
class ConveyorInterfaceCore : public common::model::IInterfaceCore
{
    public:
        ConveyorInterfaceCore(ros::NodeHandle& nh,
                              std::shared_ptr<ttl_driver::TtlInterfaceCore> ttl_interface,
                              std::shared_ptr<can_driver::CanInterfaceCore> can_interface);
        virtual ~ConveyorInterfaceCore() override;
        virtual bool init(ros::NodeHandle& nh) override;

        bool isInitialized();

        std::vector<std::shared_ptr<common::model::ConveyorState> > getConveyorStates() const;

private:
        virtual void initParameters(ros::NodeHandle& nh) override;
        virtual void startServices(ros::NodeHandle& nh) override;
        virtual void startPublishers(ros::NodeHandle& nh) override;
        virtual void startSubscribers(ros::NodeHandle& nh) override;

        conveyor_interface::SetConveyor::Response addConveyor();
        conveyor_interface::SetConveyor::Response initTTLConveyor(const std::shared_ptr<common::model::ConveyorState>& conveyor_state);
        conveyor_interface::SetConveyor::Response initCANConveyor(const std::shared_ptr<common::model::ConveyorState>& conveyor_state);
        conveyor_interface::SetConveyor::Response removeConveyor(uint8_t id);

        bool _callbackPingAndSetConveyor(conveyor_interface::SetConveyor::Request &req, conveyor_interface::SetConveyor::Response &res);
        bool _callbackControlConveyor(conveyor_interface::ControlConveyor::Request &req, conveyor_interface::ControlConveyor::Response &res);

        void _publishConveyorsFeedback(const ros::TimerEvent&);

    private:
        struct BusConfig
        {
            BusConfig(common::model::EHardwareType t) :
              type(t)
            {}

            bool isValid() { return !pool_id_list.empty() && type != common::model::EHardwareType::UNKNOWN; }

            std::shared_ptr<common::model::IDriverCore> interface;

            common::model::EHardwareType type{common::model::EHardwareType::UNKNOWN};
            uint8_t default_id{1};
            std::set<uint8_t> pool_id_list;

            double max_effort{0.0};
            double micro_steps{8.0};
        };
        std::mutex _state_map_mutex;

        std::map<common::model::EBusProtocol, BusConfig> _bus_config_map;

        ros::Timer _publish_conveyors_feedback_timer;

        ros::ServiceServer _ping_and_set_stepper_server;
        ros::ServiceServer _control_conveyor_server;

        ros::Publisher _conveyors_feedback_publisher;
        ros::Publisher _conveyor_status_publisher;

        // currently connected and configured conveyors

        std::shared_ptr<ttl_driver::TtlInterfaceCore> _ttl_interface;
        std::shared_ptr<can_driver::CanInterfaceCore> _can_interface;

        std::map<uint8_t, std::shared_ptr<common::model::ConveyorState> > _state_map;

        static constexpr int TTL_DEFAULT_ID{8};
        static constexpr int CAN_DEFAULT_ID{6};

        double _publish_feedback_duration{0.0};
};

} // ConveyorInterface

#endif
