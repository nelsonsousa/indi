/*******************************************************************************
  Copyright(c) 2013 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#pragma once

#include "defaultdevice.h"
#include "indifocuserinterface.h"

namespace Connection
{
class Serial;
class TCP;
}
/**
 * \class Focuser
   \brief Class to provide general functionality of a focuser device.

   Both relative and absolute focuser supported. Furthermore, if no position feedback is available from the focuser,
   an open-loop control is possible using timers, speed presets, and direction of motion.
   Developers need to subclass Focuser to implement any driver for focusers within INDI.

\author Jasem Mutlaq
\author Gerry Rozema
*/
namespace INDI
{

class Focuser : public DefaultDevice, public FocuserInterface
{
    public:
        Focuser();
        virtual ~Focuser();

        /** \struct FocuserConnection
                \brief Holds the connection mode of the Focuser.
            */
        enum
        {
            CONNECTION_NONE   = 1 << 0, /** Do not use any connection plugin */
            CONNECTION_SERIAL = 1 << 1, /** For regular serial and bluetooth connections */
            CONNECTION_TCP    = 1 << 2  /** For Wired and WiFI connections */
        } FocuserConnection;

        virtual bool initProperties() override;
        virtual void ISGetProperties(const char *dev) override;
        virtual bool updateProperties() override;
        virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
        virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
        virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
        virtual bool ISSnoopDevice(XMLEle *root) override;

        /**
             * @brief setConnection Set Focuser connection mode. Child class should call this in the constructor before Focuser registers
             * any connection interfaces
             * @param value ORed combination of FocuserConnection values.
             */
        void setSupportedConnections(const uint8_t &value);

        /**
             * @return Get current Focuser connection mode
             */
        uint8_t getSupportedConnections() const
        {
            return focuserConnection;
        }

        static void buttonHelper(const char *button_n, ISState state, void *context);

    protected:
        /**
             * @brief saveConfigItems Saves the Device Port and Focuser Presets in the configuration file
             * @param fp pointer to configuration file
             * @return true if successful, false otherwise.
             */
        virtual bool saveConfigItems(FILE *fp) override;

        /** \brief perform handshake with device to check communication */
        virtual bool Handshake();

        /**
         * @brief SetFocuserMaxPosition Update focuser maximum position. It only updates the PresetNP property limits.
         * @param ticks maximum ticks
         * @return True
         */
        virtual bool SetFocuserMaxPosition(uint32_t ticks) override;

        /**
         * @brief syncPresets Updates the min/max/step range of the preset as per the maximum name of Absolute Focus Travel
         */
        virtual void SyncPresets(uint32_t ticks);

        INDI::PropertyNumber PresetNP {3};
        INDI::PropertySwitch PresetGotoSP {3};

        void processButton(const char *button_n, ISState state);

        Controller *controller;

        Connection::Serial *serialConnection = nullptr;
        Connection::TCP *tcpConnection       = nullptr;

        int PortFD = -1;

    private:
        bool callHandshake();
        uint8_t focuserConnection = CONNECTION_SERIAL | CONNECTION_TCP;
};
}
