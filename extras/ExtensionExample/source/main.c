/* Empty trace point defines */
#define MY_TRACE_POINT_A()
#define MY_TRACE_POINT_B()
#define MY_TRACE_POINT_C()
#define MY_TRACE_POINT_D()

#include <MyExtension.h>
#include <trcRecorder.h>
#include <RTOS.h>

int main(void)
{
	/* First initialize */
	xTraceInitialize();

	/* Enable extensions AFTER Initialize but BEFORE Start*/
	xMyExtensionEnable();
	
	/* Start tracing */
	xTraceEnable(TRC_START);

	vStartScheduler();

	return 0;
}

void something_a()
{
	/* Example trace point A */
	MY_TRACE_POINT_A();
}

void something_b()
{
	/* Example trace point B */
	MY_TRACE_POINT_B();
}

void something_c()
{
	/* Example trace point C */
	MY_TRACE_POINT_C();
}

void something_d()
{
	/* Example trace point D */
	MY_TRACE_POINT_D();
}
