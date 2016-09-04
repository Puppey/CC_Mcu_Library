/*
 * hp_buck-boost_control.h
 *
 *  Created on: 2015Äê8ÔÂ20ÈÕ
 *      Author: FlyerPower
 */

#ifndef HP_BUCK_BOOST_CONTROL_H_
#define HP_BUCK_BOOST_CONTROL_H_

#include "DSP28x_Project.h"

void smps_init(void);
void get_volt_and_current_adc(void);
void buck_boost_control(float32 Vout_value);

#endif /* HP_BUCK_BOOST_CONTROL_H_ */
