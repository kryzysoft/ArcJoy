#include "nRF_Hal/NrfEsbRadioPrx.h"
#include "nrf_error.h"
#include "string.h"

volatile bool NrfEsbRadioPrx::frameReceived = false;

NrfEsbRadioPrx::NrfEsbRadioPrx(bool autoControlHfClock)
{
  m_enabled = false;
  m_autoControlHfClock = autoControlHfClock;
}

void NrfEsbRadioPrx::On()
{
  if(m_autoControlHfClock)
  {
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
  }

  nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
  nrf_esb_config.payload_length           = 8;
  nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
  nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
  nrf_esb_config.mode                     = NRF_ESB_MODE_PRX;
  nrf_esb_config.event_handler            = NrfEsbRadioPrx::nrfEsbEventHandler;
  nrf_esb_config.selective_auto_ack       = false;

  nrf_esb_init(&nrf_esb_config);
  m_enabled = true;

  nrf_esb_set_base_address_0(m_baseAddress0);
  nrf_esb_set_base_address_1(m_baseAddress1);
  nrf_esb_set_prefixes(m_prefixes,8);
}

void NrfEsbRadioPrx::SetupAddress0(uint8_t *address)
{
  memcpy(m_baseAddress0,address,ESB_ADDRESS_LENGTH);

  if(m_enabled)
  {
    nrf_esb_set_base_address_0(m_baseAddress0);
  }
}

void NrfEsbRadioPrx::SetupAddress1(uint8_t *address)
{
  memcpy(m_baseAddress1,address,ESB_ADDRESS_LENGTH);

  if(m_enabled)
  {
    nrf_esb_set_base_address_1(m_baseAddress1);
  }
}

void NrfEsbRadioPrx::SetupAddressPrefixes(uint8_t *prefixes, uint8_t prefixesCount)
{
  memcpy(m_prefixes,prefixes,ESB_PREFIXES_COUNT);

  if(m_enabled)
  {
    nrf_esb_set_prefixes(prefixes, prefixesCount);
  }
}

void NrfEsbRadioPrx::Off()
{
  nrf_esb_disable();
  m_enabled = false;
  if(m_autoControlHfClock)
  {
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
  }
}

bool NrfEsbRadioPrx::GetNextFrame(uint8_t *pipe, uint8_t *data, uint8_t *dataLength)
{
  nrf_esb_payload_t rx_payload;
  frameReceived = false;

  if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
  {
    *pipe = rx_payload.pipe;
    *dataLength = rx_payload.length;
    memcpy(data,rx_payload.data,*dataLength);
    return true;
  }
  else
  {
    return false;
  }
}

bool NrfEsbRadioPrx::FrameReceived()
{
  return frameReceived;
}

void NrfEsbRadioPrx::nrfEsbEventHandler(nrf_esb_evt_t const * p_event)
{
  switch (p_event->evt_id)
  {
    case NRF_ESB_EVENT_TX_SUCCESS:
      break;
    case NRF_ESB_EVENT_TX_FAILED:
      break;
    case NRF_ESB_EVENT_RX_RECEIVED:
      frameReceived = true;
      break;
  }
}
