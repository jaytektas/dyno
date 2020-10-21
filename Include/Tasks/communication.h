#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "stdint.h"
#include "cmsis_os.h"
#include "usb_device.h"

#define OUTBOUND_HEADER		9							// 01 01 CM SZ SZ TK TK TK TK
#define INBOUND_HEADER		5
#define PACKET_CRC			4							// CR CR CR CR
#define PACKET_PAYLOAD		1024						// D0...Dx
#define MAX_PACKET_SIZE		(OUTBOUND_HEADER+PACKET_PAYLOAD+PACKET_CRC)

// offsets
#define PACKET_COMMAND		2
#define PACKET_SIZE			3
#define PACKET_OFFSET		6
#define PACKET_TICKS		5

#define FIFO_SIZE 64
#define FIFO_INCR(x) (((x)+1)&((FIFO_SIZE)-1))
#define FIFO_EMPTY(x) ((x.head == x.tail))
#define FIFO_LENGTH(x) ((uint32_t)(x.head - x.tail))

typedef enum
{
    START,
    HEADER,
    PACKET,
    CRC32
} enum_serialState_t;

/* Structure of FIFO*/

typedef struct
{
	uint32_t head;
	uint32_t tail;
	uint8_t data[FIFO_SIZE];
} FIFO_t;

extern FIFO_t RX_FIFO;

typedef enum {
	CLT_FACTOR,
	MAT_FACTOR,
	EGO_FACTOR,
	MAF_FACTOR,
	FLASH_4,
	FLASH_5,
	CAN_BUF,
	OUTPC_DATAX,
	FLASH_8,
	FLASH_9,
	FLASH_10,
	FLASH_11,
	FLASH_12,
	FLASH_13,
	SIGNATURE,
	REV_NUM,
	BUF_2,
	SD_CARD,
	FLASH_18,
	FLASH_19,
	SD_READ,
	FLASH_21,
	FLASH_22,
	FLASH_23,
	FLASH_24,
	FLASH_25,
	TRIM_PAGE,
	FLASH_27,
	FLASH_28,
	FLASH_29,
	FLASH_30,
	FLASH_31,
	TOOTH_LOG = 0xf0,
	TRIG_LOG,
	COMP_LOG,
	SEC_LOG,
	MAP_LOG,
	MAF_LOG,
	ENG_LOG,
	ENG_MAP_LOG,
	ENG_MAF_LOG
} enum_tables_t;

typedef enum {
	OK,
	REALTIME_DATA,
	PAGE_DATA,
	CONFIG_ERROR,
	BURN_OK,
	PAGE10_OK,
	CAN_DATA,
	UNDER_RUN = 0x80,
	OVER_RUN,
	CRC_ERROR,
	UNKNOWN_COMMAND,
	OUT_OF_RANGE,
	BUSY,
	FLASH_LOCKED,
	SEQUENCE_FAILURE_1,
	SEQUENCE_FAILURE_2,
	CAN_QUEUE_FULL,
	CAN_TIMEOUT,
	CAN_FAILURE,
	PARITY_ERROR,
	FRAMING_ERROR,
	SERIAL_NOISE,
	TXMODE_RANGE,
	UNKNOWN
} enum_com_flags_t;

typedef struct
{
	uint16_t size;						// size of payload
	uint8_t command;					// command
	uint16_t offset;					// requested offset of data
	uint16_t length;					// requested length of data
	uint32_t crc;						// crc
	uint8_t data[MAX_PACKET_SIZE];
} packet_t;

extern void communicationTask(void const * argument);
extern uint8_t envelope_CDC_Transmit(uint8_t flag, uint8_t *src, uint16_t len);
extern void packetReset(void);

//extern USBD_HandleTypeDef hUsbDeviceFS;
extern packet_t packet;
extern FIFO_t FIFO;
//extern osThreadId communicationHandle;

#endif
