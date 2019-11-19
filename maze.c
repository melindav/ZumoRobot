//this is the maze one//
void Go_forward (void);
void Follow_line(void);
void ultrasonic(void);
void Go_to_White(void);
struct sensors_ dig;
void tankturn_right();
void tankturn_left();
uint8_t speed;
uint32_t delay;
uint8_t f_speed;
uint8_t b_speed;
int d = 0; // Print the detected distance (centimeters)




void_zmain(void)
{

    Ultra_Start();
    reflectance_start();

    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); 
    print_mqtt("Zumo006/ready","maze");
    
    motor_start();              // enable motor controller
    motor_forward(0,0);     // moving forward
    
    vTaskDelay(500);



    for(;;)
    {
        // read digital values that are based on threshold. 0 = white, 1 = black
        reflectance_digital(&dig);
        printf("DIG l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);         
        vTaskDelay(0);
    
    
        if (SW1_Read()== 0)// button press
        {
           Start_moving();
           IR_Start(); // start IR receiving
           IR_flush(); // clear IR receive buffer
           IR_wait();
           ultrasonic();
        }
        else
        {
           motor_forward(0,0);
           vTaskDelay(10);
        }
     }



void Start_moving(void) //get to the starting line
{
   
        motor_start();
        motor_forward(0,0);
        print_mqtt("Zumo006/ready","%d");
    
    while (1)
    {
        reflectance_digital(&dig);

    while ((dig.l3 == 0) && (dig.r3 == 0))
    {
        motor_forward(100,0);
        reflectance_digital(&dig);
        print_mqtt("Zumo006/line");
    }
    
   
    if ((dig.l3 == 1) && (dig.r3 == 1))
    {
        motor_forward(0,0)
        reflectance_digital(&dig);
        print_mqtt("Zumo006/stop","%d");
        print_mqtt("Zumo006/time","%d");
        break;
    }

   }





void ultrasonic(void)

do
{
    Follow_line();


    while 

    {
        
    
        d = Ultra_GetDistance(); // Print the detected distance (centimeters)
    
    
        if(d <= 13) // if the distance is less than 13 cm
        
   
        {
            motor_forward(0,0);         // stop motor
            print_mqtt("Zumo006/stop","%d");

            vTaskDelay(500);
            int n = rand() %2;
            if (n == 0)
            {
  	           tankturn_left(100,25,250);
            }
            else 
  	        {
  	           tankturn_right(25,100,250);
            }
            vTaskDelay(100);
            
       }
        
    }
    
}
    




}

void Follow_line(void)
{
	while (1)
	{
		reflectance_digital(&dig);
        
		
        if ((dig.l1 == 1) && (dig.r1 == 1))
    	{
    		motor_forward(75,0);
            print_mqtt("Zumo006/Start","%d")
    	    break;
    	}
		
        else if ((dig.l1 == 0) && (dig.r1 == 1))
		{
		    tankturn_right(75,75,0);
		}
		
        else if ((dig.l1 == 1) && (dig.r1 == 0))
		{
		    tankturn_left(75,75,0);
		}
        
        else if ((dig.l1 == 0) && (dig.r1 == 0))
        { 
            
            motor_forward(0,0);
            print_mqtt("Zumo006/stop","%d");
            print_mqtt("Zumo006/time","%d");
            break;
        
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









