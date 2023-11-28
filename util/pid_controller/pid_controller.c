#include "pid_controller.h"
/*
  @brief: computes the value to be added to the duty cycle for speed scaling
  @params: target_speed: Speed of the target wheel in cm/s
           curr_speed: Speed of the current wheel in cm/s 
	       *integral: pointer to the integral value
           *prev_error: pointer to the prev error
  @return: duty_cycle_response: success
*/
float compute_pid(float target_spd, float curr_spd, float *integral, float *prev_error)
{
    // Initialize PID gains
    //
    float Kp = 5.0; 
    float Ki = 0.3;
    float Kd = 0.05; 
    // Computes the error
    //
    printf("Target Spd: %f\n",target_spd);
    printf("Current Spd: %f\n",curr_spd);   
    float error = (target_spd - curr_spd);
    printf("Error: %f\n",error);
    // Update the Integral
    //
    *integral += error;
    // Get derivative
    // 
    float derivative = error - *prev_error;

    // Modified duty_cycle
    //
    printf("Integral: %f\n",*integral);
    printf("Derivative: %f\n",derivative);
    float duty_cycle_response = (Kp * error) + (Ki * (*integral)) + (Kd *  derivative); 

    // Update the error
    //
    printf("Duty Cycle Response: %f\n",duty_cycle_response);
    *prev_error = error;
    printf("Prev Error: %f\n",*prev_error);

    return duty_cycle_response;
}
