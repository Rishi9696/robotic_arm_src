/*
    synchronize_motor_cmd.hpp
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DXL_SYNCHRONIZE_MOTOR_CMD_H
#define DXL_SYNCHRONIZE_MOTOR_CMD_H

#include <string>
#include <vector>

#include "dynamixel_driver/dxl_enum.hpp"
namespace DynamixelDriver
{
    class SynchronizeMotorCmd {

        public:

            SynchronizeMotorCmd();
            SynchronizeMotorCmd(DxlCommandType_t type,
                                std::vector<uint8_t> motor_id,
                                std::vector<uint32_t> params);
            
            void reset();

            //setters
            void setType(DxlCommandType_t type);
            void setMotorsId(std::vector<uint8_t> motor_id);
            void setParams(std::vector<uint32_t> params);

            //getters
            DxlCommandType_t getType() const;
            std::vector<uint8_t> getMotorsId() const;
            std::vector<uint32_t> getParams() const;

            std::string str() const;
            bool isValid() const;

        private:

            DxlCommandType_t _type;
            std::vector<uint8_t> _motor_id_list;
            std::vector<uint32_t> _param_list;
    };

    inline
    DxlCommandType_t
    SynchronizeMotorCmd::getType() const
    {
        return _type;
    }

    inline
    std::vector<uint8_t>
    SynchronizeMotorCmd::getMotorsId() const
    {
        return _motor_id_list;
    }

    inline
    std::vector<uint32_t>
    SynchronizeMotorCmd::getParams() const
    {
        return _param_list;
    }

    inline
    bool SynchronizeMotorCmd::isValid() const
    {
        return DxlCommandType_t::CMD_TYPE_UNKNOWN != _type &&
                !_motor_id_list.empty() &&
                (_motor_id_list.size() == _param_list.size());
    }

} //DynamixelDriver

#endif
