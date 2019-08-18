#ifndef I_HAL_ESB_RADIO_PRX_H
#define I_HAL_ESB_RADIO_PRX_H

#include "stdint.h"

class IHalEsbRadioPrx
{
  public:
    virtual void On() = 0;
    virtual void SetupAddress0(uint8_t *address) = 0;
    virtual void SetupAddress1(uint8_t *address) = 0;
    virtual void SetupAddressPrefixes(uint8_t *prefixes, uint8_t prefixesCount) = 0;
    virtual void Off() = 0;
    virtual bool FrameReceived() = 0;
    virtual bool GetNextFrame(uint8_t *pipe, uint8_t *data, uint8_t *dataLength) = 0;
    virtual void SetRfChannel(uint8_t rfChannel) = 0;
};

#endif