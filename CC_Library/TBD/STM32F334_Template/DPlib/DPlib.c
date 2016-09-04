

#include "DPlib.h"
#include "system_typedef.h"

struct {

	float32	VIPWR;
	float32	VOPWR;
	float32	IIPWR;
	float32	IOPWR;
	float32	PWRIN;
	float32	PWROUT;
	float32 EIFFC;

}Info;

struct {

	float32 (*vipwr)(void);
	float32 (*vopwr)(void);
	float32 (*iipwr)(void);
	float32 (*iopwr)(void);
	float32 (*pwrin)(void);
	float32 (*pwrout)(void);
	float32 (*eiffc)(void);

}Get_Value;

uint16 Fault(void)
{
	return ERR_OK;
}


