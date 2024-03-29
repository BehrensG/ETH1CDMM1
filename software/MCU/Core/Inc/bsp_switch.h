/*
 * bsp_switch.h
 *
 *  Created on: Sep 10, 2020
 *      Author: grzegorz
 */

#ifndef INC_BSP_SWITCH_H_
#define INC_BSP_SWITCH_H_

#include "bsp.h"
#include "main.h"

#define U1011 1
#define U1006 2

#define CXN_REL1 0x02
#define CXN_REL2 0x04
#define CXN_REL3 0x08
#define CXN_REL4 0x10
#define CXN_REL5 0x20

#define DCV_L_CTR	0x0001U
#define VZERO_CTR	0x0002U
#define R4W_LO_CTR	0x0004U
#define MZ_CTR		0x0008U
#define DCV_H_CTR	0x0010U
#define MC_CTR		0x0020U
#define AMPS_CTR	0x0040U
#define R4W_HI_CTR	0x0080U
#define DCV_A_CTR	0x0100U
#define L_RLY		0x2000U
#define XH_RLY		0x4000U
#define XO_A_RLY 	0x8000U

#define GAIN_CIRCUIT1 0
#define GAIN_CIRCUIT2 1

#define LOAD_10MEG_OFF 0
#define LOAD_10MEG_ON 1

#define TMUX6104_MUX1 1
#define TMUX6104_MUX2 2

#define GAIN_1		1
#define GAIN_10		10
#define GAIN_100	100

#define SWITCH_OFF		0
#define SWITCH_ON		1

#define HE3621_REL_COUNT 5

typedef union
{
	uint16_t word16;
	uint8_t bytes[2];
}switch_data_t;



void SWITCH_ULN2003A_Control(uint8_t relay, uint8_t state);
void SWITCH_ULN2003A_ClearAll();

BSP_StatusTypeDef SWITCH_MCZ33996_Control(uint16_t relay, uint8_t state);
BSP_StatusTypeDef SWITCH_MCZ33996_ClearAll();

void SWITCH_DG419_Control(uint8_t gain_circuit);
void SWITCH_9012_Control(uint8_t select_10meg_load);

void SWITCH_TMUX6104_Control(uint8_t select_mux, uint8_t select_gain);
void SWITCH_TMUX6104_ClearAll();

#endif /* INC_BSP_SWITCH_H_ */
