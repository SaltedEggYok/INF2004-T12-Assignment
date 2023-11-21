#include "pid_controller.h"
// To be removed
//
// #include "hardware/pwm.h"


// uint get_slice_num(int motor_no)
// {
//     uint slice_num = pwm_gpio_to_slice_num(motor_no);

//     return slice_num;
// }

// void update_speed(uint *slice_num, enum pwm_chan channel, float duty_cycle)
// {
//     pwm_set_chan_level(*slice_num,channel,CLK_CYCLE_NO * duty_cycle);
// }

char determine_wheel(float inital_angle, float curr_angle)
{
    char chosen_wheel = {0};
    if(inital_angle-curr_angle < 0)
    {
        chosen_wheel = 'L';
    }
    else
    {
        chosen_wheel = 'R';
    }
    return chosen_wheel;
}


float compute_pid(float target_spd, float curr_spd, float *integral, float *prev_error)
{
    // Initialize PID gains
    //
    float Kp = 1.0; 
    float Ki = 0.2;
    float Kd = 0.05; 
    // float Kp = 0.05; 
    // float Ki = 0.98; 
    // float Kd = 0.0525; 

    // Get the current error
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


// int main()
// {
//     // stdio_init_all();
//     float current_pos = 0.0; // Current Position
//     float target_pos = 0.0; // Desired position
//     float integral = 0.0;  // Integral term
//     float prev_error = 0.0;  // Previous error term

//     float l_speed = 2.87;
//     float r_speed = 4.0;
//     // Infinite loop
//     while(1)
//     {
//         char chosen_wheel = {0};
//         float modified_spd = 0.0;
//         float updated_duty_cycle = 0.0;
//         chosen_wheel = determine_wheel(initial_angle,car_angle);
//         // Get slice num which corresponds to each wheel
//         //
//         // uint l_slice_num = get_slice_num(LEFT_MOTOR);
//         // uint r_slice_num = get_slice_num(RIGHT_MOTOR);
//         printf("Chosen_wheel: %c\n",chosen_wheel);
//         printf("L_spd: %f\n",l_speed);
//         printf("R_spd: %f\n",r_speed);
//         if(l_speed != r_speed)
//         {
//             if(chosen_wheel == 'L')
//             {
//                 current_pos = l_speed;
//                 target_pos = r_speed;
//                 // Compute Error with PID gains
//                 //
//                 modified_spd = compute_pid(target_pos, current_pos, &integral, &prev_error);
//                 // Update the current duty cycle with the error for the target wheel
//                 //
//                 duty_cycle += modified_spd;
//                 // Update Duty Cycle of Target wheel
//                 //
//                 // update_speed(&l_slice_num,PWM_CHAN_A,duty_cycle);
//                 car_angle -= 1.0;
//                 l_speed += modified_spd;
//             }
//             else
//             {
//                 current_pos = r_speed;
//                 target_pos = l_speed;
//                 modified_spd = compute_pid(target_pos, current_pos, &integral, &prev_error);            
//                 // updated_duty_cycle = modified_spd / time_elapsed;
//                 duty_cycle += modified_spd;
//                 // update_speed(&r_slice_num,PWM_CHAN_B,duty_cycle);
//                 car_angle += 1.0;
//                 r_speed += modified_spd;
//             }
//             printf("Modified spd: %f\n",modified_spd);
//             printf("Current Car Angled: %f\n",car_angle);
//             printf("Updated_duty_cycle: %f\n",updated_duty_cycle);
//             // sleep_ms(1000);
//         }
//         else
//         {
//             printf("Complete\n");
//             return 0;
//         }
//     }
//     return 0;
// }
