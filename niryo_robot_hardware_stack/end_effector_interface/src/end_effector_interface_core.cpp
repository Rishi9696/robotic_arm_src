/*
    end_effector_interface_core.cpp
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

// c++
#include <functional>
#include <string>
#include <vector>

// ros
#include "common/model/hardware_type_enum.hpp"
#include "niryo_robot_msgs/SetBool.h"

// niryo
#include "end_effector_interface/end_effector_interface_core.hpp"
#include "common/model/end_effector_state.hpp"


using ::std::lock_guard;
using ::std::mutex;
using ::std::string;
using ::std::to_string;

using ::common::model::EndEffectorState;
using ::common::model::ButtonTypeEnum;
using ::common::model::EButtonType;
using ::common::model::EndEffectorSingleCmd;
using ::common::model::EEndEffectorCommandType;

namespace end_effector_interface
{

/**
 * @brief EndEffectorInterfaceCore::EndEffectorInterfaceCore
 * @param nh
 * @param ttl_interface
 */
EndEffectorInterfaceCore::EndEffectorInterfaceCore(ros::NodeHandle& nh,
                                                   std::shared_ptr<ttl_driver::TtlInterfaceCore > ttl_interface):
    _ttl_interface(ttl_interface)
{
    ROS_DEBUG("EndEffectorInterfaceCore::ctor");

    init(nh);
}

/**
 * @brief EndEffectorInterfaceCore::~EndEffectorInterfaceCore
 */
EndEffectorInterfaceCore::~EndEffectorInterfaceCore()
{
}

/**
 * @brief EndEffectorInterfaceCore::init
 * @param nh
 * @return
 */
bool EndEffectorInterfaceCore::init(ros::NodeHandle &nh)
{
    ROS_DEBUG("EndEffectorInterfaceCore::init - Initializing parameters...");
    initParameters(nh);

    ROS_DEBUG("EndEffectorInterfaceCore::initEndEffectorHardware...");
    initEndEffectorHardware();

    ROS_DEBUG("EndEffectorInterfaceCore::init - Starting services...");
    startServices(nh);

    ROS_DEBUG("EndEffectorInterfaceCore::init - Starting publishers...");
    startPublishers(nh);

    ROS_DEBUG("EndEffectorInterfaceCore::init - Starting subscribers...");
    startSubscribers(nh);

    return true;
}

/**
 * @brief EndEffectorInterfaceCore::initParameters
 * @param nh
 */
void EndEffectorInterfaceCore::initParameters(ros::NodeHandle& nh)
{
    int id = -1;
    nh.getParam("end_effector_id", id);
    _id = static_cast<uint8_t>(id);

    nh.getParam("check_end_effector_status_frequency", _check_end_effector_status_frequency);

    ROS_DEBUG("EndEffectorInterfaceCore::initParameters - end effector id : %d", _id);
    ROS_DEBUG("EndEffectorInterfaceCore::initParameters - end effector status frequency : %f", _check_end_effector_status_frequency);

    std::string hw_type;
    nh.getParam("hardware_type", hw_type);
    auto ee_type = common::model::HardwareTypeEnum(hw_type.c_str());

    //  initiliaze end effector state
    _end_effector_state = std::make_shared<EndEffectorState>(_id, ee_type);

    uint8_t button_id = 1;
    while (nh.hasParam("button_"  + std::to_string(button_id) + "/type"))
    {
      std::string button_type = "";
      nh.getParam("button_" + std::to_string(button_id) + "/type", button_type);
      auto eType = ButtonTypeEnum(button_type.c_str());

      ROS_INFO("EndEffectorInterfaceCore::initParameters : configure button %d of type %s", button_id, eType.toString().c_str());
      _end_effector_state->configureButton(button_id, eType);
      button_id++;
    }
}

/**
 * @brief EndEffectorInterfaceCore::startServices
 * @param nh
 */
void EndEffectorInterfaceCore::startServices(ros::NodeHandle& nh)
{
    _digital_in_server = nh.advertiseService("set_ee_io_state",
                                             &EndEffectorInterfaceCore::_callbackSetIOState, this);
}

/**
 * @brief EndEffectorInterfaceCore::startPublishers
 * @param nh
 */
void EndEffectorInterfaceCore::startPublishers(ros::NodeHandle& nh)
{
  _free_drive_button_state_publisher = nh.advertise<end_effector_interface::EEButtonStatus>(
                                          "free_drive_button_status", 10, true);

  _save_pos_button_state_publisher = nh.advertise<end_effector_interface::EEButtonStatus>(
                                          "save_pos_button_status", 10, true);

  _custom_button_state_publisher = nh.advertise<end_effector_interface::EEButtonStatus>(
                                          "custom_button_status", 10, true);

  _digital_out_publisher = nh.advertise<end_effector_interface::EEIOState>(
                                          "io_state", 10);

  _publish_states_thread = std::thread(&EndEffectorInterfaceCore::_publishButtonState, this);
}

/**
 * @brief EndEffectorInterfaceCore::startSubscribers
 * @param nh
 */
void EndEffectorInterfaceCore::startSubscribers(ros::NodeHandle& nh)
{
    (void)nh; // unused

    ROS_DEBUG("EndEffectorInterfaceCore::startSubscribers - no subscribers to start");
}

/**
 * @brief EndEffectorInterfaceCore::initEndEffectorHardware
 */
void EndEffectorInterfaceCore::initEndEffectorHardware()
{
  // init driver
  if (_end_effector_state && _end_effector_state->isValid())
  {
      int result = _ttl_interface->setEndEffector(_end_effector_state);

      if (niryo_robot_msgs::CommandStatus::SUCCESS == result)
      {
          ROS_INFO("ToolsInterfaceCore::ctor - Set end effector success");
      }
      else
      {
          ROS_WARN("EndEffectorInterfaceCore::sendInitEndEffectorParams - "
                   "Set end effector failure, return : %d. Aborted...",
                   result);
      }
  }
}

/**
 * @brief EndEffectorInterfaceCore::_publishButtonState
 * TODO(CC) : timer
 */
void EndEffectorInterfaceCore::_publishButtonState()
{
    ros::Rate check_status_rate = ros::Rate(_check_end_effector_status_frequency);
    EEButtonStatus button_msg;
    EEIOState io_msg;

    while (ros::ok())
    {
        if (!_end_effector_state)
          continue;
        lock_guard<mutex> lck(_buttons_status_mutex);

        for (auto button : _end_effector_state->getButtonsStatus())
        {
            if (button->actions.empty())
                continue;

            button_msg.action = static_cast<int>(button->actions.front());
            switch (button->type)
            {
                case EButtonType::FREE_DRIVE_BUTTON:
                    _free_drive_button_state_publisher.publish(button_msg);
                    break;
                case EButtonType::SAVE_POSITION_BUTTON:
                    _save_pos_button_state_publisher.publish(button_msg);
                    break;
                case EButtonType::CUSTOM_BUTTON:
                    _custom_button_state_publisher.publish(button_msg);
                    break;
                default:
                    break;
            }
            button->actions.pop();
        }

        // digital io state
        io_msg.in = _end_effector_state->getDigitalIn();
        io_msg.out = _end_effector_state->getDigitalOut();
        _digital_out_publisher.publish(io_msg);

        check_status_rate.sleep();
    }
}

bool EndEffectorInterfaceCore::_callbackSetIOState(end_effector_interface::SetEEDigitalOut::Request &req,
                                                   end_effector_interface::SetEEDigitalOut::Response &res)
{
    lock_guard<mutex> lck(_io_mutex);
    res.state = false;

    if (_end_effector_state && _end_effector_state->isValid())
    {
        _ttl_interface->addSingleCommandToQueue(std::make_shared<EndEffectorSingleCmd>(EEndEffectorCommandType::CMD_TYPE_DIGITAL_OUTPUT,
                                                                                       _end_effector_state->getId(), std::initializer_list<uint32_t>{req.data}));
        // TODO(cc) find a way to check if ok
        res.state = true;
    }

    return res.state;
}

}  // namespace end_effector_interface
