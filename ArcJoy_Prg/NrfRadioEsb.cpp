#ifndef NRF_RADIO_ESB_H
#define NRF_RADIO_ESB_H

class NrfRadioEsb: public IHalEsbRadioPtx
{
  public:
    void On() = 0;
    void SetupAddress0(uint8_t *address) = 0;
    void SetupAddress1(uint8_t *address) = 0;
    void SetupAddressPrefixes(uint8_t *prefixes) = 0;
    void Off() = 0;
    void SendFrame(uint8_t pipe, uint8_t *data, uint8_t dataLength) = 0;
    bool RadioBusy() = 0;
    bool SendSucceeded() = 0;
}

#endif