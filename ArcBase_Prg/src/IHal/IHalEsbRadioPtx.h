#ifndef I_HAL_ESB_RADIO_PTX_H
#define I_HAL_ESB_RADIO_PTX_H

#include "stdint.h"

class IHalEsbRadioPtx
{
  public:
    virtual void On() = 0;
    virtual void SetupAddress0(uint8_t *address) = 0;
    virtual void SetupAddress1(uint8_t *address) = 0;
    virtual void SetupAddressPrefixes(uint8_t *prefixes, uint8_t prefixesCount) = 0;
    virtual void Off() = 0;
    virtual void SendFrame(uint8_t pipe, uint8_t *data, uint8_t dataLength) = 0;
    virtual bool IsRadioBusy() = 0;
    virtual bool SendSucceeded() = 0;
};

#endif