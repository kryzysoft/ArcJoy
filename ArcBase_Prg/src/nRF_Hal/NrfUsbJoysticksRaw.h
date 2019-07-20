#ifndef NRF_USB_JOYSTICKS_RAW
#define NRF_USB_JOYSTICKS_RAW

typedef struct
{
  bool left;
  bool right;
  bool up;
  bool down;
  bool fire;
} JoyState;

#ifdef __cplusplus
extern "C" {
#endif

void JOYSTICKS_Init(void);
void JOYSTICKS_SendReports(JoyState joy1, JoyState joy2);

#ifdef __cplusplus
}
#endif

#endif