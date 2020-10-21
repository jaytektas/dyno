#include "Tasks/communication.h"

#include "Timers/envelope.h"
#include "usbd_cdc_if.h"
#include "cmsis_os.h"
#include "main.h"
#include "crc32.h"
#include "config.h"
#include "status.h"
#include "usb_device.h"
#include "heartbeat.h"
#include "adc.h"
#include "hub.h"

packet_t packet;
FIFO_t FIFO;

/* communications task */
void communicationTask(void const * argument)
{
	/* USER CODE BEGIN communicationTask */
	/* Infinite loop */
uint16_t i;
uint32_t crc;
uint32_t src = 0, dst = 0, end = 0;
uint8_t c;
uint16_t seen = 0;
enum_serialState_t state= START;

	for (;;)
	{
		if (FIFO_EMPTY(FIFO))
		{
			osThreadYield();
			continue;
		}
		// copy FIFO

	    // 01 01 CM SZ SZ D[0]..D[SZSZ] CR CR CR CR
		// could be spread across multiple USB packets

		while (!FIFO_EMPTY(FIFO))
		{
			// turn timout off
			osTimerStop(envelopeTimeoutTimerHandle);

			c = FIFO.data[FIFO.tail];
			FIFO.tail = FIFO_INCR(FIFO.tail);

			if (state == START)
			{
				if (c != 0x01)
				{
					seen = 0;
					continue;
				}
				else
				{
					packet.data[seen++] = c;
					if (seen == 2) state = HEADER;
					continue;
				}
			}

			if (state == HEADER)
			{
				packet.data[seen++] = c;
	            if (seen >= INBOUND_HEADER)
	            {
	                state = PACKET;
	                packet.size = packet.data[PACKET_SIZE] << 8 | packet.data[PACKET_SIZE + 1];
					packet.command = packet.data[PACKET_COMMAND];

	    			if (packet.size > MAX_PACKET_SIZE)
	    			{
	    				// TODO: we should dequeue the FIFO messages
	    				envelope_CDC_Transmit(OVER_RUN, 0, 0);
	    				packetReset();
	    			}
	            }
	            continue;
			}

			if (state == PACKET)
			{
				packet.data[seen++] = c;
		        if (seen >= packet.size) state = CRC32;
			}

			// ? - Real time data
			// C - CRC of config
			// W - Write config
			// F - Save config to flash
			// I - Identify - returns firmware id
			// V - Version - returns version string
			// R - Read config
			// S - stepper related

			// execute the command?
			if (state == CRC32)
			{
				state = START;
				seen = 0;

				packet.crc = packet.data[packet.size - 4] << 24 | packet.data[packet.size - 3] << 16 | packet.data[packet.size - 2] << 8 | packet.data[packet.size - 1];

				if (packet.crc == crc32(packet.data, packet.size - PACKET_CRC))
				{
					// execute
					switch (packet.command)
					{
						case '?': // read status
							envelope_CDC_Transmit(REALTIME_DATA, (uint8_t *)&status, sizeof(status));
							packetReset();
							break;

						case 'R': // read config
							packet.offset = packet.data[PACKET_OFFSET] << 8 | packet.data[PACKET_OFFSET + 1];
							// TODO: sanity checks
							if (packet.length > PACKET_PAYLOAD)
							{
								envelope_CDC_Transmit(FRAMING_ERROR, 0, 0);
								packetReset();
								break;
							}
							envelope_CDC_Transmit(REALTIME_DATA, (uint8_t *)&config + packet.offset, packet.length);
							packetReset();
							break;

						case 'C': // read crc config
							packet.offset = *((uint16_t *) &packet.data[PACKET_OFFSET]);
							crc = crc32((uint8_t *)&config + packet.offset, packet.length);

							*((uint32_t *) &packet.data[OUTBOUND_HEADER]) = crc;
							envelope_CDC_Transmit(OK, (uint8_t *)&packet.data[OUTBOUND_HEADER], PACKET_CRC);
							packetReset();
							break;

						case 'W': // write config
							packet.offset = *((uint16_t *) &packet.data[PACKET_OFFSET]);

							if (packet.offset + packet.length > sizeof(config))
							{
								envelope_CDC_Transmit(OUT_OF_RANGE, 0, 0);
								packetReset();
								break;
							}

							portDISABLE_INTERRUPTS();
							uint8_t *ptr = ((uint8_t *)&config);
							ptr += INBOUND_HEADER;
							for (i = INBOUND_HEADER; i < packet.length - PACKET_CRC; i++)
								*ptr++ = packet.data[i];
							portENABLE_INTERRUPTS();
							envelope_CDC_Transmit(OK, 0, 0);
							packetReset();
							break;

						case 'F': // copy config to flash
							// copy flash to sector 11 because atollic is locking it so I can't fucking write to it
							HAL_FLASH_Unlock();
							FLASH_Erase_Sector(FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE_3);

							src = (uint32_t)&config;
							dst = (uint32_t)0x080E0000;
							end = src + sizeof(config_t)+3;

							while (src < end)
							{
								if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dst, *(uint32_t *)src) == HAL_OK)
								{
									src += 4;
									dst += 4;
								}
								else
								{
									__asm__("BKPT");
								}
							}
							HAL_FLASH_Lock();
							envelope_CDC_Transmit(BURN_OK, 0, 0);
							packetReset();
							break;

						case 'I': // read Identify
							envelope_CDC_Transmit(OK, config.identity, sizeof(config.identity));
							packetReset();
							break;

						case 'V': // read version
							envelope_CDC_Transmit(OK, config.version, sizeof(config.version));
							packetReset();
							break;

						default:
							envelope_CDC_Transmit(UNKNOWN_COMMAND, config.version, sizeof(config.version));
							packetReset();
							break;
					}
				}
				else
				{
					envelope_CDC_Transmit(CRC_ERROR, 0, 0);
					packetReset();
				}
			}
		}
		// we've received data but it hasn't been completed, need more input
		if (seen) osTimerStart(envelopeTimeoutTimerHandle, 50);
	}
	/* USER CODE END communicationTask */
}

// resets the packet header
void packetReset(void)
{
	packet.size = 0;
	packet.command = 0;
	packet.offset = 0;
	packet.length = 0;
	packet.crc = 0xFFFFFFFF;
	osTimerStop(envelopeTimeoutTimerHandle);
}

uint8_t envelope_CDC_Transmit(uint8_t flag, uint8_t *src, uint16_t len)
{
uint16_t i = 0;
uint32_t crc;
TickType_t ticks;

	packet.size = OUTBOUND_HEADER + len + PACKET_CRC;
	ticks = xTaskGetTickCount();

	if (packet.size  > MAX_PACKET_SIZE)
		__ASM("bkpt");

	packet.data[i++] = 1;
	packet.data[i++] = 1;
	packet.data[i++] = flag;
	packet.data[i++] = (packet.size) >> 8;
	packet.data[i++] = (packet.size) & 0xff;

	packet.data[i++] = ticks & 0xff;
	packet.data[i++] = (ticks >> 8) & 0xff;
	packet.data[i++] = (ticks >> 16) & 0xff;
	packet.data[i++] = (ticks >> 24);
	
	// copy len bytes into buffer
	while (len--)
		packet.data[i++] = *src++;

	// save crc into tx buffer
	crc = crc32(packet.data, packet.size - PACKET_CRC);
	packet.data[i++] = crc >> 24;
	packet.data[i++] = (crc >> 16) & 0xff;
	packet.data[i++] = (crc >> 8) & 0xff;
	packet.data[i++] = crc & 0xff;
	
	return CDC_Transmit_FS(packet.data, packet.size);
}

