// This #include statement was automatically added by the Particle IDE.
#include <SparkCorePolledTimer.h>
#include <spark-dallas-temperature.h>
#define ONE_WIRE_BUS 5
#define PRECISION 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress poolTemp, heaterTemp;

bool currentState = LOW;
const long SEC = 1000;
const long MINUTE = 60000;

unsigned long lastSwitch = 0;
unsigned long lastTemp = 0;
unsigned long onDelay = 2*SEC*60;
unsigned long offDelay = 10*SEC*60;
unsigned long tempDelay = MINUTE*2;
unsigned long switchDelay = offDelay;
char publishString[40];

int totalCycles = 0;
bool heaterEnabled = false;
int totalSensors = 0;
double currentTemp = 0.0;

void setup() {
    Particle.publish("PoolHeaterStartup");
    Particle.subscribe("enablePoolTimer",enablePoolTimer);
    pinMode(6,OUTPUT);
    digitalWrite(6,currentState);
    Particle.publish("SensorStatus","STARTING");
    sensors.begin();
    Particle.publish("SensorStatus","STARTED");
    totalSensors = sensors.getDeviceCount();
    sprintf(publishString,"%u",totalSensors);
    Particle.publish("SensorCount",publishString);

    if(!sensors.getAddress(poolTemp,0)) Particle.publish("SensorStatus","NO_ADDRESS");
    sensors.setResolution(poolTemp,PRECISION);
    sensors.requestTemperatures();
    currentTemp = sensors.getTempFByIndex(0);
    Particle.variable("currentState",currentState);
    Particle.variable("heaterEnabled",heaterEnabled);
    Particle.variable("poolTemp",currentTemp);

}

void loop() {
    if(heaterEnabled) {
        switchPump();
    }
    if(totalSensors > 0) {
      checkTemp();
    }
}
void checkTemp() {
  unsigned long now = millis();
  if((now - lastTemp) >= tempDelay) {
    lastTemp = now;
    sensors.requestTemperatures();
    currentTemp = sensors.getTempFByIndex(0);
    Particle.publish("PoolTemp",String(currentTemp));
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
