/*
 * config.c
 *
 *  Created on: 28 Mar 2018
 *      Author: jay
 */

#include "config.h"

// flash config
const config_t DefaultConfig =
{
		"JayTek Dynamometer (C) 2020",				// identity
		"Alpha",									// version
		// hub trigger capture edge(s)
		{ EDGE_RISING, EDGE_RISING, EDGE_RISING, EDGE_RISING },
		EDGE_RISING,
		{// rpm capture edge
				{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 } //ADC filtering
		}
};

// ram config
config_t config;
