#include "../../shared/shared.h"
#include "ringbuffer.h"

#include <Arduino.h>
#include <SPP.h>
#include <Usb.h>
#include <Wire.h>

#include <avr/wdt.h>

// From http://jeelabs.org/2011/05/22/atmega-memory-use/
int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


namespace {

volatile EMessage lastRecTWIMessage = MSG_NONE;
volatile CRingBuffer<128> TWIPassBuffer;

USB usbDev;
BTD bluetoothDev(&usbDev);
SPP serialBluetooth(&bluetoothDev);
bool btConnected;

bool receivedBTStart, receivedBTMSGSize;
uint8_t BTMsgSize, BTMsgBytesRead;
CRingBuffer<BRIDGE_MAX_REQSIZE> tempBTMsgBuffer;


void TWIRequest(void)
{
    // UNDONE
/*    if (lastRecTWIMessage == MSG_PING_BR)
    {
        uint8_t buf[4];
        longToBytes(132560, buf);
        Wire.write(buf, 4);
    }*/

    lastRecTWIMessage = MSG_NONE;
}

void TWIReceive(int bytes)
{
    lastRecTWIMessage = static_cast<EMessage>(Wire.read());
    --bytes;

    if ((lastRecTWIMessage >= MSG_PASS_START) && (lastRecTWIMessage <= MSG_PASS_END))
    {
        if (serialBluetooth.connected)
        {
            TWIPassBuffer.push(MSG_BT_STARTMARKER);
            TWIPassBuffer.push(bytes + 1);
            TWIPassBuffer.push(lastRecTWIMessage);
        }

        while (bytes)
        {
            if (serialBluetooth.connected)
                TWIPassBuffer.push(Wire.read());
            else
                Wire.read(); // Read & discard
            --bytes;
        }

        if (serialBluetooth.connected)
            TWIPassBuffer.push(MSG_BT_ENDMARKER);
    }
    else // Discard any data from other (unknown) messages
    {
        while (bytes)
        {
            Wire.read();
            --bytes;
        }
    }

}

}

void setup()
{
    Serial.begin(115200);

    usbDev.Init();

    Wire.begin(BRIDGE_TWI_ADDRESS);
    Wire.onRequest(TWIRequest);
    Wire.onReceive(TWIReceive);

    // Enable watchdog in case bluetooth crashes (e.g. out of range)
    wdt_enable(WDTO_4S);

    Serial.println(F("Initialized"));
    Serial.print(F("Free RAM: ")); Serial.println(freeRam());
}

void loop()
{
    const uint32_t curtime = millis();
    static uint32_t updelay, pinguptime, ignoreconupdatedelay;
    static EMessage curbtmessage = MSG_NONE;

    if ((ignoreconupdatedelay < curtime) && !btConnected && serialBluetooth.connected)
        btConnected = true;

    if (!btConnected)
        wdt_reset(); // Only update if not connected yet or got a pong (see below)

    usbDev.Task();

    if (updelay < curtime)
    {
        updelay = curtime + 10;

        if (serialBluetooth.connected)
        {
            uint8_t buf[15];
            uint8_t count = 0;
            while (!TWIPassBuffer.isEmpty() && (count < 15))
            {
                buf[count] = TWIPassBuffer.pop();
                ++count;
            }
            serialBluetooth.print(buf, count);

            count = 0;
            while (serialBluetooth.available() && (count < 15))
            {
                const uint8_t b = serialBluetooth.read();
                if (!receivedBTStart)
                    receivedBTStart = (b == MSG_BT_STARTMARKER);
                else if (!receivedBTMSGSize)
                {
                    receivedBTMSGSize = true;
                    BTMsgSize = b;
                }
                else if (BTMsgBytesRead < BTMsgSize)
                {
                    if (curbtmessage == MSG_NONE)
                        curbtmessage = static_cast<EMessage>(b);
                    tempBTMsgBuffer.push(b);
                    ++BTMsgBytesRead;
                }
                else
                {
                    // Msg got through OK?
                    if (b == MSG_BT_ENDMARKER)
                    {
                        // First check special cases that are not passed through
                        if (curbtmessage == MSG_PONG)
                        {
                            tempBTMsgBuffer.clear();
                            wdt_reset();
                        }
                        else if (curbtmessage == MSG_CNTRL_DISCONNECT)
                        {
                            tempBTMsgBuffer.clear();
                            Serial.println("cntrl disconnect");
                            btConnected = false;
                            // Wait for a bit for actual disconnection before
                            // updating status
                            ignoreconupdatedelay = curtime + 3000;
                        }
                        else
                        {
                            Wire.beginTransmission(SPIDER_TWI_ADDRESS);
                            while (!tempBTMsgBuffer.isEmpty())
                                Wire.write(tempBTMsgBuffer.pop());
                            Wire.endTransmission();
                        }
                    }

                    receivedBTStart = receivedBTMSGSize = false;
                    BTMsgBytesRead = 0;
                    curbtmessage = MSG_NONE;
                    tempBTMsgBuffer.clear();
                }

                ++count;
            }
#if 0
            if (count)
            {
                Serial.print("Send bytes:"); Serial.println(count, DEC);
            }
#endif
        }
    }
    else if (pinguptime < curtime)
    {
        pinguptime = curtime + 100;
        if (serialBluetooth.connected)
        {
            TWIPassBuffer.push(MSG_BT_STARTMARKER);
            TWIPassBuffer.push(1);
            TWIPassBuffer.push(MSG_PING);
            TWIPassBuffer.push(MSG_BT_ENDMARKER);
        }
    }
}
