#include <trcRecorder.h>

/* The handle to the runnable */
TraceRunnableHandle_t xMyRunnableHandle1 = 0;
TraceRunnableHandle_t xMyRunnableHandle2 = 0;
TraceRunnableHandle_t xMyRunnableHandle3 = 0;
const char* MY_RUNNABLE_NAME1 = "MyRunnable1";
const char* MY_RUNNABLE_NAME2 = "MyRunnable2";
const char* MY_RUNNABLE_NAME3 = "MyRunnable3";

void looper(void);

int main(void)
{
	/* First initialize */
	xTraceInitialize();
	
	/* Start tracing */
	xTraceEnable(TRC_START);

	looper();

	return 0;
}

void looper(void)
{
	/* Runnable in EntryTable */
	xTraceRunnableRegister(MY_RUNNABLE_NAME1, TRC_RUNNABLE_REGISTER_METHOD_USE_ENTRY_TABLE, &xMyRunnableHandle1);

	/* Runnable from string address */
	xTraceRunnableRegister(MY_RUNNABLE_NAME2, TRC_RUNNABLE_REGISTER_METHOD_USE_STRING_ADDRESS, &xMyRunnableHandle2);

	/* Runnable from Handle address */
	xTraceRunnableRegister(MY_RUNNABLE_NAME3, TRC_RUNNABLE_REGISTER_METHOD_USE_HANDLE_ADDRESS, &xMyRunnableHandle3);

	for(;;)
	{
		xTraceRunnableStart(xMyRunnableHandle1);
		/* call runnable 1 */
		xTraceRunnableStop();

		xTraceRunnableStart(xMyRunnableHandle2);
		/* call runnable 2 */
		xTraceRunnableStop();

		xTraceRunnableStart(xMyRunnableHandle3);
		/* call runnable 3 */
		xTraceRunnableStop();
	}
}
