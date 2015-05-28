/*
	Molole - Mobots Low Level library
	An open source toolkit for robot programming using DsPICs

	Copyright (C) 2007--2011 Stephane Magnenat <stephane at magnenat dot net>,
	Philippe Retornaz <philippe dot retornaz at epfl dot ch>
	Mobots group (http://mobots.epfl.ch), Robotics system laboratory (http://lsro.epfl.ch)
	EPFL Ecole polytechnique federale de Lausanne (http://www.epfl.ch)

	See authors.txt for more details about other contributors.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published
	by the Free Software Foundation, version 3 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _MOLOLE_MOTOR_CSP_H
#define _MOLOLE_MOTOR_CSP_H

/** \addtogroup motor_csp */ 
/*@{*/

/** \file
        \brief A nested current, speed and position motor module.
*/

/** External callback to be called before the position or speed controller execute */
typedef void (*motor_csp_enc_cb)(void);

#define MOTOR_CSP_OVERCURRENT_ACTIVE 0
#define MOTOR_CSP_OVERCURRENT_CLEARED 1	
/** External callback to be called when overcurrent status change */
typedef void (*motor_csp_overcurrent) (int status);

typedef struct {
// Current PI part
	int *current_m; 				//! Current mesure
	int current_t;					//! Current target, automatically set if enable_s is true
	int kp_i;						//! KP value for current, must be >= 0
	int ki_i;						//! KI value for current, must be >= 0
	int scaler_i;					//! Scale factor for pwm output. 0 mean scaling disabled, must be >= 0
	long integral_i;				//! Integral term for current, internal use only.
	int pwm_min;					//! Minimum PWM value
	int pwm_max;					//! Maximum PWM value
	int pwm_output;					//! PWM output value
	int current_max;				//! Maximum current for speed PID output and current PI input
	int current_min;				//! Minimum current for speed PID output and current PI input
	unsigned char time_cst;			//! Motor winding time constant for heat dissipation in 128*current period
	int current_nominal;			//! Max DC current for the motor
	unsigned long square_c_iir;		//! Square current IIR filter
	unsigned long iir_sum;			//! Sum for the mean
	unsigned char _iir_counter;		//! IIR counter for mean
	unsigned char _over_status;		//! overcurrent internal status
	
	unsigned int prescaler_period;	//! Prescaler value for speed and position controller
	unsigned int prescaler_c;		//! Counter for prescaler, internal use only.
	
// 	Speed PID part
	int *speed_m;					//! Speed mesure
	int speed_t;					//! Speed target, automatically set if enable_p is true
	int kp_s;						//! KP value for speed, must be >= 0
	int ki_s;						//! KI value for speed, must be >= 0
	int kd_s;						//! KD value for speed, must be >= 0
	int scaler_s;					//! Scale factor for current output.  0 mean scaling disabled, must be >= 0
	long integral_s;				//! Integral value for speed, internal use only
	bool enable_s;					//! Enable speed PID
	int last_error_s;				//! Last speed error (for D term)
	
//	Position part
	void *position_m;				//! Position mesure
	void *position_t;				//! Position target
	int kp_p;						//! KP value for position, must be >= 0
	int kd_p;						//! KD value for position, must be >= 0
	int scaler_p;					//! Scale factor for speed output. 0 mean scaling disabled, must be >= 0
	bool enable_p;					//! Enable position PID
	int speed_max;					//! Maximum speed for position PD output
	int speed_min;					//! Minimum speed for position PD output
	long last_error_p;				//! Last position error (for D term)
	bool is_32bits;					//! True if the position is 32bits, false if it's 16bits
	
	motor_csp_enc_cb enc_up;		//! Encoder update callback pointer
	motor_csp_overcurrent ov_up;	//! Motor overcurrent callback pointer
	
	int sat_status;					//! 2bit-field of current controller saturation status, internal use only
} motor_csp_data;

// init 32bit, init 16bits

void motor_csp_step(motor_csp_data * d);
void motor_csp_init_32(motor_csp_data *d);
void motor_csp_init_16(motor_csp_data *d);

// 32bits / 16 bits => 32 bits implemented with two 32/16 => 16
unsigned long div32by16u(unsigned long a, unsigned int b);
long div32by16s(long a, int b);

// Some helper defines for aseba

#define MOTOR_VMDESC(name) 	{ 1, #name ".pid._period"}, \
							{ 1, #name ".pid._kp_i"}, \
							{ 1, #name ".pid._ki_i"}, \
							{ 1, #name ".pid._scaler_i"}, \
							{ 1, #name ".pwm"} , \
							{ 1, #name ".pid._prescaler_s"}, \
							{ 1, #name ".pid._current_speed_pulse" }, \
							{ 1, #name ".pid._target_speed_pulse" }, \
							{ 1, #name ".pid._kp_s" }, \
							{ 1, #name ".pid._ki_s" }, \
							{ 1, #name ".pid._kd_s" }, \
							{ 1, #name ".pid._scaler_s" }, \
							{ 1, #name ".pid._speed_max_pulse"}, \
							{ 1, #name ".pid._kp_p" }, \
							{ 1, #name ".pid._kd_p" }, \
							{ 1, #name ".pid._scaler_p"}, \
							{ 2, #name ".pid._target_position_pulse"}, \
							{ 1, #name ".pid.current_max" }, \
							{ 1, #name ".pid.speed_max"}, \
							{ 1, #name ".pid._nominal_current"}, \
							{ 1, #name ".pid._time_cst"}, \
							{ 1, #name ".pid._override"}, \
							{ 1, #name ".pid.enable"}, \
							{ 2, #name ".enc._pulse"}, \
							{ 1, #name ".current"}, \
							{ 1, #name ".speed" }, \
							{ 1, #name ".position" }, \
							{ 1, #name ".pid.target_current"}, \
							{ 1, #name ".pid.target_speed"}, \
							{ 1, #name ".pid.target_position"}, \
							{ 1, #name "._raw_current"},
							 
							
#define MOTOR_VMVAR(name)		sint16 name##_pid_period; \
								sint16 name##_kp_i; \
								sint16 name##_ki_i; \
								sint16 name##_scaler_i; \
								sint16 name##_pwm_output; \
								sint16 name##_prescaler_s; \
								sint16 name##_speed_read; \
								sint16 name##_speed_target; \
								sint16 name##_kp_s; \
								sint16 name##_ki_s; \
								sint16 name##_kd_s; \
								sint16 name##_scaler_s; \
								sint16 name##_speed_max; \
								sint16 name##_kp_p; \
								sint16 name##_kd_p; \
								sint16 name##_scaler_p; \
								sint16 name##_position_target[2]; \
								sint16 name##_current_max; \
								sint16 name##_speed_max_ext; \
								sint16 name##_current_nominal; \
								sint16 name##_time_cst;  \
								sint16 name##_override; \
								sint16 name##_pid_enable; \
								sint16 name##_position[2]; \
								sint16 name##_current; \
								sint16 name##_speed_ext; \
								sint16 name##_position_ext; \
								sint16 name##_current_t; \
								sint16 name##_target_speed_ext; \
								sint16 name##_target_position_ext; \
								sint16 name##_raw_current; 
								
								
							
#define MOTOR_PRIVSET(name)		int name##_kp_i;					/* 0 */ \
								int name##_ki_i;					/* 1 */ \
								int name##_scaler_i;				/* 2 */ \
								int name##_prescaler_s;				/* 3 */ \
								int name##_kp_s;					/* 4 */ \
								int name##_kd_s;					/* 5 */ \
								int name##_ki_s;					/* 6 */ \
								int name##_scaler_s;				/* 7 */ \
								int name##_current_max;				/* 8 */ \
								int name##_nominal_current;			/* 9 */ \
								int name##_time_cst;				/* 10 */ \
								int name##_speed_max;				/* 11 */ \
								int name##_kp_p;					/* 12 */ \
								int name##_kd_p;					/* 13 */ \
								int name##_scaler_p;				/* 14 */ \
								int name##_pid_period;				/* 15 */ \
								int name##_raw_current_offset;		/* 16 */ \
								int name##_position_max;			/* 17 */ \
								int name##_position_min;			/* 18 */ 
								
#define MOTOR_ASEBA_WRITE(name) 	static int name##_old_period; static int name##_old_enable; 							\
		if(vmVariables.name##_pid_period != name##_old_period || name##_old_enable != vmVariables.name##_pid_enable) { 	\
		name##_old_period = vmVariables.name##_pid_period; 																\
		name##_old_enable = vmVariables.name##_pid_enable; 																\
		if(vmVariables.name##_pid_period && name##_old_enable) { 															\
			int temp = abs(vmVariables.name##_pid_period);			 													\
			if(temp > 400) 																									\
				temp = 400; 																								\
			switch(name##_old_enable) { 																					\
				case 1: 																									\
					name##_pid.enable_s = 0; 																				\
					name##_pid.enable_p = 0; 																				\
					break; 																									\
				case 2: 																									\
					name##_pid.enable_s = 1; 																				\
					name##_pid.enable_p = 0; 																				\
					break; 																									\
				default: 																									\
				case 3: 																									\
					name##_pid.enable_s = 1; 																				\
					name##_pid.enable_p = 1; 																				\
					break; 																									\
			} 																												\
			name##_pid.kp_s = vmVariables.name##_kp_s; 																	\
			name##_pid.kd_s = vmVariables.name##_kd_s; 																	\
			name##_pid.ki_s = vmVariables.name##_ki_s; 																	\
			name##_pid.scaler_i = vmVariables.name##_scaler_i; 															\
			name##_pid.prescaler_period = vmVariables.name##_prescaler_s; 												\
			name##_pid.kp_i = vmVariables.name##_kp_i; 																	\
			name##_pid.ki_i = vmVariables.name##_ki_i; 																	\
			name##_pid.scaler_s = vmVariables.name##_scaler_s; 															\
			if(vmVariables.name##_pid_period > 0) { 																		\
				if(vmVariables.name##_time_cst > 255L * 128 * vmVariables.name##_pid_period) { 							\
					name##_pid.time_cst = 255; 																				\
					vmVariables.name##_time_cst = 255L * 128 * vmVariables.name##_pid_period;	 						\
				} else { 																									\
					name##_pid.time_cst = vmVariables.name##_time_cst / (vmVariables.name##_pid_period * 128L); 		\
				} 																											\
			} 																												\
			name##_pid.current_nominal = name##_ma_to_raw(vmVariables.name##_current_nominal); 							\
			name##_pid.scaler_p = vmVariables.name##_scaler_p; 															\
			name##_pid.kp_p = vmVariables.name##_kp_p; 																	\
			name##_pid.kd_p = vmVariables.name##_kd_p; 																	\
			timer_set_period(name##_PID_TIMER, temp, 3); 																	\
			timer_enable(name##_PID_TIMER); 																				\
		} else { 																											\
			timer_disable(name##_PID_TIMER); 																				\
			pwm_set_duty(name##_PWM, 0);																					\
		} 																													\
	} 																														\
	name##_pid.current_max = name##_ma_to_raw(vmVariables.name##_current_max); 											\
	name##_pid.current_min = name##_ma_to_raw(-vmVariables.name##_current_max); 											\
	vmVariables.name##_speed_max = name##_speed_to_pulse(vmVariables.name##_speed_max_ext);								\
	name##_pid.speed_max = vmVariables.name##_speed_max; 																	\
	name##_pid.speed_min = -vmVariables.name##_speed_max; 																\
	if(!name##_pid.enable_s)																							\
		name##_pid.current_t = name##_ma_to_raw(vmVariables.name##_current_t); 											\
	if(!name##_pid.enable_p) { 																								\
		vmVariables.name##_speed_target = name##_speed_to_pulse(vmVariables.name##_target_speed_ext);					\
		name##_pid.speed_t = vmVariables.name##_speed_target; 															\
	}																													\
	*((long *) vmVariables.name##_position_target) = name##_position_to_pulse(vmVariables.name##_target_position_ext);	\
	if(vmVariables.name##_pid_period <= 0)																				\
		pwm_set_duty(name##_PWM, vmVariables.name##_pwm_output);		
		
#define MOTOR_ASEBA_READ(name)	 if(vmVariables.name##_pid_period > 0	 && vmVariables.name##_pid_enable) 				\
			vmVariables.name##_pwm_output = name##_pid.pwm_output;														\
		if(name##_pid.enable_s)																								\
			vmVariables.name##_current_t = name##_raw_to_ma( name##_pid.current_t);										\
		if(name##_pid.enable_p)	{																							\
			vmVariables.name##_speed_target = name##_pid.speed_t;														\
			vmVariables.name##_target_speed_ext = name##_pulse_to_speed(vmVariables.name##_speed_target);				\
		}																												\
		vmVariables.name##_speed_ext = name##_pulse_to_speed(vmVariables.name##_speed_read);							\
		vmVariables.name##_position_ext = name##_pulse_to_position((long *) vmVariables.name##_position);						
					
		

		
#define MOTOR_LOAD_CONF(name) 	name##_pid.kp_s = vmVariables.name##_kp_s = settings.name##_kp_s;						\
		name##_pid.kd_s = vmVariables.name##_kd_s = settings.name##_kd_s;												\
		name##_pid.ki_s = vmVariables.name##_ki_s = settings.name##_ki_s;												\
		name##_pid.scaler_i = vmVariables.name##_scaler_i = settings.name##_scaler_i;									\
		name##_pid.prescaler_period = vmVariables.name##_prescaler_s = settings.name##_prescaler_s;						\
		name##_pid.kp_i = vmVariables.name##_kp_i = settings.name##_kp_i;												\
		name##_pid.ki_i = vmVariables.name##_ki_i = settings.name##_ki_i;												\
		name##_pid.scaler_s = vmVariables.name##_scaler_s = settings.name##_scaler_s;									\
		name##_pid.current_max = name##_ma_to_raw(settings.name##_current_max);											\
		vmVariables.name##_current_max = settings.name##_current_max;													\
		name##_pid.current_min = - name##_pid.current_max;																	\
		vmVariables.name##_current_nominal = settings.name##_nominal_current;											\
		name##_pid.current_nominal = name##_ma_to_raw(settings.name##_nominal_current);									\
		name##_pid.time_cst = vmVariables.name##_time_cst = settings.name##_time_cst;									\
		vmVariables.name##_speed_max_ext = settings.name##_speed_max;															\
		name##_pid.speed_max = vmVariables.name##_speed_max = name##_speed_to_pulse(vmVariables.name##_speed_max_ext);	\
		name##_pid.speed_min = -name##_pid.speed_max;																	\
		name##_pid.kp_p = vmVariables.name##_kp_p = settings.name##_kp_p;												\
		name##_pid.kd_p = vmVariables.name##_kd_p = settings.name##_kd_p;												\
		name##_pid.scaler_p = vmVariables.name##_scaler_p = settings.name##_scaler_p;										\
		if(settings.name##_pid_period > 0 && settings.name##_pid_period < 400) {										\
			vmVariables.name##_pid_period = settings.name##_pid_period;													\
			/*timer_set_period( name##_PID_TIMER, vmVariables.name##_pid_period, 3);*/										\
			vmVariables.name##_pid_enable = 0;																			\
			/*timer_enable(name##_PID_TIMER);*/																					\
		}
		
#define MOTOR_INIT(name) motor_csp_init_32(&name##_pid); 				\
		name##_pid.current_m = (int *) &vmVariables.name##_raw_current; \
		name##_pid.pwm_min = -1401;										\
		name##_pid.pwm_max = 1401; 										\
		name##_pid.speed_m = (int *) &vmVariables.name##_speed_read; 	\
		name##_pid.position_m = vmVariables.name##_position; 			\
		name##_pid.position_t = vmVariables.name##_position_target;		\
		name##_pid.enc_up = name##_update_cb;							\
		name##_pid.ov_up = name##_overcurrent_cb;
		
#define MOTOR_ONE_STEP(name) motor_csp_step(&name##_pid);														\
         pwm_set_duty(name##_PWM, name##_pid.pwm_output);														\
         if((settings.name##_position_max != settings.name##_position_min) && !vmVariables.name##_override) {	\
	     	int pos;																							\
	     	pos = name##_pulse_to_position((long *) vmVariables.name##_position);								\
	     	if(pos < settings.name##_position_min)																\
	     		name##_pid.pwm_min = 0;																			\
	     	else																								\
	     		name##_pid.pwm_min = -1401;																		\
	     																										\
	     	if(pos > settings.name##_position_max)																\
	     		name##_pid.pwm_max = 0;																			\
	     	else																								\
	     		name##_pid.pwm_max = 1401;																		\
	     }

/*@}*/

#endif
