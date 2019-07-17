#ifndef ARC_BASE_C_H
#define ARC_BASE_C_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  bool left;
  bool right;
  bool up;
  bool down;
  bool fire;
} JoyState;

void arcBase_whileLoop(JoyState joy1, JoyState joy2);
void arcBase_init(void);

#ifdef __cplusplus
}
#endif

#endif
