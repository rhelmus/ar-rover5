#include "../../shared/shared.h"
#include "ringbuffer.h"

#include <Arduino.h>
#include <SPP.h>
#include <Usb.h>
#include <Wire.h>


// From http://jeelabs.org/2011/05/22/atmega-memory-use/
int freeRam()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


namespace {

volatile EMessage lastRecTWIMessage = MSG_NONE;
volatile uint32_t bytesPassed;
volatile CRingBuffer<128> TWIPassBuffer;

USB usbDev;
BTD bluetoothDev(&usbDev);
SPP serialBluetooth(&bluetoothDev);


void TWIRequest(void)
{
    if (lastRecTWIMessage == MSG_PING)
    {
        uint8_t buf[4];
        longToBytes(132560, buf);
        Wire.write(buf, 4);
    }

    lastRecTWIMessage = MSG_NONE;
}

void TWIReceive(int bytes)
{
    lastRecTWIMessage = static_cast<EMessage>(Wire.read());
    --bytes;

    if ((lastRecTWIMessage >= MSG_PASS_START) && (lastRecTWIMessage <= MSG_PASS_END))
    {
        bytesPassed += (bytes + 1);
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


//    Serial.print("received:");
//    while (Wire.available())
//        Serial.print(Wire.read(), DEC);
//    Serial.println("");
}

}

void setup()
{
    Serial.begin(115200);

    usbDev.Init();

    Wire.begin(BRIDGE_TWI_ADDRESS);
    Wire.onRequest(TWIRequest);
    Wire.onReceive(TWIReceive);

    Serial.println(F("Initialized"));
    Serial.print(F("Free RAM: ")); Serial.println(freeRam());
}

void loop()
{
    usbDev.Task();

    const uint32_t curtime = millis();
    static uint32_t updelay;

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
        }


//        Serial.print("Passed "); Serial.print(bytesPassed, DEC); Serial.println(" through");
        /*if (serialBluetooth.connected)
            serialBluetooth.println(F("Hello from Arduino"));*/
        /*
        if (serialBluetooth.connected)
        {
            serialBluetooth.print(MSG_BT_STARTMARKER);
            serialBluetooth.print(6);
            serialBluetooth.print(MSG_BATTERY);
            serialBluetooth.print(F("hello"));
            serialBluetooth.print(MSG_BT_ENDMARKER);
        }*/
    }
}
