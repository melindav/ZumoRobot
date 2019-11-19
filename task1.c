#if 1
    // zumo wrestling
void Go_Stop (void);
struct sensors_ dig;
void tankturn_right();
void tankturn_left();
void random_reverse();
uint8_t speed;
uint32_t delay;
uint8_t f_speed;
uint8_t b_speed;


void zmain(void)
{
	TickType_t start_time, stop_time, execution_time, hit_time;
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000

    // void IR_wait(void); //wait for any IR
    for(;;)
    {
        // read digital values that are based on threshold. 0 = white, 1 = black
        reflectance_digital(&dig); 
        printf("DIG l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);         
        vTaskDelay(0);
       if (SW1_Read()== 0)// button press
        {
            Go_Stop();
            print_mqtt("Zumo006/zumo","ready");
            IR_Start(); // start IR receiving
            IR_flush(); // clear IR receive buffer
   		    IR_wait();
   		    start_time = xTaskGetTickCount( );
   		    print_mqtt("Zumo006/start","%d",start_time);

			Stay_in_Circle ();
			LSM303D_Read_Acc(&data);
		
            if(data.accX < -2000 || data.accX > -2000)
            {
                hit_time = xTaskGetTickCount( );
                print_mqtt("Zumo006/hit"," %d", hit_time);
                break;
            }
		

   		    print_mqtt("Zumo006/hit","%d",hit_time);

   		    finish_time = xTaskGetTickCount( );
   		    print_mqtt("Zumo006/stop","%d",stop_time);

   		    execution_time = stop_time - start_time;
   		    print_mqtt("Zumo006/time","%d",execution_time);

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
	motor_start();
	motor_forward(0,0);

	while(1)
	{
	   reflectance_digital(&dig);
       while ((dig.l3 == 0) && (dig.r3 == 0))
       {
           motor_forward(150,0);
           reflectance_digital(&dig);
        }
       if ((dig.l1 == 1) && (dig.r1 == 1))
       {
            motor_forward(0,0);
            reflectance_digital(&dig);
            random_reverse();

        }
    	
	}
	
}
void random_reverse()
{    
  
  motor_backward(100,1000); // moving backward
  int n = rand() %2;
  if (n == 0)
  {
  	tankturn_left(100,25,250);//turn first 
  }
  else 
  {
  	tankturn_right(25,100,250);
  }
} 	
