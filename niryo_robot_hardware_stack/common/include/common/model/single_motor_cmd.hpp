/*
single_motor_cmd.hpp
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

#ifndef DXL_SINGLE_MOTOR_CMD_H
#define DXL_SINGLE_MOTOR_CMD_H

#include <string>
#include <vector>

#include "common/model/abstract_motor_cmd.hpp"
#include "common/model/dxl_command_type_enum.hpp"
#include "common/model/stepper_command_type_enum.hpp"
#include "common/model/single_motor_cmd_interface.hpp"

namespace common
{
namespace model
{

/**
 * @brief The SingleMotorCmd class
 */
template<typename T, typename TE>
class SingleMotorCmd : public AbstractMotorCmd<T>, public SingleMotorCmdI
{
    public:
        SingleMotorCmd();
        SingleMotorCmd(T type,
                       uint8_t motor_id,
                       uint32_t param = 0);

        // AbstractMotorCmd interface
        bool isCmdStepper() const override;
        bool isCmdDxl() const override;
        virtual void reset() override;
        virtual void clear() override;
        virtual std::string str() const override;
        virtual bool isValid() const override;
};

/**
 * @brief SingleMotorCmd::SingleMotorCmd
 */
template<typename T, typename TE>
SingleMotorCmd<T, TE>::SingleMotorCmd() :
    AbstractMotorCmd<T>(T::CMD_TYPE_UNKNOWN)
{
    reset();
}

/**
 * @brief SingleMotorCmd::SingleMotorCmd
 * @param type
 * @param motor_id
 * @param param
 */
template<typename T, typename TE>
SingleMotorCmd<T, TE>::SingleMotorCmd(T type,
                               uint8_t motor_id,
                               uint32_t param) :
    AbstractMotorCmd<EDxlCommandType>(type),
    SingleMotorCmdI(motor_id, param)
{}

// ***********************
//  AbstractMotorCmd intf
// ***********************

/**
 * @brief SingleMotorCmd::isCmdStepper
 * @param none
 * @return
 */
template<typename T, typename TE>
bool SingleMotorCmd<T, TE>::isCmdStepper() const
{
    return typeid(T) == typeid(common::model::EStepperCommandType);
}

/**
 * @brief SingleMotorCmd::isCmdDxl
 * @param none
 * @return
 */
template<typename T, typename TE>
bool SingleMotorCmd<T, TE>::isCmdDxl() const
{
    return typeid(T) == typeid(common::model::EDxlCommandType);
}

/**
 * @brief SingleMotorCmd::reset
 */
template<typename T, typename TE>
void SingleMotorCmd<T, TE>::reset()
{
    this->setType(T::CMD_TYPE_UNKNOWN);
    clear();
}

/**
 * @brief SingleMotorCmd::clear
 */
template<typename T, typename TE>
void SingleMotorCmd<T, TE>::clear()
{
    _id = 0;
    _param = 0;
}

/**
 * @brief SingleMotorCmd::str
 * @return
 */
template<typename T, typename TE>
std::string SingleMotorCmd<T, TE>::str() const
{
    std::ostringstream ss;
    ss << "Single motor cmd - ";

    ss << TE(this->getType()).toString();

    ss << ": ";
    ss << "motor " << static_cast<int>(_id) << ": " << static_cast<int>(_param);

    return ss.str();
}

/**
 * @brief SingleMotorCmd::isValid
 * @return
 */
template<typename T, typename TE>
bool SingleMotorCmd<T, TE>::isValid() const
{
    return (T::CMD_TYPE_UNKNOWN != this->getType()) &&
           (0 != _id);
}

} // namespace model
} // namespace common

#endif
