#ifndef NTP_H
#define NTP_H

#include <Arduino.h>

String get_time();
struct tm get_time_raw();
bool is_time_available();
void timeavailable(struct timeval *t);
void setup_ntp();

#endif