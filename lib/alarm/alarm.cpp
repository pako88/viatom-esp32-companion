#include <secrets.h>

#include <Arduino.h>

bool trigger_alarm(struct tm * timeinfo, int start_time_hour, int start_time_minute, int stop_time_hour, int stop_time_minute) {
    // time can't be configured across multiple hours currently. Alarm has to stop at the full hour. (e.g. 5:50 - 6:10 doesn't work)
    
    bool check_hour;
    if (start_time_hour == stop_time_hour) {
        check_hour = timeinfo->tm_hour >= start_time_hour;
    } else {
        check_hour = timeinfo->tm_hour >= start_time_hour && timeinfo->tm_hour < stop_time_hour;
    }
    
    bool check_minute;
    if (stop_time_minute == 0) {
        check_minute = timeinfo->tm_min >= start_time_minute;
    } else {
        check_minute = timeinfo->tm_min >= start_time_minute && timeinfo->tm_min < stop_time_minute;
    }

    /* weekdays
        0 = Sunday
        1 = Monday
        2 = Tuesday
        3 = Wednesday
        4 = Thursday
        5 = Friday
        6 = Saturday
    */
    int weekdayArray[] = {1, 2, 3, 4, 5};
    bool isInArray = false;
    for (int i = 0; i < sizeof(weekdayArray) / sizeof(weekdayArray[0]); i++) {
        if (weekdayArray[i] == timeinfo->tm_wday) {
            isInArray = true;
            break;
        }
    }
    if (isInArray) {
        if (check_hour) {
            if (check_minute) {
                return true;
            }
        }
    }
    return false;
}