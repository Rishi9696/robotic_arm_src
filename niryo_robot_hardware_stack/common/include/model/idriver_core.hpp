/*
    idriver_core.hpp
    Copyright (C) 2017 Niryo
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

#ifndef IDriverCore_H
#define IDriverCore_H

#include <string>

namespace common {
    namespace model {

        class IDriverCore
        {
            public:
                virtual ~IDriverCore() = 0;

                virtual void startControlLoop() = 0;
                virtual bool isConnectionOk() const = 0;

                virtual void activeDebugMode(bool mode) = 0;

                virtual int launchMotorsReport() = 0;

            private:
                virtual void init() = 0;
                virtual void initParameters() = 0;
                virtual void resetHardwareControlLoopRates() = 0;
                virtual void controlLoop() = 0;
                virtual void _executeCommand() = 0;
        };

        inline
        IDriverCore::~IDriverCore()
        {

        }

    } // namespace model
} // namespace common

#endif
