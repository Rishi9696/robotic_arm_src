/*
    ttl_driver_unit_tests.cpp
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

// Bring in my package's API, which is what I'm testing
#include "common/model/abstract_motor_state.hpp"
#include "common/model/bus_protocol_enum.hpp"
#include "common/model/dxl_command_type_enum.hpp"
#include "common/model/dxl_motor_state.hpp"
#include "common/model/hardware_type_enum.hpp"
#include "common/model/joint_state.hpp"
#include "common/model/single_motor_cmd.hpp"
#include "common/model/stepper_command_type_enum.hpp"
#include "common/model/stepper_motor_state.hpp"
#include "common/model/synchronize_motor_cmd.hpp"
#include "ros/node_handle.h"
#include "ttl_driver/ttl_interface_core.hpp"
#include "ttl_driver/ttl_manager.hpp"
#include "dynamixel_sdk/dynamixel_sdk.h"

// Bring in gtest
#include <gtest/gtest.h>
#include <memory>
#include <ros/console.h>
#include <cassert>
#include <string>

using ::std::to_string;
using ::std::string;
using ::common::model::HardwareTypeEnum;
using ::common::model::BusProtocolEnum;
using ::common::model::DxlMotorState;
using ::common::model::EHardwareType;
using ::common::model::StepperMotorState;
using ::common::model::EBusProtocol;

/**
 * @brief addJointToTtlInterface
 * @param ttl_interface
 */
void addJointToTtlInterface(const std::shared_ptr<ttl_driver::TtlInterfaceCore>& ttl_interface)
{
    size_t nb_joints = 0;

    ros::NodeHandle robot_hwnh("joints_interface");
    // retrieve nb joints with checking that the config param exists for both name and id
    while (robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/id") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/name") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/type") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/bus"))
        nb_joints++;

    // connect and register joint state interface

    int currentIdDxl = 1;
    int currentIdStepper = 1;

    for (size_t j = 0; j < nb_joints; j++)
    {
        int joint_id_config = 0;
        string joint_name;
        string joint_type;
        string joint_bus;

        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/id", joint_id_config);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/name", joint_name);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/type", joint_type);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/bus", joint_bus);
        HardwareTypeEnum eType = HardwareTypeEnum(joint_type.c_str());
        BusProtocolEnum eBusProto = BusProtocolEnum(joint_bus.c_str());

        if (eType == EHardwareType::STEPPER || eType == EHardwareType::FAKE_STEPPER_MOTOR)
        {  // stepper
            std::string currentStepperNamespace = "steppers/stepper_" + to_string(currentIdStepper);

            auto stepperState = std::make_shared<StepperMotorState>(joint_name,
                                                                    eType,
                                                                    common::model::EComponentType::JOINT,
                                                                    eBusProto,
                                                                    static_cast<uint8_t>(joint_id_config));
            if (stepperState)
            {
                double offsetPos = 0.0;
                double gear_ratio = 1.0;
                int direction = 1;
                double max_effort = 0.0;

                robot_hwnh.getParam(currentStepperNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentStepperNamespace + "/gear_ratio", gear_ratio);
                robot_hwnh.getParam(currentStepperNamespace + "/direction", direction);
                robot_hwnh.getParam(currentStepperNamespace + "/max_effort", max_effort);

                // add parameters
                stepperState->setOffsetPosition(offsetPos);
                stepperState->setGearRatio(gear_ratio);
                stepperState->setDirection(static_cast<int8_t>(direction));
                stepperState->setMaxEffort(max_effort);

                if (eBusProto == EBusProtocol::TTL)
                  ttl_interface->addJoint(stepperState);

                currentIdStepper++;
            }
        }
        else if (eType != EHardwareType::UNKNOWN)
        {  // dynamixel
            auto dxlState = std::make_shared<DxlMotorState>(joint_name,
                                                            eType,
                                                            common::model::EComponentType::JOINT,
                                                            static_cast<uint8_t>(joint_id_config));
            if (dxlState)
            {
                double offsetPos = 0.0;
                int direction = 1;
                int positionPGain = 0;
                int positionIGain = 0;
                int positionDGain = 0;
                int velocityPGain = 0;
                int velocityIGain = 0;
                int FF1Gain = 0;
                int FF2Gain = 0;

                std::string currentDxlNamespace = "dynamixels/dxl_" + to_string(currentIdDxl);

                robot_hwnh.getParam(currentDxlNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentDxlNamespace + "/direction", direction);

                robot_hwnh.getParam(currentDxlNamespace + "/position_P_gain", positionPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_I_gain", positionIGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_D_gain", positionDGain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_P_gain", velocityPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/velocity_I_gain", velocityIGain);

                robot_hwnh.getParam(currentDxlNamespace + "/FF1_gain", FF1Gain);
                robot_hwnh.getParam(currentDxlNamespace + "/FF2_gain", FF2Gain);

                dxlState->setOffsetPosition(offsetPos);
                dxlState->setDirection(static_cast<int8_t>(direction));

                dxlState->setPositionPGain(static_cast<uint32_t>(positionPGain));
                dxlState->setPositionIGain(static_cast<uint32_t>(positionIGain));
                dxlState->setPositionDGain(static_cast<uint32_t>(positionDGain));

                dxlState->setVelocityPGain(static_cast<uint32_t>(velocityPGain));
                dxlState->setVelocityIGain(static_cast<uint32_t>(velocityIGain));

                dxlState->setFF1Gain(static_cast<uint32_t>(FF1Gain));
                dxlState->setFF2Gain(static_cast<uint32_t>(FF2Gain));

                if (eBusProto == EBusProtocol::TTL)
                  ttl_interface->addJoint(dxlState);

                currentIdDxl++;
            }
        }
    }  // end for (size_t j = 0; j < nb_joints; j++)
}

/**
 * @brief addJointToTtlManager
 * @param ttl_drv
 */
void addJointToTtlManager(const std::shared_ptr<ttl_driver::TtlManager>& ttl_drv)
{
    size_t nb_joints = 0;

    ros::NodeHandle robot_hwnh("joints_interface");
    // retrieve nb joints with checking that the config param exists for both name and id
    while (robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/id") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/name") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/type") &&
          robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/bus"))
        nb_joints++;

    // connect and register joint state interface
    int currentIdStepper = 1;
    int currentIdDxl = 1;

    for (size_t j = 0; j < nb_joints; j++)
    {
        int joint_id_config = 0;
        string joint_name;
        string joint_type;
        string joint_bus;

        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/id", joint_id_config);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/name", joint_name);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/type", joint_type);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/bus", joint_bus);
        HardwareTypeEnum eType = HardwareTypeEnum(joint_type.c_str());
        BusProtocolEnum eBusProto = BusProtocolEnum(joint_bus.c_str());

        if (eType == EHardwareType::STEPPER || eType == EHardwareType::FAKE_STEPPER_MOTOR)
        {  // stepper
            std::string currentStepperNamespace = "steppers/stepper_" + to_string(currentIdStepper);

            auto stepperState = std::make_shared<StepperMotorState>(joint_name,
                                                                    eType,
                                                                    common::model::EComponentType::JOINT,
                                                                    eBusProto,
                                                                    static_cast<uint8_t>(joint_id_config));
            if (stepperState)
            {
                double offsetPos = 0.0;
                double gear_ratio = 1.0;
                int direction = 1;
                double max_effort = 0.0;

                robot_hwnh.getParam(currentStepperNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentStepperNamespace + "/gear_ratio", gear_ratio);
                robot_hwnh.getParam(currentStepperNamespace + "/direction", direction);
                robot_hwnh.getParam(currentStepperNamespace + "/max_effort", max_effort);

                // add parameters
                stepperState->setOffsetPosition(offsetPos);
                stepperState->setGearRatio(gear_ratio);
                stepperState->setDirection(static_cast<int8_t>(direction));
                stepperState->setMaxEffort(max_effort);

                if (eBusProto == EBusProtocol::TTL)
                {
                  ttl_drv->addHardwareComponent(stepperState);
                }
                currentIdStepper++;
            }
        }
        else if (eType != EHardwareType::UNKNOWN)
        {  // dynamixel
            auto dxlState = std::make_shared<DxlMotorState>(joint_name,
                                                            eType,
                                                            common::model::EComponentType::JOINT,
                                                            static_cast<uint8_t>(joint_id_config));

            if (dxlState)
            {
                double offsetPos = 0.0;
                int direction = 1;
                int positionPGain = 0;
                int positionIGain = 0;
                int positionDGain = 0;
                int velocityPGain = 0;
                int velocityIGain = 0;
                int FF1Gain = 0;
                int FF2Gain = 0;

                std::string currentDxlNamespace = "dynamixels/dxl_" + to_string(currentIdDxl);

                robot_hwnh.getParam(currentDxlNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentDxlNamespace + "/direction", direction);

                robot_hwnh.getParam(currentDxlNamespace + "/position_P_gain", positionPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_I_gain", positionIGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_D_gain", positionDGain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_P_gain", velocityPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/velocity_I_gain", velocityIGain);

                robot_hwnh.getParam(currentDxlNamespace + "/FF1_gain", FF1Gain);
                robot_hwnh.getParam(currentDxlNamespace + "/FF2_gain", FF2Gain);

                dxlState->setOffsetPosition(offsetPos);
                dxlState->setDirection(static_cast<int8_t>(direction));

                dxlState->setPositionPGain(static_cast<uint32_t>(positionPGain));
                dxlState->setPositionIGain(static_cast<uint32_t>(positionIGain));
                dxlState->setPositionDGain(static_cast<uint32_t>(positionDGain));

                dxlState->setVelocityPGain(static_cast<uint32_t>(velocityPGain));
                dxlState->setVelocityIGain(static_cast<uint32_t>(velocityIGain));

                dxlState->setFF1Gain(static_cast<uint32_t>(FF1Gain));
                dxlState->setFF2Gain(static_cast<uint32_t>(FF2Gain));

                if (eBusProto == EBusProtocol::TTL)
                {
                  ttl_drv->addHardwareComponent(dxlState);
                }
                currentIdDxl++;
            }
        }
    }  // end for (size_t j = 0; j < nb_joints; j++)
}

// Declare a test
/******************************************************/
/************ Tests of ttl interface ******************/
/******************************************************/
/**
 * @brief The TtlInterfaceTestSuite class
 * TODO(Thuc) use config to get info of motors for each type of hw instead of hardcode
 */
class TtlInterfaceTestSuite : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
      ros::NodeHandle nh("ttl_driver");
      ros::Duration(5.0).sleep();

      ttl_interface = std::make_shared<ttl_driver::TtlInterfaceCore>(nh);

      addJointToTtlInterface(ttl_interface);
      // check connections
      EXPECT_TRUE(ttl_interface->isConnectionOk());
      EXPECT_TRUE(ttl_interface->scanMotorId(2));
      EXPECT_TRUE(ttl_interface->scanMotorId(3));
      EXPECT_TRUE(ttl_interface->scanMotorId(6));
    }

    static void TearDownTestCase()
    {
      ros::shutdown();
    }

    static std::shared_ptr<ttl_driver::TtlInterfaceCore> ttl_interface;
};

std::shared_ptr<ttl_driver::TtlInterfaceCore> TtlInterfaceTestSuite::ttl_interface;

// Test reboot motors
TEST_F(TtlInterfaceTestSuite, testRebootMotors)
{
  int resutl = ttl_interface->rebootHardware(ttl_interface->getJointState(2));
  EXPECT_EQ(resutl, static_cast<int>(niryo_robot_msgs::CommandStatus::SUCCESS));
}

// Test reboot motor with wrong id
TEST_F(TtlInterfaceTestSuite, testRebootMotorsWrongID)
{
  bool result;
  result = ttl_interface->rebootHardware(std::make_shared<common::model::DxlMotorState>());
  EXPECT_FALSE(result);
}

/**
 * @brief The TtlManagerTestSuite class
 */
class TtlManagerTestSuite : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
      ros::NodeHandle nh("ttl_driver");

      ttl_drv = std::make_shared<ttl_driver::TtlManager>(nh);

      addJointToTtlManager(ttl_drv);
      // check connections
      EXPECT_TRUE(ttl_drv->ping(2));
      EXPECT_TRUE(ttl_drv->ping(3));
      EXPECT_TRUE(ttl_drv->ping(6));
    }

    static std::shared_ptr<ttl_driver::TtlManager> ttl_drv;
};

std::shared_ptr<ttl_driver::TtlManager> TtlManagerTestSuite::ttl_drv;

/******************************************************/
/************** Tests of ttl manager ******************/
/******************************************************/

// Test driver received cmd
TEST_F(TtlManagerTestSuite, testSingleCmds)
{
  auto cmd_1 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE,
                                                             2,
                                                             std::initializer_list<uint32_t>{1});
  EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1)), COMM_SUCCESS);
  ros::Duration(0.01).sleep();

  // wrong id
  auto cmd_2 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE,
                                                             20,
                                                             std::initializer_list<uint32_t>{1});
  EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_2)), COMM_SUCCESS);
  ros::Duration(0.01).sleep();

  // wrong type cmd
  auto cmd_3 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_UNKNOWN,
                                                             2,
                                                             std::initializer_list<uint32_t>{1});
  EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_3)), COMM_SUCCESS);
  ros::Duration(0.01).sleep();

  // wrong type of cmd object
  auto cmd_4 = std::make_unique<common::model::StepperTtlSingleCmd>(common::model::EStepperCommandType::CMD_TYPE_TORQUE,
                                                                    2,
                                                                    std::initializer_list<uint32_t>{1});
  EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_4)), COMM_SUCCESS);
}

TEST_F(TtlManagerTestSuite, testSyncCmds)
{
  bool simulation_mode;
  ros::NodeHandle nh_private("~");
  nh_private.getParam("simulation_mode", simulation_mode);

  if (simulation_mode)
  {
    // sync cmd
    auto dynamixel_cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_1->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 2, 1);
    dynamixel_cmd_1->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 3, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // sync cmd with different motor types
    auto dynamixel_cmd_2 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_2->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 2, 1);
    dynamixel_cmd_2->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 6, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_2)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // redondant id
    auto dynamixel_cmd_3 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_3->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 3, 1);
    dynamixel_cmd_3->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 3, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_3)), COMM_SUCCESS);

    // wrong cmd type
    auto dynamixel_cmd_4 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_UNKNOWN);
    dynamixel_cmd_4->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 5, 1);
    dynamixel_cmd_4->addMotorParam(common::model::EHardwareType::FAKE_DXL_MOTOR, 3, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_4)), COMM_SUCCESS);
  }
  else
  {
    // sync cmd
    auto dynamixel_cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_1->addMotorParam(common::model::EHardwareType::XL430, 2, 1);
    dynamixel_cmd_1->addMotorParam(common::model::EHardwareType::XL430, 3, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // sync cmd with different motor types
    auto dynamixel_cmd_2 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_2->addMotorParam(common::model::EHardwareType::XL430, 2, 1);
    dynamixel_cmd_2->addMotorParam(common::model::EHardwareType::XL320, 6, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_2)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // redondant id
    auto dynamixel_cmd_3 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_3->addMotorParam(common::model::EHardwareType::XL430, 3, 1);
    dynamixel_cmd_3->addMotorParam(common::model::EHardwareType::XL430, 3, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_3)), COMM_SUCCESS);

    // wrong cmd type
    auto dynamixel_cmd_4 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_UNKNOWN);
    dynamixel_cmd_4->addMotorParam(common::model::EHardwareType::XL320, 5, 1);
    dynamixel_cmd_4->addMotorParam(common::model::EHardwareType::XL430, 3, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_4)), COMM_SUCCESS);
  }
}

//  Test control cmds
TEST_F(TtlManagerTestSuite, testSingleControlCmds)
{
    auto cmd_1_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE,
                                                                      2,
                                                                      std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    auto cmd_2_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE,
                                                                      3,
                                                                      std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_2_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

     auto cmd_3_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE,
                                                                       6,
                                                                       std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_3_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    ttl_drv->readJointsStatus();
    auto state_motor_2 = std::dynamic_pointer_cast<common::model::AbstractMotorState>(ttl_drv->getHardwareState(2));
    assert(state_motor_2);
    auto state_motor_3 = std::dynamic_pointer_cast<common::model::AbstractMotorState>(ttl_drv->getHardwareState(3));
    assert(state_motor_3);
    auto state_motor_6 = std::dynamic_pointer_cast<common::model::AbstractMotorState>(ttl_drv->getHardwareState(6));
    assert(state_motor_6);

    uint32_t pos_2 = state_motor_2->getPosition();
    uint32_t pos_3 = state_motor_3->getPosition();
    uint32_t pos_6 = state_motor_6->getPosition();

    uint32_t new_pos_2 = (pos_2 > 2048) ? pos_2 - 100 : pos_2 + 100;
    uint32_t new_pos_3 = (pos_3 > 2048) ? pos_3 - 100 : pos_3 + 100;
    uint32_t new_pos_6 = (pos_6 > 2048) ? pos_6 - 100 : pos_6 + 100;

    auto cmd_1 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION,
                                                               2,
                                                               std::initializer_list<uint32_t>{new_pos_2});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1)), COMM_SUCCESS);
    ros::Duration(1.0).sleep();

    auto cmd_2 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION,
                                                               3,
                                                               std::initializer_list<uint32_t>{new_pos_3});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_2)), COMM_SUCCESS);
    ros::Duration(1.0).sleep();

    auto cmd_3 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION,
                                                               6,
                                                               std::initializer_list<uint32_t>{new_pos_6});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_3)), COMM_SUCCESS);
    ros::Duration(1.0).sleep();

    ttl_drv->readJointsStatus();
    EXPECT_NEAR(state_motor_2->getPosition(), new_pos_2, 30);

    EXPECT_NEAR(state_motor_3->getPosition(), new_pos_3, 30);

    EXPECT_NEAR(state_motor_6->getPosition(), new_pos_6, 30);
}

TEST_F(TtlManagerTestSuite, testSyncControlCmds)
{
  bool simulation_mode;
  ros::NodeHandle nh_private("~");
  nh_private.getParam("simulation_mode", simulation_mode);

  common::model::EHardwareType dxl_type;
  if (!simulation_mode)
    dxl_type = common::model::EHardwareType::XL430;
  else
    dxl_type = common::model::EHardwareType::FAKE_DXL_MOTOR;

  // sync cmd
  auto cmd_1_torque = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
  cmd_1_torque->addMotorParam(dxl_type, 2, 1);
  cmd_1_torque->addMotorParam(dxl_type, 3, 1);

  EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1_torque)), COMM_SUCCESS);
  ros::Duration(0.01).sleep();

  EXPECT_EQ(ttl_drv->readJointsStatus(), true);
  auto state_motor_2 = std::dynamic_pointer_cast<common::model::AbstractMotorState>(ttl_drv->getHardwareState(2));
  assert(state_motor_2);
  auto state_motor_3 = std::dynamic_pointer_cast<common::model::AbstractMotorState>(ttl_drv->getHardwareState(3));
  assert(state_motor_3);
  uint32_t pos_2 = state_motor_2->getPosition();
  uint32_t pos_3 = state_motor_3->getPosition();

  auto cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION);

  uint32_t new_pos_2 = (pos_2 > 2048) ? pos_2 - 100 : pos_2 + 100;
  uint32_t new_pos_3 = (pos_3 > 1000) ? pos_3 - 100 : pos_3 + 100;
  cmd_1->addMotorParam(dxl_type, 2, new_pos_2);
  cmd_1->addMotorParam(dxl_type, 3, new_pos_3);

  EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1)), COMM_SUCCESS);
  ros::Duration(1.0).sleep();

  EXPECT_EQ(ttl_drv->readJointsStatus(), true);
  EXPECT_NEAR(state_motor_2->getPosition(), new_pos_2, 30);

  EXPECT_NEAR(state_motor_3->getPosition(), new_pos_3, 30);
}

// Test driver scan motors
TEST_F(TtlManagerTestSuite, scanTest)
{
    EXPECT_EQ(ttl_drv->scanAndCheck(), COMM_SUCCESS);
}

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    ros::init(argc, argv, "ttl_driver_unit_tests");

    return RUN_ALL_TESTS();
}