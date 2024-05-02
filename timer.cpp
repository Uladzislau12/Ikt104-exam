/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */


#define wait_ms 100 
#include "mbed.h"

InterruptIn bt_1(D1, PullUp); //alarm 
InterruptIn bt_2(D2, PullUp); // snooze
InterruptIn bt_3(D3, PullUp); // mute
//enable/disable 
PwmOut buzz(D0);

DigitalInOut alarm_1(D4);
DigitalOut snooze_alarm(D5);


typedef struct
 {
bool alarm;
bool snoozed;
bool mute;
bool active;
Mutex* mut; // Using pointer to Mutex
} Clock; 

Clock alarm;
Timeout alarmTimeout, snoozeTimeout, muteTimeout;
const int melody[] = {262, 294, 330, 349, 440, 494};
const int note_duration[] = {4, 4, 4, 4, 4, 4, 4}; // in milliseconds

using namespace std::chrono;


void Play_melody() //Plays a melody through the buzzer
{
     alarm.mut->lock();
    for(int i = 0; i < sizeof(melody) / sizeof(int); i++)
    // play the note
    {
        buzz = 1;
        int duration = 1000 /note_duration[i];
         thread_sleep_for(wait_ms);

         // stop playing the note
         buzz = 0;
         thread_sleep_for(wait_ms);
    }
     alarm.mut->unlock();
}

void muteAlarm() // Mutes the alarm
{
     alarm.mut->lock();
     alarm.active = false;
     alarm.mute = true;
     printf("Alarm muted.\n");
     alarm.mut->unlock();

}
void alarm_on() //Activates the alarm, plays the melody, and sets a timeout to mute the alarm after 2 minutes
 {
    alarm.mut->lock();
    // Sound the alarm using the melody
    Play_melody();
    alarm.active = true;

    alarmTimeout.attach(&muteAlarm, 120000ms); // Mute for 2 min
    alarm.mut->unlock();
}




void unmuteAlarm()
{
    alarm.mut->lock();
    alarm.mute = false;
    printf("alarm unmuted.\n");
    alarm.mut->unlock();
    
}

void resetsnooze()
{
    alarm.mut->lock();
    printf("snooze reset.\n");
    alarm.snoozed = false; // reset snooze state
    alarm.mut->unlock();

}
void snoozeAlarm()
 {
    alarm.mut->lock();// Sets the alarm to snooze mode, silences it, and sets a timeout to reset snooze after 100 milliseconds.
    if (alarm.active || alarm.snoozed)
     {
        alarm.snoozed = true;
        alarm_1 = 0;
        snooze_alarm = 1;
        alarmTimeout.attach(&muteAlarm, 120000ms); // Mute for 2 min
        snoozeTimeout.attach(&resetsnooze, 1000ms);
        printf("Alarm snoozed.\n");
    }
    alarm.mut->unlock();
}
void mute_Or_enable()
{
    if(alarm.active || alarm.snoozed)
    {
    alarm.mut->lock();
    alarm_1 = 0;
    snooze_alarm = 0;
    alarm.active = false;
    alarm.snoozed = false;
    alarm.mute = true;
    printf("Alarm muted for two minutes. \n");
    }
    else
    {
        alarm.mute = !alarm.mute;
        printf("Alarm %s\n", alarm.mute ? "muted  :", "Enabled" : "disable");

    }
    alarm.mut->unlock();
}
void enable_disable()
{ alarm.mut->lock();
//alarm.Enabled = !alarm.Enabled;

}

int main()
{
    

    bt_1.fall(&alarm_on);
    bt_2.fall(&snoozeAlarm);
    bt_3.fall(mute_Or_enable);

    while(true)
    {
        if(!alarm.active && !alarm.mute)
        {
            
            alarm_on();
            int currenthour = 7;
            int  currentmin = 30;
            if(currenthour == 7 && currentmin == 30)
            {
                alarm_on();
            }
        }
        ThisThread::sleep_for(1000);
    }


}
