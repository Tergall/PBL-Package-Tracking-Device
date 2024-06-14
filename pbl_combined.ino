//Libraries for the gsm module
#include "agsmII_IP_lbr.h"
#include "agsmII_SMS_lbr.h"
#include "agsmII_DTMF_lbr.h"
#include "agsmII_FS_lbr.h"
#include "agsmII_MMS_lbr.h"
//Libraries for temperature and humidity sensors
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL345_U.h>
//Library and variable for the arduino sleep
#include <avr/sleep.h>
uint8_t loopCount = 0 ;

void RTC_init(void)
{
  while (RTC.STATUS > 0) ;    // Wait for all register to be synchronized 
 
  RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;        // Run low power oscillator (OSCULP32K) at 1024Hz for long term sleep
  RTC.PITINTCTRL = RTC_PI_bm;              // PIT Interrupt: enabled 

  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;     // Set period 32 seconds (see data sheet) and enable PIC                      
}



ISR(RTC_PIT_vect)
{
  RTC.PITINTFLAGS = RTC_PI_bm;          // Clear interrupt flag by writing '1' (required) 
}

#define DHTTYPE DHT11   // Definicja typu czujnika (DHT11)

#if (ARDUINO >= 100)
  #include "Arduino.h"
	#if !defined(HARDWARESERIAL)
		#include <SoftwareSerial.h>
	#endif
#else
  #include "WProgram.h"
	#if !defined(HARDWARESERIAL)
		#include <NewSoftSerial.h>
	#endif
#endif


#define powerPIN     7//Arduino Digital pin used to power up / power down the gsm modem
#define resetPIN     6//Arduino Digital pin used to reset the gsm modem 
#define statusPIN    5//Arduino Digital pin used to monitor if gsm modem is powered 

int state=0, i=0, powerState = 0; 
int listMode = 0;//var used for multi level menues
const int temp_and_humid_pin = 4; //Pin used to connect the DHT11 sensor
DHT dht(temp_and_humid_pin, DHTTYPE);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();


#define UNO_MODE  //Arduino UNO
#define BUFFDSIZE 200 //240
#if defined usejLader //added only for cross code compatibility with c-uGSM and d-u3G
	SoftwareSerial agsmSerial(3, 2);  //RX==>3 ,TX soft==>2
#else
	SoftwareSerial agsmSerial(2, 3);  //RX==>2 ,TX soft==>3
#endif

//Defining variables and buffers for gsm communication, DHT11 readout, and GPS readout
char ch;
char buffd[BUFFDSIZE];
char readBuffer[200];
char datatoSMS[200];
char phonenumber[15] = {"+48695609323"};
char tempch[20];
char humidch[20];
float temperatura;
float wilgotnosc;
signed long latitude, longitude;
float longitude_calc, latitude_calc;
byte gps_data[100];

void setup(){
  //Configuring serial ports
	agsmSerial.begin(9600);
  Serial1.begin(9600);
	Serial.begin(57600);

	clearagsmSerial();
	clearSerial();
	delay(10);

	modemHWSetup();							//configure Arduino IN and OUT to be used with modem
	
	Serial.flush();
	agsmSerial.flush();
	delay(1000);
	Serial.println(F("Package sensor"));
	Serial.flush();

	Serial.println(F("sit back and relax until a-gsmII is ready"));
	delay(100);
  
  //GSM modem setup
	powerOnModem();

	clearBUFFD();
	while(strlen(buffd)<1){
		getIMEI();
		delay(500);
	}
  
	ready4SMS = 0; 
	ready4Voice = 0;

	Serial.println(F("a-gsmII ready"));
	Serial.print(F("a-gsmII IMEI: ")); Serial.flush();
	Serial.println(buffd); Serial.flush();
	setAUDIOchannel(0);
	delay(500);

  delay(1000);

  //DHT11 setup
  dht.begin(); 

  if(!accel.begin())
   {
      Serial.println("No ADXL345 sensor detected.");
   }

  RTC_init();   
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Set sleep mode to POWER DOWN mode 
  sleep_enable();                       // Enable sleep mode, but not going to sleep yet 
}

void loop() {
  //Powering on modem, beginning data collection loop
  powerOnModem();
	ready4SMS = 0; 
  int g = 0;
  while(g < 50){ //Loop for obtaining GPS data
    int i = 0;
    if(Serial1.available()){
      while(Serial1.available() && i < 100){
        gps_data[i] = Serial1.read();
        i++;
      }
      if(gps_data[2] == 0x01 && gps_data[3] == 0x07 && i > 50){
        longitude = 0;
        latitude = 0;
        longitude |= gps_data[33];
        longitude = longitude << 8;
        longitude |= gps_data[32];
        longitude = longitude << 8;
        longitude |= gps_data[31];
        longitude = longitude << 8;
        longitude |= gps_data[30];
        longitude_calc = longitude;
        longitude_calc /= 10000000;
        latitude |= gps_data[37];
        latitude = latitude << 8;
        latitude |= gps_data[36];
        latitude = latitude << 8;
        latitude |= gps_data[35];
        latitude = latitude << 8;
        latitude |= gps_data[34];
        latitude_calc = latitude;
        latitude_calc /= 10000000;
        Serial.print(latitude_calc, 7);
        Serial.print(" ");
        Serial.println(longitude_calc, 7);
        /*for(int j = 0; j < i; j++){
          Serial.print(gps_data[j], HEX);
          Serial.print(" ");
        }
        Serial.println();*/
      }
    }
    delay(100);
    g++;
  }
  for(int d = 0; d < 10; d++){ //Temperature and humidity readout
    temperatura = dht.readTemperature();
    wilgotnosc = dht.readHumidity();
    if(!isnan(temperatura) && !isnan(wilgotnosc)){
      break;
    }
  }
  

  // Printing the obtained data to the serial monitor for debugging
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  Serial.print("Wilgotność: ");
  Serial.print(wilgotnosc);
  Serial.println(" %\n\n");
  delay(1000);
  
  //Preparing the SMS message
  datatoSMS[0] = '\0';
  dtostrf(temperatura, 8, 6, tempch);
  dtostrf(wilgotnosc, 8, 6, humidch);
  strcat(datatoSMS, "T: ");
  strcat(datatoSMS, tempch);
  strcat(datatoSMS, " H: ");
  strcat(datatoSMS, humidch);
  char pos_buffer[10];
  dtostrf(latitude_calc, 8, 6, pos_buffer);
  strcat(datatoSMS, " Lat: ");
  strcat(datatoSMS, pos_buffer);
  dtostrf(longitude_calc, 8, 6, pos_buffer);
  strcat(datatoSMS, " Lon: ");
  strcat(datatoSMS, pos_buffer);
  Serial.println(datatoSMS);
  sendSMS(phonenumber, datatoSMS);

  //Powering off modem, going into sleep mode for 15 minutes
  powerOffModem();

  for(int k = 0; k < 30; k++){
    sleep_cpu();
  }
}
