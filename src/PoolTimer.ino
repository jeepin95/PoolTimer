
// This #include statement was automatically added by the Particle IDE.
#include <SparkIntervalTimer.h>


IntervalTimer runTimer;
bool currentState = LOW;
bool tempEnable = false;
int nextTimer = 60;

void setup() {
    Particle.publish("PoolTimerStartup");

    Particle.subscribe("enablePoolTimer",enablePoolTimer);
    pinMode(6,OUTPUT);
   // digitalWrite(6,HIGH);
   runTimer.begin(runPool, 10000, hmSec);
}

void loop() {

}

void runPool() {
    if(currentState == LOW) {
        currentState = HIGH;
        nextTimer = 60000;
        Particle.publish("TurnOn2Minutes");
    } else {
        currentState = LOW;
        nextTimer = 60000;
        Particle.publish("TurnOff10Minutes");
    }
    digitalWrite(6,currentState);
    runTimer.resetPeriod_SIT(nextTimer,hmSec);
}

void enablePoolTimer(const char *event, const char *data) {
    if(currentState == LOW) {
        currentState = HIGH;
    } else {
        currentState = LOW;
    }
}
