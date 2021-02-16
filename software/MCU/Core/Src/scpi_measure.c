/*
 * scpi_measure.c
 *
 *  Created on: Sep 6, 2020
 *      Author: grzegorz
 */

#include "scpi_measure.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "bsp.h"

extern uint8_t log_status;
extern uint8_t rx_data[6];
extern unsigned char  shiftRegisters[2];
extern struct cfg_struct dmm;
extern uint8_t new_data;

extern osThreadId_t MeasurTaskHandle;

extern UART_HandleTypeDef huart2;
extern uint8_t valid_data;


double get_voltage(uint8_t* adc_array, uint8_t it_adc, double cf)
{
  int64_t tmp1,tmp2;
  uint16_t adc_up, adc_total, adc_rundown;
  uint8_t adc_x;
  double retval;
  adc_up = (((uint16_t)(adc_array[0]))<<8)+(((uint16_t)(adc_array[1]))<<0);
  adc_total = (((uint16_t)(adc_array[2]&0x0F))<<8)+(((uint16_t)(adc_array[3]))<<0);
  adc_rundown = (((uint16_t)(adc_array[4]))<<8)+(((uint16_t)(adc_array[5]))<<0);
  adc_x = (adc_array[2]&0x80)>>7;

  if (adc_x==1)
  {
	  tmp1 = adc_rundown+(dmm.calibration.structure.adc_z_offset);
  }
  else
  {
	  tmp1 = -adc_rundown;
  }

  tmp2 = (adc_total+1)/2;
  tmp2 = tmp2-adc_up;
  tmp2 = tmp2*dmm.calibration.structure.adc_step;
  tmp2 = tmp2 - tmp1;
  retval = tmp2;							//tmp2 now holds ADC counts
  retval = retval/338.10;					//roughly map ADC counts to voltage
  retval = retval/((double)(it_adc));		//correction to integration time
  retval = retval / cf;
  return retval;
}

double get_measure(uint8_t adc_it, double adc_cf, uint8_t nplc)
{
	double sum = 0.0;
	double result = 0.0;
	uint8_t loop_max = 10;

	if(nplc > 3)
	{
		for(uint32_t x = 0; x < loop_max; x++)
		{
			HAL_UART_Init(&huart2);
			HAL_UART_Receive(&huart2, (uint8_t *)rx_data, 6, 400);
			HAL_UART_DeInit(&huart2);
			sum+= get_voltage(rx_data, adc_it, adc_cf);

		}

		result = sum/(double)loop_max;
	}
	else
	{
		HAL_UART_Init(&huart2);
		HAL_UART_Receive(&huart2, (uint8_t *)rx_data, 6, 400);
		HAL_UART_DeInit(&huart2);
		result = get_voltage(rx_data, adc_it, adc_cf);

	}

	return result;
}


double auto_zero()
{
	double adc_nz = 0.0;

	//meas_path(dmm.range, AUTO_ZERO);
	osDelay(pdMS_TO_TICKS(400));

	adc_nz = get_measure(dmm.range, dmm.adc_cf[dmm.nplc_index], dmm.nplc_index);
/*
	switch (dmm.range)
	{
		case 1:{
			adc_nz = adc_nz/(double)100000;
			adc_nz = adc_nz + dmm.calibration.structure.v_1V_offset;
			adc_nz = adc_nz * dmm.calibration.structure.v_1V_gain;
		}break;
		case 10:{
			adc_nz = adc_nz/(double)1000;
			adc_nz = adc_nz + dmm.calibration.structure.v_10V_offset;
			adc_nz = adc_nz * dmm.calibration.structure.v_10V_gain;

		}break;
		case 100:{
			adc_nz = adc_nz/(double)10;
			adc_nz = adc_nz + dmm.calibration.structure.v_100V_offset;
			adc_nz = adc_nz * dmm.calibration.structure.v_100V_gain;
		}break;
	}
*/
	adc_nz = (-1.0)*adc_nz;
	adc_nz = adc_nz/1000;

	//meas_path(dmm.range, NORM_MEAS);
	osDelay(pdMS_TO_TICKS(400));

	return adc_nz;
}






/*
 * MEASure:CURRent[:DC] [{<range>|AUTO|MIN|MAX|DEF} [,{<resolution>|MIN|MAX|DEF}]]
 *
 * @INFO:
 * Sets all measurement parameters and trigger parameters to their default values for DC current measurements and immediately triggers a measurement.
 * The results are sent directly to the instrument's output buffer.
 *
 * @PARAMETERS:
 *				<range>			100 μA|1 mA|10 mA|100 mA|1 A|3 A, default AUTO (autorange)
 *				<resolution>	TBD
 *
 */

scpi_result_t SCPI_MeasureCurrentDC(scpi_t * context)
{
	return SCPI_RES_OK;
}

/*
 * MEASure:CURRent[:DC]?
 *
 * @INFO:
 *
 */

scpi_result_t SCPI_MeasureCurrentDCQ(scpi_t * context)
{
	return SCPI_RES_OK;
}


/*
 * MEASure:[VOLtage][:DC] [{<range>|AUTO|MIN|MAX|DEF} [,{<resolution>|MIN|MAX|DEF}]]
 *
 * @INFO:
 * Sets all measurement parameters and trigger parameters to their default values for DC voltage ratio measurements
 * and immediately triggers a measurement. The results are sent directly to the instrument's output buffer.
 *
 * @PARAMETERS:
 *				<range>			100 mV, 1 V, 10 V, 100 V, AUTO (default) or DEFault
 *				<resolution>	TBD
 *
 */

scpi_result_t SCPI_MeasureVoltageDC(scpi_t * context)
{
	return SCPI_RES_OK;
}

/*
 * MEASure:[VOLtage][:DC]:RATio?
 *
 * @INFO:
 *
 */

scpi_result_t SCPI_MeasureVoltageDCQ(scpi_t * context)
{
	double adc_in = 0.0;
	double results[10] = {0.0};
	uint32_t index = 0;
	uint32_t samples = dmm.samples;



	if((ZERO_ONCE == dmm.zero_status) && (!dmm.zero_done))
	{
		dmm.zero_val = auto_zero();
		dmm.zero_done = 1;
	}

	while(samples--)
	{
		if(ZERO_ON == dmm.zero_status)
		{
			dmm.zero_val = auto_zero();
		}
		else if (ZERO_OFF == dmm.zero_status)
		{

		}

		adc_in = get_measure(dmm.range, dmm.adc_cf[dmm.nplc_index], dmm.nplc_index);
		adc_in = adc_in/1000;
		adc_in = adc_in + dmm.zero_val;

		switch (dmm.range)
		{
		case 1:{
			adc_in = adc_in/(double)100;
			adc_in = adc_in + dmm.calibration.structure.v_1V_offset;
			adc_in = adc_in * dmm.calibration.structure.v_1V_gain;
			//adc_in = adc_in + dmm.zero_val;

		}break;
		case 10:{
				adc_in = adc_in*(double)1.0;
				adc_in = adc_in + dmm.calibration.structure.v_10V_offset;
				adc_in = adc_in * dmm.calibration.structure.v_10V_gain;
				//adc_in = adc_in + dmm.zero_val;
			}break;
		case 100:{
				adc_in = adc_in*(double)100;
				adc_in = adc_in + dmm.calibration.structure.v_100V_offset;
				adc_in = adc_in * dmm.calibration.structure.v_100V_gain;
				//adc_in = adc_in + dmm.zero_val;

			}break;

		}

		results[index] = adc_in;
		index++;
	}

	SCPI_ResultArrayDouble(context, results, dmm.samples, SCPI_FORMAT_ASCII);
	return SCPI_RES_OK;
}

