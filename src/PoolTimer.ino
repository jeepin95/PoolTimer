// This #include statement was automatically added by the Particle IDE.
#include <SparkCorePolledTimer.h>

bool currentState = LOW;
const long SEC = 1000;

unsigned long lastSwitch = 0;
unsigned long onDelay = 2*SEC*60;
unsigned long offDelay = 10*SEC*60;
unsigned long switchDelay = offDelay;

int totalCycles = 0;
bool heaterEnabled = false;

void setup() {
    Particle.publish("PoolHeaterStartup");
    Particle.subscribe("enablePoolTimer",enablePoolTimer);
    pinMode(6,OUTPUT);
    digitalWrite(6,currentState);
    Particle.variable("currentState",currentState);
    Particle.variable("heaterEnabled",heaterEnabled);
}

void loop() {
    if(heaterEnabled) {
        switchPump();
    }
}

void switchPump() {
    unsigned long now = millis();
    if((now - lastSwitch) >= switchDelay) {
        lastSwitch = now;
        totalCycles++;
        if(totalCycles > 2000) {
            Particle.publish("MaxCyclesReached");
            currentState = LOW;
            heaterEnabled = false;
            pumpOn(LOW);
        } else {
            if(currentState == LOW) {
                currentState = HIGH;
                switchDelay = onDelay;
            } else {
                currentState = LOW;
                switchDelay = offDelay;
            }
            pumpOn(currentState);
        }
    }
}

void pumpOn(bool val) {
    digitalWrite(6,val);
    if(val) {
        Particle.publish("PumpStatus","ON");
    } else {
        Particle.publish("PumpStatus","OFF");
    }
}


void enablePoolTimer(const char *event, const char *data) {
    if(data) {
        totalCycles = 0;
        heaterEnabled = true;
        currentState = HIGH;
        pumpOn(currentState);
    } else {
        heaterEnabled = false;
        currentState = LOW;
        pumpOn(currentState);
    }
}
