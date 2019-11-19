#if 1
void zmain(void)
{

struct sensors_ ref;
struct sensors_ dig;

reflectance_start();
reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
//SW1_Read(); //button 

for(;;)
    {
        // read raw sensor values
        reflectance_read(&ref);
        // print out each period of reflectance sensors
        printf("REF: l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig); 
        //print out 0 or 1 according to results of reflectance period
        printf("DIG l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);       
        
        vTaskDelay(200);
    }
//reflectance_digital(&dig);// reading the digital response from reflecctors in dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3
//printf("l3:%d. l2:%d. l1:%d. r1:%d. r2:%d. r3:%d.\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);

}

#endif 