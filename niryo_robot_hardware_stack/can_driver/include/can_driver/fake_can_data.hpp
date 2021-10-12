/*
fake_can_data.hpp
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

#ifndef FAKE_CAN_DATA_HPP
#define FAKE_CAN_DATA_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace can_driver
{

class FakeCanData
{
public:
    FakeCanData() = default;

    struct FakeStepperRegister
    {
        std::string   firmware{};
        int32_t       position{0};
        uint32_t      temperature{0};
        double        voltage{0};
        uint16_t      model_number{0};

        uint8_t       id{0};

        uint8_t       speed{0};
        uint8_t       direction{0};
        bool          state{false};
    };

    // stepper
    std::map<uint8_t, FakeStepperRegister> stepper_registers;
};
}
#endif //FAKE_TTL_DATA_HPP
