#include "nRF_Hal/NrfEsbRadioPtx.h"
#include "nrf_error.h"
#include "string.h"

volatile bool NrfEsbRadioPtx::radioBusy = false;
volatile bool NrfEsbRadioPtx::radioSuccess = false;

NrfEsbRadioPtx::NrfEsbRadioPtx(bool autoControlHfClock)
{
  radioBusy = false;
  radioSuccess = false;
  m_enabled = false;
  m_autoControlHfClock = autoControlHfClock;
}

void NrfEsbRadioPtx::On()
{
  if(m_autoControlHfClock)
  {
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
  }
  nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
  nrf_esb_config.retransmit_count         = 40;
  nrf_esb_config.selective_auto_ack       = false;
  nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
  nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_1MBPS;
  nrf_esb_config.event_handler            = NrfEsbRadioPtx::nrfEsbEventHandler;
  nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;

  nrf_esb_init(&nrf_esb_config);
  m_enabled = true;

  nrf_esb_set_base_address_0(m_baseAddress0);
  nrf_esb_set_base_address_1(m_baseAddress1);
  nrf_esb_set_prefixes(m_prefixes,8);
}

void NrfEsbRadioPtx::SetupAddress0(uint8_t *address)
{
  memcpy(m_baseAddress0,address,ESB_ADDRESS_LENGTH);

  if(m_enabled)
  {
    nrf_esb_set_base_address_0(m_baseAddress0);
  }
}

void NrfEsbRadioPtx::SetupAddress1(uint8_t *address)
{
  memcpy(m_baseAddress1,address,ESB_ADDRESS_LENGTH);

  if(m_enabled)
  {
    nrf_esb_set_base_address_1(m_baseAddress1);
  }
}

void NrfEsbRadioPtx::SetupAddressPrefixes(uint8_t *prefixes, uint8_t prefixesCount)
{
  memcpy(m_prefixes,prefixes,ESB_PREFIXES_COUNT);

  if(m_enabled)
  {
    nrf_esb_set_prefixes(prefixes, prefixesCount);
  }
}

void NrfEsbRadioPtx::Off()
{
  nrf_esb_disable();
  m_enabled = false;
  if(m_autoControlHfClock)
  {
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
  }
}

void NrfEsbRadioPtx::SendFrame(uint8_t pipe, uint8_t *data, uint8_t dataLength)
{
  nrf_esb_payload_t tx_payload;

  NrfEsbRadioPtx::radioBusy = true;
  NrfEsbRadioPtx::radioSuccess = false;

  tx_payload.length  = dataLength;
  tx_payload.pipe    = pipe;
  memcpy(tx_payload.data, data, dataLength);
  tx_payload.noack = false;
  nrf_esb_write_payload(&tx_payload);
}

bool NrfEsbRadioPtx::IsRadioBusy()
{
  return NrfEsbRadioPtx::radioBusy;
}

bool NrfEsbRadioPtx::SendSucceeded()
{
  return NrfEsbRadioPtx::radioSuccess;
}

void NrfEsbRadioPtx::nrfEsbEventHandler(nrf_esb_evt_t const * p_event)
{
  nrf_esb_payload_t rx_payload;
  NrfEsbRadioPtx::radioBusy = false;
  switch (p_event->evt_id)
  {
    case NRF_ESB_EVENT_TX_SUCCESS:
      NrfEsbRadioPtx::radioSuccess = true;
      nrf_esb_flush_tx();
      break;
    case NRF_ESB_EVENT_TX_FAILED:
      nrf_esb_flush_tx();
      break;
    case NRF_ESB_EVENT_RX_RECEIVED:
      while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS);
      break;
  }
}

void NrfEsbRadioPtx::SetRfChannel(uint8_t rfChannel)
{
  nrf_esb_set_rf_channel(rfChannel);
}
