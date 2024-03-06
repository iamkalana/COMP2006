/*
 * Part of: Operating Systems Assignment
 * Description: This file contains Custom structs for the program
 * Author: Kalana Tharusha
 * Date: 01/05/2023
*/
#ifndef CUSTOMSTRUCTS_H
#define CUSTOMSTRUCTS_H

#include <time.h>

//Struct for customer
typedef struct {
    int no; //Customer number
    char service; //Service type
    struct tm aTime; //Arrival time
    struct tm rTime; //Response time
    struct tm cTime; //Completion time
} cstmr;

//Struct for teller
typedef struct {
    int no; //Teller number
    int servedCount; //Number of served customers
    struct tm sTime; //Start time
    struct tm tTime; //Terminate time
} tlr;

#endif