#include "usbd_cdc_if.h"

extern int _write(int file, char *ptr, int len);
#define DEBUG(m) CDC_Transmit_FS(m, sizeof(m));
