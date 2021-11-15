/*
    conveyor_service_client.cpp
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


#include <ros/ros.h>
#include <ros/service_client.h>
#include "conveyor_interface/conveyor_interface_core.hpp"
#include <gtest/gtest.h>

#include <string>

static std::unique_ptr<ros::NodeHandle> nh;

TEST(TESTSuiteNedAndOne, setConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 6;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::ADD;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
    EXPECT_NE(srv.response.id, srv.request.id);
}

/***********************************************/
/******** Test case when we have 2 convoyers ***/

// TEST(TESTSuite, setConveyorWithoutId)
// {
//     auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

//     bool exists(client.waitForExistence(ros::Duration(1)));
//     EXPECT_TRUE(exists);

//     conveyor_interface::SetConveyor srv;
//     srv.request.cmd = conveyor_interface::SetConveyor::Request::ADD;
//     client.call(srv);

//     EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
//     EXPECT_EQ(srv.response.id, srv.request.id);
// }

// TEST(TESTSuite, setConveyorOverFlow)
// {
//     // This is the 3rd conveyor added, support only 2 now
//     auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

//     bool exists(client.waitForExistence(ros::Duration(1)));
//     EXPECT_TRUE(exists);

//     conveyor_interface::SetConveyor srv;
//     srv.request.id = 1;
//     srv.request.cmd = conveyor_interface::SetConveyor::Request::ADD;
//     client.call(srv);

//     EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::NO_CONVEYOR_LEFT);
// }

TEST(TESTSuiteNedAndOne, controlConveyor1)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 12;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = 1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNedAndOne, controlConveyor2)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 12;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}


TEST(TESTSuiteNedAndOne, controlConveyor3)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 12;
    srv.request.control_on = false;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNedAndOne, controlConveyorWrongId)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 7;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::CONVEYOR_ID_INVALID);
}

TEST(TESTSuiteNedAndOne, removeConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 12;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNedAndOne, duplicatteRemovingConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 12;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::NO_CONVEYOR_FOUND);
}

TEST(TESTSuiteNedAndOne, RemoveConveyorWrongId)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 7;  // to be checked
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::NO_CONVEYOR_FOUND);
}

TEST(TESTSuiteNed2, setConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 8;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::ADD;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
    EXPECT_NE(srv.response.id, srv.request.id);
}

TEST(TESTSuiteNed2, controlConveyor1)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 9;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = 1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNed2, controlConveyor2)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 9;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}


TEST(TESTSuiteNed2, controlConveyor3)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 9;
    srv.request.control_on = false;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNed2, controlConveyorWrongId)
{
    auto client = nh->serviceClient<conveyor_interface::ControlConveyor>("/niryo_robot/conveyor/control_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::ControlConveyor srv;
    srv.request.id = 7;
    srv.request.control_on = true;
    srv.request.speed = 75;
    srv.request.direction = -1;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::CONVEYOR_ID_INVALID);
}

TEST(TESTSuiteNed2, removeConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 9;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::SUCCESS);
}

TEST(TESTSuiteNed2, duplicatteRemovingConveyor)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 9;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::NO_CONVEYOR_FOUND);
}

TEST(TESTSuiteNed2, RemoveConveyorWrongId)
{
    auto client = nh->serviceClient<conveyor_interface::SetConveyor>("/niryo_robot/conveyor/ping_and_set_conveyor");

    bool exists(client.waitForExistence(ros::Duration(1)));
    EXPECT_TRUE(exists);

    conveyor_interface::SetConveyor srv;
    srv.request.id = 7;
    srv.request.cmd = conveyor_interface::SetConveyor::Request::REMOVE;
    client.call(srv);

    EXPECT_EQ(srv.response.status, niryo_robot_msgs::CommandStatus::NO_CONVEYOR_FOUND);
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "conveyor_interface_service_client");

    nh = std::make_unique<ros::NodeHandle>();

    testing::InitGoogleTest(&argc, argv);

    std::string hardware_version;
    ros::NodeHandle nh_private("~");
    nh_private.getParam("hardware_version", hardware_version);

    if (hardware_version == "ned2")
        testing::GTEST_FLAG(filter) = "-TESTSuiteNedAndOne.*";
    else if (hardware_version == "ned" || hardware_version == "one")
        testing::GTEST_FLAG(filter) = "-TESTSuiteNed2.*";

    return RUN_ALL_TESTS();
}
