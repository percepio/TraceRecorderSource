/*
 * Trace Recorder for Tracealyzer v4.5.2
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The implementation of the internal buffer.
 */

#include "trcRecorder.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "trcInternalBuffer.h"


/* The status codes for the pages of the internal trace buffer. */
#define PAGE_STATUS_FREE 0
#define PAGE_STATUS_WRITE 1
#define PAGE_STATUS_READ 2


typedef struct{
	uint16_t Status;  /* 16 bit to avoid implicit padding (warnings) */
	uint16_t BytesRemaining;
	char* WritePointer;
} PageType;


PageType PageInfo[TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT] TRC_CFG_RECORDER_DATA_ATTRIBUTE;


char* EventBuffer TRC_CFG_RECORDER_DATA_ATTRIBUTE;
uint32_t TotalBytesRemaining_LowWaterMark TRC_CFG_RECORDER_DATA_ATTRIBUTE;
uint32_t TotalBytesRemaining TRC_CFG_RECORDER_DATA_ATTRIBUTE;


/**
 * @brief Retrieve a buffer page to write to.
 * 
 * @param prevPage Index of previous page.
 * @return int Index of available page or -1 if no page is currently available.
 */
static int prvAllocateBufferPage(int prevPage)
{
	int index;
	int count = 0;

	index = (prevPage + 1) % (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT);

	while((PageInfo[index].Status != PAGE_STATUS_FREE) && (count ++ < (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT)))
	{
		index = (index + 1) % (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT);
	}

	if (PageInfo[index].Status == PAGE_STATUS_FREE)
	{
		return index;
	}

	return -1;
}

/**
 * @brief Mark a page read as complete.
 * 
 * @param pageIndex Index of page
 */
static void prvPageReadComplete(int pageIndex)
{
  	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();
	PageInfo[pageIndex].BytesRemaining = (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);
	PageInfo[pageIndex].WritePointer = &EventBuffer[pageIndex * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE)];
	PageInfo[pageIndex].Status = PAGE_STATUS_FREE;

	TotalBytesRemaining += (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);

	TRACE_EXIT_CRITICAL_SECTION();
}

/**
 * @brief Get the current buffer page index and remaining number of bytes.
 * 
 * @param bytesUsed Pointer to int where bytes used will be written.
 * @return int Index of page or -1 if there is no current page.
 */
static int prvGetBufferPage(int32_t* bytesUsed)
{
	static int8_t lastPage = -1;
	int count = 0;
  	int8_t index = (int8_t) ((lastPage + 1) % (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT));

	while((PageInfo[index].Status != PAGE_STATUS_READ) && (count++ < (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT)))
	{
		index = (int8_t)((index + 1) % (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT));
	}

	if (PageInfo[index].Status == PAGE_STATUS_READ)
	{
		*bytesUsed = (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE) - PageInfo[index].BytesRemaining;
		lastPage = index;
		return index;
	}

	*bytesUsed = 0;

	return -1;
}

void prvPagedEventBufferInit(char* buffer)
{
	TotalBytesRemaining_LowWaterMark = (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT)* (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);
	TotalBytesRemaining = (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT)* (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);

  	int i;
  	TRACE_ALLOC_CRITICAL_SECTION();
    
    EventBuffer = buffer;
    
	TRACE_ENTER_CRITICAL_SECTION();
	for (i = 0; i < (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT); i++)
	{
		PageInfo[i].BytesRemaining = (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE);
		PageInfo[i].WritePointer = &EventBuffer[i * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE)];
		PageInfo[i].Status = PAGE_STATUS_FREE;
	}
	TRACE_EXIT_CRITICAL_SECTION();
}

void* prvPagedEventBufferGetWritePointer(int sizeOfEvent)
{
	void* ret;
	static int currentWritePage = -1;

	if (currentWritePage == -1)
	{
	    currentWritePage = prvAllocateBufferPage(currentWritePage);
		if (currentWritePage == -1)
		{
		  	//DroppedEventCounter++;
			return NULL;
		}
	}

    if (PageInfo[currentWritePage].BytesRemaining - sizeOfEvent < 0)
	{
		PageInfo[currentWritePage].Status = PAGE_STATUS_READ;

		TotalBytesRemaining -= PageInfo[currentWritePage].BytesRemaining; // Last trailing bytes

		if (TotalBytesRemaining < TotalBytesRemaining_LowWaterMark)
		  TotalBytesRemaining_LowWaterMark = TotalBytesRemaining;

		currentWritePage = prvAllocateBufferPage(currentWritePage);
		if (currentWritePage == -1)
		{
		  //DroppedEventCounter++;
		  return NULL;
		}
	}
	ret = PageInfo[currentWritePage].WritePointer;
	PageInfo[currentWritePage].WritePointer += sizeOfEvent;
	PageInfo[currentWritePage].BytesRemaining = (uint16_t)(PageInfo[currentWritePage].BytesRemaining -sizeOfEvent);

	TotalBytesRemaining = (TotalBytesRemaining-(uint16_t)sizeOfEvent);

	if (TotalBytesRemaining < TotalBytesRemaining_LowWaterMark)
		TotalBytesRemaining_LowWaterMark = TotalBytesRemaining;

	return ret;
}

uint32_t prvPagedEventBufferTransfer(void)
{
	int8_t pageToTransfer = -1;
    int32_t bytesTransferredTotal = 0;
	int32_t bytesTransferredNow = 0;
	int32_t bytesToTransfer;

    pageToTransfer = (int8_t)prvGetBufferPage(&bytesToTransfer);

	/* bytesToTransfer now contains the number of "valid" bytes in the buffer page, that should be transmitted.
	There might be some unused junk bytes in the end, that must be ignored. */
    
    if (pageToTransfer > -1)
    {
        while (1)  /* Keep going until we have transferred all that we intended to */
        {
			if (TRC_STREAM_PORT_WRITE_DATA(
					&EventBuffer[pageToTransfer * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE) + bytesTransferredTotal],
					(uint32_t)(bytesToTransfer - bytesTransferredTotal),
					&bytesTransferredNow) == 0)
			{
				/* Write was successful. Update the number of transferred bytes. */
				bytesTransferredTotal += bytesTransferredNow;

				if (bytesTransferredTotal == bytesToTransfer)
				{
					/* All bytes have been transferred. Mark the buffer page as "Read Complete" (so it can be written to) and return OK. */
					prvPageReadComplete(pageToTransfer);
					return (uint32_t)bytesTransferredTotal;
				}
			}
			else
			{
				/* Some error from the streaming interface... */
				vTraceStop();
				return 0;
			}
		}
	}
	return 0;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)*/
