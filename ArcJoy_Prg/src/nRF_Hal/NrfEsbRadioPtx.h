#ifndef NRF_ESB_RADIO_PTX_H
#define NRF_ESB_RADIO_PTX_H

#include "stdint.h"
#include "nrf_esb.h"
#include "nrf.h"
#include "IHal/IHalEsbRadioPtx.h"

const uint8_t ESB_ADDRESS_LENGTH = 4;
const uint8_t ESB_PREFIXES_COUNT = 8;

class NrfEsbRadioPtx: public IHalEsbRadioPtx
{
  private:
    static void nrfEsbEventHandler(nrf_esb_evt_t const * p_event);
    static volatile bool radioBusy;
    static volatile bool radioSuccess;
    bool m_autoControlHfClock;
    bool m_enabled;
    uint8_t m_baseAddress0[ESB_ADDRESS_LENGTH];
    uint8_t m_baseAddress1[ESB_ADDRESS_LENGTH];
    uint8_t m_prefixes[ESB_PREFIXES_COUNT];

  public:
    NrfEsbRadioPtx(bool autoControlHfClock);
    void On();
    void SetupAddress0(uint8_t *address);
    void SetupAddress1(uint8_t *address);
    void SetupAddressPrefixes(uint8_t *prefixes, uint8_t prefixesCount);
    void Off();
    void SendFrame(uint8_t pipe, uint8_t *data, uint8_t dataLength);
    bool IsRadioBusy();
    bool SendSucceeded();
};

#endif
