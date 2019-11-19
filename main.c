/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <stdlib.h>
/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
    // zumo wrestling task 1
void Go_Stop (void);
void Stay_in_Circle (void);
struct sensors_ dig;
void tankturn_right();
void tankturn_left();
void random_reverse();
void Go_to_White();
struct accData_ data;
uint8_t speed;
uint32_t delay;
uint8_t f_speed;
uint8_t b_speed;


void zmain(void)
{
    struct accData_ data;
	TickType_t start_time, stop_time, execution_time, hit_time;
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000

    for(;;)
    {
    	LSM303D_Start();
        // read digital values that are based on threshold. 0 = white, 1 = black
        reflectance_digital(&dig); 
        printf("DIG l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);         
        vTaskDelay(0);
       if (SW1_Read()== 0)// button press
        {
            Go_Stop();
            print_mqtt("Zumo045/ready","zumo");
            IR_Start(); // start IR receiving
            IR_flush(); // clear IR receive buffer
   		    IR_wait();
   		    start_time = xTaskGetTickCount( );
   		    print_mqtt("Zumo045/start","%d",start_time);
            Go_to_White();
			Stay_in_Circle ();
			motor_forward(0,0);
		    stop_time = xTaskGetTickCount( );
   		    print_mqtt("Zumo045/stop","%d",stop_time);
        	execution_time = stop_time - start_time;
   		    print_mqtt("Zumo045/time","%d",execution_time);
			

   		    

        }
       else
        motor_forward(0,0);

    }
}

void Go_Stop (void)//goes forward and stops at the black line
{
   
	motor_start();
	motor_forward(0,0);
    
   while (1)
   {
       reflectance_digital(&dig);
       while ((dig.l3 == 0) && (dig.r3 == 0))
       {
           motor_forward(100,0);
           reflectance_digital(&dig);
        }
       if ((dig.l3 == 1) && (dig.r3 == 1))
       {
            motor_forward(0,0);
            reflectance_digital(&dig);
            break;
        }
    }
}
void Stay_in_Circle (void)//goes forward until meets the black line than stops, moves back and does a random reverse
{
    TickType_t hit_time;
	motor_start();
	motor_forward(0,0);
    int n = rand() %2;

    while(1)
	{
       LSM303D_Read_Acc(&data);
	   reflectance_digital(&dig);
  
    if ((dig.l1 == 1) || (dig.r1 == 1))
       {
            motor_forward(0,0);
            motor_backward(200,200); // moving backward
  
            if (SW1_Read()== 0)
            {
            motor_forward(0,0);
            break;
            }
            else if (n == 0)
            {
        	tankturn_left(100,100,450);//turn first 
            }
            else if (n==1)
            {
        	tankturn_right(100,100,450);
            }
            else if((data.accX > 6000))
            {
                hit_time = xTaskGetTickCount( );
                vTaskDelay(100);
                print_mqtt("Zumo045/hit"," %d", hit_time);
            }  
             
        }     
    else if ((dig.l1 == 0) && (dig.r1 == 0))
       {   
            motor_forward(225,0);
            
            if (SW1_Read()== 0)
            {
                motor_forward(0,0);
            break;
            } 
            else if((data.accX < -6000))
            {
                hit_time = xTaskGetTickCount( );
                vTaskDelay(100);
                print_mqtt("Zumo045/hit"," %d", hit_time);
            } 
            
        }
        

    	
	}
	
}

void tankturn_right(f_speed, b_speed, delay)
{
	MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(1);     // set RightMotor backward mode
    PWM_WriteCompare1(f_speed); 
    PWM_WriteCompare2(b_speed); 
    vTaskDelay(delay);
}
void tankturn_left(f_speed, b_speed, delay)
{
	MotorDirLeft_Write(1);      // set LeftMotor backward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(f_speed); 
    PWM_WriteCompare2(b_speed); 
    vTaskDelay(delay);
}

void Go_to_White(void)//goes off from the black line and stops on white
{
while(1)
{
reflectance_digital(&dig);
    if ((dig.l3 == 0) && (dig.r3 == 0))
    {
    motor_forward(0,0);
    break;
    }
    else
    {
    motor_forward(100,0);
    vTaskDelay(10);
    }
}
}
#endif

#if 0
//Task 2 linefollower

void Go_Stop ();
void Go_to_White();
struct sensors_ dig;
void Follow_Line_Stop();
void tankturn_right();
void tankturn_left();
void Go_Stop2 ();
uint8_t speed;
uint32_t delay;
uint8_t f_speed;
uint8_t b_speed;


void zmain(void)
{
reflectance_start(); // turn on reflectors on Zumo 
reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
TickType_t start; // variables for measuring time
TickType_t end;

for(;;)
    {
        // read digital values that are based on threshold. 0 = white, 1 = black
        reflectance_digital(&dig); 
        printf("DIG l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);         
        vTaskDelay(0);
    if (SW1_Read()== 0)// button press
    {
        Go_Stop(); //start  and go to the first black line
        print_mqtt("Zumo045/ready", "line"); // MQTT send ready to start
        IR_Start(); // start IR receiving
        IR_flush(); // clear IR receive buffer
        IR_wait(); // wait for the IR signal to start
        start = xTaskGetTickCount(); //start time
        print_mqtt("Zumo045/start", "%d", start); // Send MQTT time
        Go_to_White(); // go away from the first black line  to start  the line following
   		Follow_Line_Stop(); // go and follow the line and  stop  on the second black line
        Go_to_White();
        Follow_Line_Stop();
        end = xTaskGetTickCount(); //stop time
        print_mqtt("Zumo045/stop", "%d", end); // MQTT end time
        print_mqtt("Zumo045/time", "%d", end-start); // MQTT endd - start time
    }
    else
        motor_forward(0,0);

    }
}



void Go_Stop (void)
{
   
    motor_start();
    motor_forward(0,0);
    
while (1)
{
    reflectance_digital(&dig);
while ((dig.l3 == 0) && (dig.r3 == 0))
   {
    motor_forward(50,0);
    reflectance_digital(&dig);
    }
if ((dig.l3 == 1) && (dig.r3 == 1))
    {
    motor_forward(0,0);
    reflectance_digital(&dig);
    break;
    }
    
}
}
void Go_to_White(void)
{
while(1)
{
reflectance_digital(&dig);
    if ((dig.l3 == 0) && (dig.r3 == 0))
    {
    motor_forward(0,0);
    break;
    }
    else
    motor_forward(100,0);
    vTaskDelay(10);
}


}

void Follow_Line_Stop(void)
{
    
    while (1)
    {
        reflectance_digital(&dig);
        if ((dig.l3 == 1) && (dig.r3 == 1)) 
        {
            
           motor_forward(0,0);
            break;
            
            
        }
        else if ((dig.l1 == 1) && (dig.r1 == 1))
        {
            motor_forward(200,0);
        }
        else if ((dig.l1 == 0) && (dig.r1 == 1))
        {
        tankturn_right(150,150,0);
        }
        else if ((dig.l1 == 1) && (dig.r1 == 0))
        {
        tankturn_left(150,150,0);
        }
        
        
    vTaskDelay(0);
    }
}

void tankturn_right(f_speed, b_speed, delay)
{
    MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(1);     // set RightMotor backward mode
    PWM_WriteCompare1(f_speed); 
    PWM_WriteCompare2(b_speed); 
    vTaskDelay(delay);
}

void tankturn_left(f_speed, b_speed, delay)
{
    MotorDirLeft_Write(1);      // set LeftMotor backward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(f_speed); 
    PWM_WriteCompare2(b_speed); 
    vTaskDelay(delay);
}


#endif 

#if 1
    
//this is the maze one//
void Follow_line(void);
struct sensors_ dig;
void tankturn_right();
void tankturn_left();
uint8_t speed;
uint32_t delay;
uint8_t f_speed;
uint8_t b_speed;
int d = 0; // Print the detected distance (centimeters)
void Maze_Runner();
void Go_to_White();
void Go_Stop();




void zmain(void)
{
    
    TickType_t start_time, stop_time;
    
    Ultra_Start();
    reflectance_start();

    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); 
    
    
    motor_start();              // enable motor controller
    motor_forward(0,0);     // moving forward
  

    for(;;)
    {
        // read digital values that are based on threshold. 0 = white, 1 = black
        reflectance_digital(&dig);
       
        if (SW1_Read()== 0)// button press
        {
           Go_Stop();
           print_mqtt("Zumo045/ready","maze");
           IR_Start(); // start IR receiving 
           IR_flush(); // clear IR receive buffer
           IR_wait();
           start_time = xTaskGetTickCount( );
   		   print_mqtt("Zumo045/start","%d",start_time);
           Maze_Runner();
           stop_time = xTaskGetTickCount( );
           print_mqtt("Zumo045/stop","%d", stop_time);
           print_mqtt("Zumo045/time","%d", stop_time-start_time);

        }
        else
        {
           motor_forward(0,0);
           vTaskDelay(10);
        }
     }

}

void Go_Stop (void)
{
   
    motor_start();
    motor_forward(0,0);
    
while (1)
{
    reflectance_digital(&dig);
while ((dig.l3 == 0) && (dig.r3 == 0))
   {
    motor_forward(50,0);
    reflectance_digital(&dig);
    }
if ((dig.l3 == 1) && (dig.r3 == 1))
    {
    motor_forward(0,0);
    reflectance_digital(&dig);
    break;
    }
    
}
}

void Maze_Runner(void)
{
	int d;
    int x=0;
    int y=-1;//waits at -1 and when IR signal received moves to (0,0) and starts the maze
	
    while(1)
    {
    reflectance_digital(&dig);
    d = Ultra_GetDistance();
    
    
    if ((dig.l3 == 1) && (dig.r3 == 1) && (d <= 15))
	{
	    motor_forward(0,0);
        if (x<2)
        {
        tankturn_right(125,125,250);
        Follow_line();
        x ++;
        print_mqtt("Zumo045/position","%d %d", x, y);
        Follow_line();
        x ++;
        print_mqtt("Zumo045/position","%d %d", x, y);
        tankturn_left(125,125,250);
        Follow_line();
        motor_forward(0,0);
          y ++;
        print_mqtt("Zumo045/position","%d %d", x, y);
        }
        else if (x >= 2)
        {
        tankturn_left(125,125,250);
        
        Follow_line();
        x --;
        print_mqtt("Zumo045/position","%d %d", x, y);
        Follow_line();
        x --;
        print_mqtt("Zumo045/position","%d %d", x, y);
        tankturn_right(125,125,250);
        Follow_line();
        motor_forward(0,0);
        y ++;
        print_mqtt("Zumo045/position","%d %d", x, y);
        
        }
        
    }
    else if (((dig.l3 == 1) && (dig.r3 == 1)) && (d > 15))
    {
	   
        Follow_line();
        y ++;
        print_mqtt("Zumo045/position","%d %d", x, y);
        
            if (((y == 11)&& (x==0)))
            {
            Follow_line(); 
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            
            motor_forward(0,0);
            
            }
            else if ((y>12)&&(x==0))
            {   
            motor_forward(75,500);
            motor_forward(0,0);
            break;
            }   
            else if ((y == 11) && (x==1))
            {
            tankturn_left(125,125,250);
            Follow_line();
            x --;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_right(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
            else if ((y == 11) && (x==2))
            {
            tankturn_left(125,125,250);
            Follow_line();
            x--;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x --;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_right(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
            else if ((y == 11) && (x==3))
            {
            tankturn_left(125,125,250);
            Follow_line();
            x --;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x --;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x --;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_right(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
            else if ((y == 11) && (x==-1))
            {
            tankturn_right(125,125,250);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_left(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
            else if ((y == 11) && (x==-2))
            {
            tankturn_right(125,125,250);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_left(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
             else if ((y == 11) && (x==-3))
            {
            tankturn_right(125,125,250);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            Follow_line();
            x ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            tankturn_left(125,125,250);
            Follow_line();
            y ++;
            print_mqtt("Zumo045/position","%d %d", x, y);
            }
        
    }
       
   
    
}
}


 
 void Go_to_White(void)// moves off from the black line and waits on the white
{
while(1)
{
reflectance_digital(&dig);
    if ((dig.l3 == 0) && (dig.r3 == 0))
    {
    motor_forward(0,0);
    break;
    }
    else
    motor_forward(100,0);
    vTaskDelay(10);
}
}

void Follow_line(void)
{
	Go_to_White();  
    while (1)
	{
		reflectance_digital(&dig);
        
        if ((dig.l3 == 1) && (dig.r3 == 1))
        { 
            
            motor_forward(0,0);
            break;
        
        }
		
        else if ((dig.l1 == 1) && (dig.r1 == 1))
    	{
    		motor_forward(150,0);
    	}
		
        else if ((dig.l1 == 0) && (dig.r1 == 1))
		{
		    tankturn_right(100,100,0);
		}
		
        else if ((dig.l1 == 1) && (dig.r1 == 0))
		{
		    tankturn_left(100,100,0);
		}
        else if ((dig.l1 == 0) && (dig.r1 == 0))
        {
            motor_backward(100,0);
            
        }
        
        
        vTaskDelay(0);
	}
}
    

    

void tankturn_right(f_speed, b_speed, delay)
{
    MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(1);     // set RightMotor backward mode
    PWM_WriteCompare1(f_speed);
    PWM_WriteCompare2(b_speed);
    vTaskDelay(delay);
}
void tankturn_left(f_speed, b_speed, delay)
{
    MotorDirLeft_Write(1);      // set LeftMotor backward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(f_speed);
    PWM_WriteCompare2(b_speed);
    vTaskDelay(delay);
}



#endif

/* [] END OF FILE */
