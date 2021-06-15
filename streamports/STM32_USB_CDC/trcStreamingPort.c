/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use USB CDC as streaming channel.
 * The example is for STM32 using STM32Cube.
 */

#include "trcRecorder.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include "stdint.h"

typedef struct{
	uint32_t idx;
	uint8_t data[64];
}recBuf;

recBuf commandBuffer;

static int8_t CDC_Receive_FS_modified  (uint8_t* pbuf, uint32_t *Len);

extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

static int8_t (* CDC_Receive_FS)(uint8_t *Buf, uint32_t *Len);

static int8_t CDC_Receive_FS_modified (uint8_t* Buf, uint32_t *Len)
{
	for( uint32_t i=0;i<* Len;i++)
	{		
		commandBuffer.data[commandBuffer.idx]=Buf[i];
		commandBuffer.idx++;
	}	

	CDC_Receive_FS(Buf, Len);

	return (USBD_OK);
}


void trcCDCInit(void)
{

	/* Store the original "Receive" function, from the static initialization */
	CDC_Receive_FS = USBD_Interface_fops_FS.Receive;

	/* Update the function pointer with our modified variant */
	USBD_Interface_fops_FS.Receive = CDC_Receive_FS_modified;

	MX_USB_DEVICE_Init();
}

/* The READ function, used in trcStreamingPort.h */
int32_t trcCDCReceive(void *data, uint32_t size, int32_t* NumBytes)
{
	uint32_t i,diff;

	if(commandBuffer.idx>0)
	{
		if (size >= commandBuffer.idx) // more than what is stored, number of bytes will be .idx
		{
			memcpy(data,commandBuffer.data, commandBuffer.idx);
			*NumBytes=commandBuffer.idx;
			commandBuffer.idx=0; // Make the buffer ready for a new command
		}
		else  //If some data in the buffer is not read
		{
			diff = commandBuffer.idx-size;
			memcpy(data,commandBuffer.data, size);
			for(i=0;i<diff;i++)
			{
				commandBuffer.data[i]=commandBuffer.data[i+size];
			}
			*NumBytes=size;
			commandBuffer.idx=diff;
		}
	}
	else
	{
		*NumBytes=0;
	}
	return 0;
}



/* The WRITE function, used in trcStreamingPort.h */
int32_t trcCDCTransmit(void* data, uint32_t size, int32_t * noOfBytesSent )
{
	static int fail_counter = 0;

	int32_t result;

	*noOfBytesSent = 0;

	result=CDC_Transmit_FS(data, size);
	
	if (result == USBD_OK)
	{
		fail_counter = 0;
		*noOfBytesSent = size;
		return 0;
	}
	else
	{
		fail_counter++;

		/* We keep trying to send more data. If busy, we delay for a while. This function will be called again afterwards. */
		TRC_OS_DELAY( TRC_CFG_DELAY_ON_BUSY );

		if (fail_counter == 100)
		{
			/* If many unsuccessful attempts in a row, something is very wrong. Returning -1 will stop the recorder. */
			return -1;
		}
	}

	return 0;
}


#endif	/*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
#endif  /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

