/*
 * Title: Operating Systems Assignment
 * Description: This file contains the main program
 * Author: Kalana Tharusha
 * Date: 01/05/2023
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"
#include "customStructs.h"

void *customer();

void *teller(void *no);

void writeArrivalLog(cstmr newCustomer);

void writeResponseLog(int tellerNo, cstmr servingCustomer);

void writeCompletionLog(int tellerNo, cstmr servingCustomer);

void writeTellerStatistics();

void writeTermination(int tellerNo);

void simulateService(char serviceType);

int getTotalServedCustomers(tlr tlrArr[]);

pthread_mutex_t c_queue_mutex; //Mutex for customer queue
pthread_mutex_t r_log_mutex; //Mutex for log file

pthread_cond_t c_queue_cond; //Condition variable for customer queue

FILE *logFile, *customerFile; //File pointers for log file and customer file

struct Queue c_queue; //Customer queue

int activeTellerCount = 0; //number of active tellers

int end = 0; //is the file read completed

//Array of tellers to store teller information
tlr tellers[4] = {{1, 0, {0}, {0}},
                  {2, 0, {0}, {0}},
                  {3, 0, {0}, {0}},
                  {4, 0, {0}, {0}}};

//m: length of customer queue,
// tc: periodic time for the customer to arrive in the queue,
// tw: time duration to serve withdrawal,
// td: time duration to serve deposit,
// ti: time duration to serve information
int m, tc, tw, td, ti;


int main(int argc, char *argv[]) {

    //Check if the number of arguments is correct
    if (argc != 6) {
        printf("Invalid number of arguments!\n");
        printf("Run command -> ./cq m tc tw td ti\n");
        printf("m: length of customer queue\n");
        printf("tc: periodic time for the customer to arrive in the queue\n");
        printf("tw: time duration to serve withdrawal\n");
        printf("td: time duration to serve deposit\n");
        printf("ti: time duration to serve information\n");

        return 1;
    }

    printf("Program started...\n"); //Print program started message

    // get the arguments
    m = strtol(argv[1], NULL, 10);
    tc = strtol(argv[2], NULL, 10);
    tw = strtol(argv[3], NULL, 10);
    td = strtol(argv[4], NULL, 10);
    ti = strtol(argv[5], NULL, 10);

    pthread_t cus_thread; //Customer thread
    pthread_t teller_thread[4]; //Teller threads

    pthread_mutex_init(&c_queue_mutex, NULL); //Initialize customer queue mutex
    pthread_mutex_init(&r_log_mutex, NULL); //Initialize log file mutex
    pthread_cond_init(&c_queue_cond, NULL); //Initialize customer queue condition variable

    logFile = fopen("r_log", "w"); //Open log file

    //Check if the log file is created
    if (logFile == NULL) {
        printf("Failed to create log file\n");
    }

    initQueue(&c_queue, m); //Initialize customer queue

    //Create customer thread
    pthread_create(&cus_thread, NULL, customer, NULL);

    //Create teller threads
    for (int i = 0; i < 4; ++i) {
        pthread_create(&teller_thread[i], NULL, teller, (void *) &tellers[i].no);
    }

    //join customer thread
    pthread_join(cus_thread, NULL);

    //join teller threads
    for (int i = 0; i < 4; ++i) {
        pthread_join(teller_thread[i], NULL);
    }

    //Destroy mutex and condition variable
    pthread_mutex_destroy(&c_queue_mutex);
    pthread_cond_destroy(&c_queue_cond);
    pthread_mutex_destroy(&r_log_mutex);

    //Close log file
    fclose(logFile);

    printf("Program ended.\n"); //Print program ended message

    return 0;
}

//Customer thread function
void *customer() {

    int num; //Customer number
    char service; //Service type
    char buffer[100]; //Buffer to store the line read from the file

    customerFile = fopen("c_file", "r"); //Open customer file

    //Check if the customer file is opened
    if (customerFile == NULL) {
        printf("Failed to open customer file\n");
    }

    //Read the file line by line
    while (fgets(buffer, sizeof(buffer), customerFile) != NULL) {

        sleep(tc); //Sleep for tc seconds to add customers periodically

        char *endPtr; //Pointer to the end of the string
        num = strtol(buffer, &endPtr, 10); //get the customer number as an integer

        //Check if the file format is correct
        if (buffer != endPtr && *endPtr == ' ' && *(endPtr + 1) != '\n' && *(endPtr + 1) != '\0') {

            service = *(endPtr + 1); //Get the service type

            time_t aTime = time(NULL); //Get the arrival time
            struct tm tm = *localtime(&aTime); //Convert to local time

            cstmr newCustomer; //Create a new customer
            newCustomer.no = num; //Set the customer number
            newCustomer.service = service; //Set the service type
            newCustomer.aTime = tm; //Set the arrival time

            //Wait until the queue is not full
            while (isQueueFull(&c_queue) && !end) {
                pthread_cond_wait(&c_queue_cond, &c_queue_mutex);
            }

            pthread_mutex_lock(&c_queue_mutex); //Lock the customer queue mutex

            enqueue(&c_queue, newCustomer); //Enqueue the customer

            pthread_mutex_unlock(&c_queue_mutex); //Unlock the customer queue mutex
            pthread_cond_signal(&c_queue_cond); //Signal the condition variable

            writeArrivalLog(newCustomer); //Write the arrival log

        }
    }

    fclose(customerFile); //Close the customer file

    end = 1; //Mark the end of the file read

    return NULL;
}

//Teller thread function
void *teller(void *no) {

    activeTellerCount++; //Increment the active teller count

    time_t sTime = time(NULL); //Get the start time
    struct tm s_tm = *localtime(&sTime); //Convert to local time

    tellers[(*(int *) no) - 1].sTime = s_tm; //Set the start time

    //Run until the customer queue is empty and the file read is completed
    while (1) {
        pthread_mutex_lock(&c_queue_mutex); //Lock the customer queue mutex

        //If the queue is empty and the file read is not completed, wait for the condition variable
        if (isQueueEmpty(&c_queue) && !end) {
            pthread_cond_wait(&c_queue_cond, &c_queue_mutex);
        }

        //If the queue is empty and the file read is completed, terminate the thread
        if (isQueueEmpty(&c_queue)) {

            time_t tTime = time(NULL); //Get the termination time
            struct tm t_tm = *localtime(&tTime); //Convert to local time

            tellers[(*(int *) no) - 1].tTime = t_tm; //Set the termination time

            writeTermination(*(int *) no); //Log the termination

            pthread_mutex_unlock(&c_queue_mutex); //Unlock the customer queue mutex
            pthread_cond_signal(&c_queue_cond); //Signal the condition variable

            //Check if is the last active teller
            if (activeTellerCount == 1) {
                writeTellerStatistics(); //Log the teller statistics
            }

            activeTellerCount--; //Decrement the active teller count

            break; //Break the loop and terminate the thread
        }

        cstmr servingCustomer = dequeue(&c_queue); //Dequeue the customer

        time_t rTime = time(NULL); //Get the response time
        struct tm r_tm = *localtime(&rTime); //Convert to local time

        servingCustomer.rTime = r_tm; //Set the response time

        writeResponseLog(*(int *) no, servingCustomer); //Write to the response log

        tellers[(*(int *) no) - 1].servedCount++; //Increment the served customer count

        pthread_mutex_unlock(&c_queue_mutex); //Unlock the customer queue mutex

        simulateService(servingCustomer.service); //Simulate the service

        time_t cTime = time(NULL); //Get the completion time
        struct tm c_tm = *localtime(&cTime); //Convert to local time

        servingCustomer.cTime = c_tm; //Set the completion time

        writeCompletionLog(*(int *) no, servingCustomer); //Write to the completion log

    }

    return NULL;
}

//Log the arrival message
void writeArrivalLog(cstmr newCustomer) {

    pthread_mutex_lock(&r_log_mutex); //Lock the log file mutex

    fprintf(logFile,
            "-----------------------------------------------------------------------\n"
            "%d: %c\n"
            "Arrival time: %02d:%02d:%02d \n"
            "-----------------------------------------------------------------------\n",
            newCustomer.no, newCustomer.service,
            newCustomer.aTime.tm_hour, newCustomer.aTime.tm_min, newCustomer.aTime.tm_sec);

    pthread_mutex_unlock(&r_log_mutex); //Unlock the log file mutex
}

//Log the Response message
void writeResponseLog(int tellerNo, cstmr servingCustomer) {

    pthread_mutex_lock(&r_log_mutex); //Lock the log file mutex

    //Write to the log file
    fprintf(logFile,
            "-----------------------------------------------------------------------\n"
            "Teller: %d\n"
            "Customer: %d\n"
            "Arrival time: %02d:%02d:%02d\n"
            "Response time: %02d:%02d:%02d\n"
            "-----------------------------------------------------------------------\n",
            tellerNo,
            servingCustomer.no,
            servingCustomer.aTime.tm_hour, servingCustomer.aTime.tm_min, servingCustomer.aTime.tm_sec,
            servingCustomer.rTime.tm_hour, servingCustomer.rTime.tm_min, servingCustomer.rTime.tm_sec);

    pthread_mutex_unlock(&r_log_mutex); //Unlock the log file mutex
}

//Log the completion of the service
void writeCompletionLog(int tellerNo, cstmr servingCustomer) {

    pthread_mutex_lock(&r_log_mutex); //Lock the log file mutex

    //Write to the log file
    fprintf(logFile,
            "-----------------------------------------------------------------------\n"
            "Teller: %d\n"
            "Customer: %d\n"
            "Arrival time: %02d:%02d:%02d\n"
            "Completion time: %02d:%02d:%02d\n"
            "-----------------------------------------------------------------------\n", tellerNo,
            servingCustomer.no,
            servingCustomer.aTime.tm_hour, servingCustomer.aTime.tm_min, servingCustomer.aTime.tm_sec,
            servingCustomer.cTime.tm_hour, servingCustomer.cTime.tm_min, servingCustomer.cTime.tm_sec);

    pthread_mutex_unlock(&r_log_mutex); //Unlock the log file mutex
}

//Log the termination message
void writeTermination(int tellerNo) {

    pthread_mutex_lock(&r_log_mutex); //Lock the log file mutex

    //Write to the log file
    fprintf(logFile,
            "\nTermination: teller-%d\n"
            "#served customers: %d\n"
            "Start time: %02d:%02d:%02d \n"
            "Termination time: %02d:%02d:%02d\n\n",
            tellerNo,
            tellers[tellerNo - 1].servedCount,
            tellers[tellerNo - 1].sTime.tm_hour, tellers[tellerNo - 1].sTime.tm_min, tellers[tellerNo - 1].sTime.tm_sec,
            tellers[tellerNo - 1].tTime.tm_hour, tellers[tellerNo - 1].tTime.tm_min,
            tellers[tellerNo - 1].tTime.tm_sec);

    pthread_mutex_unlock(&r_log_mutex); //Unlock the log file mutex
}

//Log the teller statistics
void writeTellerStatistics() {

    pthread_mutex_lock(&r_log_mutex); //Lock the log file mutex

    //Write to the log file
    fprintf(logFile,
            "\nTeller Statistic\n"
            "Teller-1 serves %d customers. \n"
            "Teller-2 serves %d customers.\n"
            "Teller-3 serves %d customers.\n"
            "Teller-4 serves %d customers.\n\n"
            "Total number of customers: %d customers.\n\n",
            tellers[0].servedCount, tellers[1].servedCount, tellers[2].servedCount, tellers[3].servedCount,
            getTotalServedCustomers(tellers));

    pthread_mutex_unlock(&r_log_mutex); //Unlock the log file mutex
}

//Simulate the service based on the service type
void simulateService(char serviceType) {
    switch (serviceType) {
        case 'W':
            sleep(tw);
            break;
        case 'D':
            sleep(td);
            break;
        case 'I':
            sleep(ti);
            break;
        default:
            printf("Invalid service type: %c \n", serviceType);
    }
}

//Get the total number of served customers
int getTotalServedCustomers(tlr tlrArr[]) {
    int sum = 0;
    for (int i = 0; i < 4; ++i) {
        sum = sum + tlrArr[i].servedCount;
    }
    return sum;
}
