/*
    abstract_motor_driver.cpp
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

#include "ttl_driver/abstract_motor_driver.hpp"

#include <sstream>
#include <vector>
#include <string>

using ::std::shared_ptr;

namespace ttl_driver
{

/**
 * @brief AbstractMotorDriver::AbstractMotorDriver
 */
AbstractMotorDriver::AbstractMotorDriver() :
  AbstractTtlDriver()
{
}

/**
 * @brief AbstractMotorDriver::AbstractMotorDriver
 * @param portHandler
 * @param packetHandler
 */
AbstractMotorDriver::AbstractMotorDriver(shared_ptr<dynamixel::PortHandler> portHandler,
                                         shared_ptr<dynamixel::PacketHandler> packetHandler) :
    AbstractTtlDriver(portHandler, packetHandler)
{
}

/**
 * @brief AbstractMotorDriver::~AbstractMotorDriver
 */
AbstractMotorDriver::~AbstractMotorDriver()
{
}

/**
 * @brief AbstractMotorDriver::str
 * @return
 */
std::string AbstractMotorDriver::str() const
{
    return "Motor Driver (" + AbstractTtlDriver::str() + ")";
}

}  // namespace ttl_driver
