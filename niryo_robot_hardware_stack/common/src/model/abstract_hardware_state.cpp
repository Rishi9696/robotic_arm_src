/*
    abstract_hardware_state.cpp
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

#include "common/model/abstract_hardware_state.hpp"

#include <string>
#include <sstream>

using ::std::string;
using ::std::ostringstream;

namespace common
{
namespace model
{
/**
 * @brief AbstractHardwareState::AbstractHardwareState
 */
AbstractHardwareState::AbstractHardwareState() :
    _type(EHardwareType::UNKNOWN),
    _bus_proto(EBusProtocol::UNKNOWN)
{
    reset();
}

/**
 * @brief AbstractHardwareState::AbstractHardwareState
 * @param type
 * @param bus_proto
 * @param id
 */
AbstractHardwareState::AbstractHardwareState(EHardwareType type, EBusProtocol bus_proto, uint8_t id) :
      _type(type),
      _bus_proto(bus_proto),
      _id(id),
      _temperature(0),
      _voltage(0.0),
      _hw_error(0),
      _hw_error_message("")
{
}

/**
 * @brief AbstractHardwareState::~AbstractHardwareState
 */
AbstractHardwareState::~AbstractHardwareState()
{
}

/**
 * @brief AbstractHardwareState::reset
 */
void AbstractHardwareState::reset()
{
    _id = 0;
    _temperature = 0;
    _voltage = 0.0;
    _hw_error = 0;
    _hw_error_message.clear();
}

/**
 * @brief AbstractHardwareState::operator ==
 * @param m
 * @return
 */
bool AbstractHardwareState::operator==(const AbstractHardwareState &m)
{
    return (this->_id == m._id);
}

/**
 * @brief AbstractHardwareState::str
 * @return
 */
string AbstractHardwareState::str() const
{
    ostringstream ss;

    ss << "AbstractHardwareState (" << static_cast<int>(_id) << ")" << "\n";

    ss << "temperature " << _temperature << "\n"
       << "voltage " << _voltage << "\n"
       << "hw_error " << _hw_error << "\n"
       << "hw_error_message \"" << _hw_error_message << "\"";
    ss << "\n";

    return ss.str();
}

/**
 * @brief StepperMotorState::setFirmwareVersion
 * @param firmware_version
 */
void AbstractHardwareState::setFirmwareVersion(std::string& firmware_version)
{
    _firmware_version = firmware_version;
}

/**
 * @brief AbstractHardwareState::setTemperature
 * @param temp
 */
void AbstractHardwareState::setTemperature(uint32_t temp)
{
    _temperature = temp;
}

/**
 * @brief AbstractHardwareState::setVoltage
 * @param volt
 */
void AbstractHardwareState::setVoltage(double volt)
{
    _voltage = volt;
}

/**
 * @brief AbstractHardwareState::setHardwareError
 * @param hw_error
 */
void AbstractHardwareState::setHardwareError(uint32_t hw_error)
{
    _hw_error = hw_error;
}

/**
 * @brief AbstractHardwareState::setHardwareError
 * @param hw_error_msg
 */
void AbstractHardwareState::setHardwareError(std::string hw_error_msg)
{
  _hw_error_message = hw_error_msg;
}

}  // namespace model
}  // namespace common
