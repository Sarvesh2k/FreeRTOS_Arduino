#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <String.h>

TaskHandle_t LPT_TaskHandle;
TaskHandle_t MPT_TaskHandle;
TaskHandle_t HPT_TaskHandle;

SemaphoreHandle_t binSemaphore_A;


void printMsg(TaskHandle_t taskhandle,String str)  {
        Serial.print("Priority ");  // Print task priority 
        Serial.print(uxTaskPriorityGet(taskhandle));
        Serial.print(" : ");
        Serial.println(str);        // Print user string
}

void setup()
{  
    Serial.begin(9600);
    Serial.println(F("In Setup function, Creating Binary Semaphore"));

    binSemaphore_A = xSemaphoreCreateBinary(); // Creates Binary Semaphore
    /*vSemaphoreCreateBinary(binSemaphore_A); -> performs the same functions as above, 
                                                 but has been deprecated in newer versions of FreeRTOS. 
                                                 This function is a macro function*/

    if(binSemaphore_A != NULL)
    {
        Serial.println(F("Creating low priority task"));
        xTaskCreate(LPT_Task, "LPT_Task", 100, NULL, 1, &LPT_TaskHandle);
    }
    else
    {
        Serial.println(F("Failed to create Semaphore"));
    }
}


void loop()
{ // Hooked to Idle Task, will run when CPU is Idle
    Serial.println(F("Loop function"));
    delay(50);
}


/*LPT: Low priority task*/
void LPT_Task(void* pvParameters)
{
    printMsg(LPT_TaskHandle,"LPT_Task Acquiring semaphore"); 
    xSemaphoreTake(binSemaphore_A,portMAX_DELAY);

    printMsg(LPT_TaskHandle,"LPT_Task Creating HPT"); 
    xTaskCreate(HPT_Task, "HPT_Task", 100, NULL, 3, &HPT_TaskHandle); 


    printMsg(LPT_TaskHandle,"LPT_Task Creating MPT");  
    xTaskCreate(MPT_Task, "MPT_Task", 100, NULL, 2, &MPT_TaskHandle); 

    printMsg(LPT_TaskHandle,"LPT_Task Releasing Semaphore");
    xSemaphoreGive(binSemaphore_A);

    printMsg(LPT_TaskHandle,"LPT_Task Finally Exiting");
    vTaskDelete(LPT_TaskHandle);
}


/*MPT: Medium priority task*/
void MPT_Task(void* pvParameters)
{
    printMsg(MPT_TaskHandle,"MPT_Task Done and about to exit");
    vTaskDelete(MPT_TaskHandle);
}


/*MPT: High priority task*/
void HPT_Task(void* pvParameters)
{
    printMsg(HPT_TaskHandle,"HPT_Task Trying to Acquire the semaphore");
    xSemaphoreTake(binSemaphore_A,portMAX_DELAY);

    printMsg(HPT_TaskHandle,"HPT_Task Acquired the semaphore");    

    printMsg(HPT_TaskHandle,"HPT_Task Releasing the semaphore");
    xSemaphoreGive(binSemaphore_A);    

    printMsg(HPT_TaskHandle,"HPT_Task About to Exit");
    vTaskDelete(HPT_TaskHandle);
}
