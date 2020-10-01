/*
 * scpi_misc.c
 *
 *  Created on: Sep 6, 2020
 *      Author: grzegorz
 */

#include "scpi_misc.h"
#include "bsp.h"
#include "bsp_switch.h"

 scpi_choice_def_t route_input_relay_select[] =
 {
 		{"H",  0x01},
 		{"L",  0x02},
 		{"I",  0x04},
		{"LS", 0x08},
		{"HS", 0x10},
		{"ALL",0x1F},
 		SCPI_CHOICE_LIST_END
 };

const char* arr_str[5] = {"H", "L", "I", "LS", "HS"};

scpi_result_t SCPI_TestAllQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_Abort(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_FetchQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_InitiateImmediate(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_RQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_ReadQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_SampleCount(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_SampleCountQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_RouteOpen(scpi_t * context)
{
	int32_t param_relay;
	uint8_t param_tmp = 0;
	uint8_t index = 0;

	if(!SCPI_ParamChoice(context, route_input_relay_select, &param_relay, TRUE))
	{
		return SCPI_RES_ERR;
	}


	param_tmp =(uint8_t)(param_relay & 0xFF);

	for(uint8_t i = 0; i < 5; i++)
	{
		index = index << 1;
		(index == param_tmp) ? (board_current.relay.status[i] = 0) : (board_current.relay.status[i] = 1);
	}

	SWITCH_ULN2003A_Control(param_relay, 0);

	return SCPI_RES_OK;
}

scpi_result_t SCPI_RouteOpenQ(scpi_t * context)
{
	return SCPI_RES_OK;
}

scpi_result_t SCPI_RouteClose(scpi_t * context)
{
	int32_t param_relay;
	uint8_t param_tmp = 0;
	uint8_t index = 0;

	if(!SCPI_ParamChoice(context, route_input_relay_select, &param_relay, TRUE))
	{
		return SCPI_RES_ERR;
	}

	param_tmp =(uint8_t)(param_relay & 0xFF);

	for(uint8_t i = 0; i < 5; i++)
	{
		index = index << 1;
		(index == param_tmp) ? (board_current.relay.status[i] = 1) : (board_current.relay.status[i] = 0);
	}

	SWITCH_ULN2003A_Control(param_relay, 1);

	return SCPI_RES_OK;
}

scpi_result_t SCPI_RouteCloseQ(scpi_t * context)
{
	const char* arr_str[5] = {"H", "L", "I", "LS", "HS"};

	for(uint8_t i = 0; i < 5; i++)
	{
		if(board_current.relay.status[i])
		{
			SCPI_ResultCharacters(context, arr_str[5], strlen(arr_str[5]));
		}
	}

	return SCPI_RES_OK;
}
