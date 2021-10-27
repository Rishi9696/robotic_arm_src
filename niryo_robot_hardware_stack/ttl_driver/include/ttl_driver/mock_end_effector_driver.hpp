/*
mock_end_effector_driver.hpp
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

#ifndef MOCK_END_EFFECTOR_DRIVER_HPP
#define MOCK_END_EFFECTOR_DRIVER_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

#include "abstract_end_effector_driver.hpp"
#include "fake_ttl_data.hpp"

#include "common/model/action_type_enum.hpp"
#include "end_effector_reg.hpp"
#include "common/model/end_effector_command_type_enum.hpp"
#include "common/model/end_effector_state.hpp"

namespace ttl_driver
{

/**
 * @brief The MockEndEffectorDriver class
 */
class MockEndEffectorDriver : public AbstractEndEffectorDriver
{
    public:
        MockEndEffectorDriver(std::shared_ptr<FakeTtlData> data);

    public:
        // AbstractTtlDriver interface : we cannot define them globally in AbstractTtlDriver
        // as it is needed here for polymorphism (AbstractTtlDriver cannot be a template class and does not
        // have access to reg_type). So it seems like a duplicate of StepperDriver
        std::string str() const override;

        int checkModelNumber(uint8_t id) override;
        int readFirmwareVersion(uint8_t id, std::string &version) override;
        
        int readTemperature(uint8_t id, uint32_t &_temperature) override;
        int readVoltage(uint8_t id, double &_voltage) override;
        int readHwErrorStatus(uint8_t id, uint32_t &hardware_status) override;

        int syncReadFirmwareVersion(const std::vector<uint8_t> &id_list, std::vector<std::string> &firmware_list) override;
        int syncReadTemperature(const std::vector<uint8_t> &id_list, std::vector<uint32_t> &temperature_list) override;
        int syncReadVoltage(const std::vector<uint8_t> &id_list, std::vector<double> &voltage_list) override;
        int syncReadHwErrorStatus(const std::vector<uint8_t> &id_list, std::vector<uint32_t> &hw_error_list) override;
        
        int scan(std::vector<uint8_t> &id_list) override;
    public:
        int ping(uint8_t id) override;

        // AbstractEndEffectorDriver
        int readButton0Status(uint8_t id, common::model::EActionType& action) override;
        int readButton1Status(uint8_t id, common::model::EActionType& action) override;
        int readButton2Status(uint8_t id, common::model::EActionType& action) override;

        int readAccelerometerXValue(uint8_t id, uint32_t& x_value) override;
        int readAccelerometerYValue(uint8_t id, uint32_t& y_value) override;
        int readAccelerometerZValue(uint8_t id, uint32_t& z_value) override;

        int readCollisionStatus(uint8_t id, bool& status) override;

        int readDigitalInput(uint8_t id, bool& in) override;
        int writeDigitalOutput(uint8_t id, bool out) override;

    private:
        std::shared_ptr<FakeTtlData> _fake_data;
};

} // ttl_driver

#endif // MockEndEffectorDriver
