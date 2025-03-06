#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

bool trigger_alarm(struct tm * timeinfo, int start_time_hour, int start_time_minute, int stop_time_hour, int stop_time_minute);

#endif