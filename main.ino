#include <Time.h> 

//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
  return celsius + 273.15;
}

double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

        // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

        // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}


#include <dht11.h>

dht11 DHT11;

#define DHT11PIN 3
#define OPTOPIN 2
#define LEDPIN 13

#define DEBUG false
#define SHOWLED false

void setup()
{

  pinMode(OPTOPIN,OUTPUT);
  digitalWrite(OPTOPIN,LOW);
  pinMode(LEDPIN, OUTPUT);

  if (DEBUG) {
    Serial.begin(115200);
    Serial.println("DHT11 TEST PROGRAM ");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT11LIB_VERSION);
    Serial.println();
  }
}

void fanOff() {
  if (DEBUG) { Serial.print("turning fan off"); }
  pinMode(OPTOPIN,OUTPUT);
  digitalWrite(OPTOPIN,LOW);
  if (SHOWLED) {
    digitalWrite(LEDPIN, LOW);
  }
}

void fanOn() {
  if (DEBUG) { Serial.print("turning fan on"); }
  pinMode(OPTOPIN,OUTPUT);
  digitalWrite(OPTOPIN,HIGH);
  if (SHOWLED) {
    digitalWrite(LEDPIN, HIGH);
  }
}


void loop()
{
  if (DEBUG) { Serial.println("\n"); }

  int chk = DHT11.read(DHT11PIN);

  if (DEBUG) { Serial.print("Read sensor: "); }
  
  switch (chk)
  {
    case DHTLIB_OK: 
      
      if (DEBUG) {
        Serial.println("OK"); 
        Serial.print("Humidity (%): ");
        Serial.println((float)DHT11.humidity, 2);
        Serial.print("Temperature (°C): ");
        Serial.println((float)DHT11.temperature, 2);
        Serial.print("Dew Point (°C): ");
        Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
      }
      
      if (DHT11.humidity > 55) {
      // if (second() > 29) {
        fanOn(); 
      } else {
        fanOff();
      }
    

      break;
    case DHTLIB_ERROR_CHECKSUM: 
      if (DEBUG) { Serial.println("Checksum error"); }
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      if (DEBUG) { Serial.println("Time out error"); }
      break;
    default: 
      if (DEBUG) { Serial.println("Unknown error"); }
      break;
  }


  delay(5000);
}
