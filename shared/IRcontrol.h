#ifndef IRCONTROL_H
#define IRCONTROL_H

enum EIRCommand
{
    IRCMD_FWD=0,
    IRCMD_BWD,
    IRCMD_LEFT,
    IRCMD_RIGHT,
    IRCMD_LEFT_FWD,
    IRCMD_LEFT_BWD,
    IRCMD_RIGHT_FWD,
    IRCMD_RIGHT_BWD,
    IRCMD_NONE // Marker, never actually send
};

union SIRMessage
{
    struct
    {
        EIRCommand command : 3;
        uint8_t speed : 4;
        bool translate : 1;
    };
    uint8_t byte;
    SIRMessage(EIRCommand c, uint8_t s, uint8_t t) : command(c), speed(s), translate(t) { }
    SIRMessage(uint8_t b) : byte(b) { }
    SIRMessage(void) : byte(0) { }
};

inline uint8_t IRCheckSum(const SIRMessage &msg)
{ return msg.command + msg.speed + msg.translate; }

#endif // IRCONTROL_H
