// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_ILI9341/Adafruit_ILI9341.h"

// This #include statement was automatically added by the Particle IDE.
#include "OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "ds18b20.h"

// This #include statement was automatically added by the Spark IDE.
//#include "spark-dallas-temperature/spark-dallas-temperature.h"

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_mfGFX/Adafruit_mfGFX.h"

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_ILI9341/Adafruit_ILI9341.h"

#include "string.h"





//USB DEBUG
// find usb: ls -la /dev/tty.usb*
// screen /dev/tty.usbmodem1421 9600
//QUERY VARIABLES
//YOU MUST REPLACE THE {DEVICE_ID} AND THE {ACCESS_TOKEN} IN THE URLS BELOW
//get temp (string) variable
//curl https://api.spark.io/v1/devices/{DEVICE_ID}/temp?access_token={ACCESS_TOKEN}
//get "f" variable
//curl https://api.spark.io/v1/devices/{DEVICE_ID}/f?access_token={ACCESS_TOKEN}
//EVENTS
//notified when debug statement happens
//curl -H "Authorization: Bearer {ACCESS_TOKEN}" https://api.spark.io/v1/events/temperatureInfo



//temp variable definitions
int sensor_dig = D1;
int sensor_analog = D0;
char temperatureInfo[64];
float fahrenheit;
float celsius;
double f;

double target_temp_1 = 0;
double target_temp_2 = 0;

int relay_a = D2; 
int relay_b = D3;
int relay_c = D4;
int relay_d = D5;

//on/off state of relays 0 off, 1 on
int a = 0;
int b = 0;
int c = 0;
int d = 0;

//use the array instead of independent variables,
// index is related to Digital pin number, 0 to 0, 1 to 1...etc. 0 and 1 are the temp pins. 
//int relays[6] = {0,0,0,0,0,0};


//object initialization
DS18B20 ds18b20 = DS18B20(sensor_dig);

//screen init
Adafruit_ILI9341 tft = Adafruit_ILI9341(D6, D7, A6);
int x = A1;
int y = A0;
int xmin = A7;
int ymin = A2;
int xcord;
int ycord;


//####################################################   temps

//setup runs once when device is powered or rebooted
void setup() {
    
    tft.begin();

    //start serial communication for debugging
    //Serial.begin(9600);
    //define temps as a input
    pinMode(sensor_dig, INPUT);
    pinMode(sensor_analog, INPUT);
    //lights
    pinMode(relay_a, OUTPUT);
    pinMode(relay_b, OUTPUT);
    pinMode(relay_c, OUTPUT);
    pinMode(relay_c, OUTPUT);
    //define public variables
    Spark.variable("temp", temperatureInfo, STRING);
    Spark.variable("f", &f, DOUBLE);

	
}





//loop runs over and over while device is powered
void loop(void) {
    //do initial search to find all connected 1-Wire devices on the bus
    if(!ds18b20.search()){
    Serial.println("No more addresses.");
    Serial.println();
    ds18b20.resetsearch();
   // delay(250);
    return;
    }
    //get temerature data
    celsius = ds18b20.getTemperature();
    fahrenheit = ds18b20.convertToFahrenheit(celsius);
    //convert data to more useful formats
    f = (double)fahrenheit;
    sprintf(temperatureInfo, "%2.2f", fahrenheit);
    //publish temperature string to any subscribed (listening) devices
    Spark.publish("temperatureInfo", temperatureInfo);
    //output same string to the debug serial
    Serial.println(temperatureInfo);

//current execution code for display

    xcord = check_x();
    ycord = check_y();
    testcords(xcord,ycord);
    delay(500);
    
    //interface and check for updates? or analyze updates sent. .... or the webapp just changes the variables it wants to. 

    //disp_vars(celsius, target_temp_1);
    //delay(500);

//TODO need an update for turning on and off the relays??


}
// FUNCTIONS

//checks the x coordinate on screen touch
int check_x(){
    pinMode(y, OUTPUT);
    pinMode(ymin, OUTPUT);
    pinMode(x,INPUT_PULLUP);
    pinMode(xmin, INPUT);
    
    digitalWrite(y, HIGH);
    digitalWrite(ymin, LOW);
    delay(10);
    return analogRead(x);
}

//checks the y coordinate on screen touch
int check_y(){
    pinMode(x, OUTPUT);
    pinMode(xmin, OUTPUT);
    pinMode(y,INPUT_PULLUP);
    pinMode(ymin, INPUT);
    
    digitalWrite(x, HIGH);
    digitalWrite(xmin, LOW);
    delay(10);
    return analogRead(y);
}

//prints out the current x and y on the screen
unsigned long testcords(int xc, int yc) {
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
	tft.println("OHMBREWER!");
	tft.setTextColor(ILI9341_GREEN);

	tft.setTextSize(3);
	char printx [10]; 
	char printy [10]; 

	sprintf(printx,"x is %d",xc); 
	tft.println(printx);
	sprintf(printy,"y is %d",yc); 
	tft.println(printy);

	return micros() - start;
}

//prints out the current variable values on the screen
// unsigned long disp_vars(int p1, int temp_c) {
// 	tft.fillScreen(ILI9341_BLACK);
// 	unsigned long start = micros();
// 	tft.setCursor(0, 0);
// 	tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
// 	tft.println("OHMBREWER!");
// 	tft.setTextColor(ILI9341_GREEN);

// 	tft.setTextSize(2);
// 	char probe_1 [24]; 
// 	char target_1 [24]; 
// 	char relay_print [24];
	

// 	sprintf(probe_1,"current temp is %d C",p1); 
// 	tft.println(probe_1);
// 	sprintf(target_1,"target temp is %d C",temp_c); 
// 	tft.println(target_1);
// 	for(int x=2; x<5; x++){
// 	    if (relays[x]){
// 	        sprintf(relay_print,"Pin %d is ON", x); 
// 	        tft.println(relay_print);
// 	    }else{
// 	        sprintf(relay_print,"Pin %d is OFF", x); 
// 	        tft.println(relay_print);
// 	    }
// 	}

// 	return micros() - start;
// }


// //heartbeat function
// void flashLED(){
// digitalWrite(ledPin, HIGH);
// delay(500);
// digitalWrite(ledPin, LOW);
// delay(500);

// }

// void templed(){
//   if (celsius < 15 ){
//       digitalWrite(green, HIGH);
//       delay(500);
//       digitalWrite(green, LOW);
//   }
//   if (celsius >= 15 && celsius < 25){
//       digitalWrite(yellow, HIGH);
//       delay(500);
//       digitalWrite(yellow, LOW);
//   }
//   if (celsius > 25){
//       digitalWrite(red, HIGH);
//       delay(500);
//       digitalWrite(red, LOW);
//   }
// }


// //##################################################### sceen



// void setup() {
    


// 	tft.begin();



// }

// void loop(void) {


//     xcord = check_x();
//     ycord = check_y();
//     testcords(xcord,ycord);
//     delay(500);
    

// }






// // UNUSED ###############################################################
// // 	testFillScreen();
// // 	delay(500);

// //     testText();
// // 	delay(500);

// // 	testLines(ILI9341_CYAN);
// // 	delay(500);

// // 	testFastLines(ILI9341_RED, ILI9341_BLUE);
// // 	delay(500);

// // 	testRects(ILI9341_GREEN);
// // 	delay(500);

// // 	testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA);
// // 	delay(500);

// // 	testFilledCircles(10, ILI9341_MAGENTA);
// // 	delay(500);

// // 	testCircles(10, ILI9341_WHITE);
// // 	delay(500);

// // 	testTriangles();
// // 	delay(500);

// // 	testFilledTriangles();
// // 	delay(500);

// // 	testRoundRects();
// // 	delay(500);

// // 	testFilledRoundRects();
// // 	delay(500);

 
// // 	for (uint8_t rotation = 0; rotation<4; rotation++) {
// // 		tft.setRotation(rotation);
// // 		testText();
// // 		delay(2000);
// // 	}
	
// // 	delay(5000);

// unsigned long testFillScreen() {
// 	unsigned long start = micros();
// 	tft.fillScreen(ILI9341_BLACK);
// 	tft.fillScreen(ILI9341_RED);
// 	tft.fillScreen(ILI9341_GREEN);
// 	tft.fillScreen(ILI9341_BLUE);
// 	tft.fillScreen(ILI9341_BLACK);
// 	return micros() - start;
// }

// unsigned long testText() {
// 	tft.fillScreen(ILI9341_BLACK);
// 	unsigned long start = micros();
// 	tft.setCursor(0, 0);
// 	tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);

// 	tft.println("Hello World!");
// 	tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
// 	tft.println(1234.56);
// 	tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
// 	tft.println(0xDEADBEEF, HEX);
// //	tft.println();
// 	tft.setTextColor(ILI9341_GREEN);
// 	tft.setTextSize(3);
// 	tft.println("Groop");
// 	tft.setTextSize(2);
// 	tft.println("I implore thee,");
// 	tft.setTextSize(1);
// 	tft.println("my foonting turlingdromes.");
// 	tft.println("And hooptiously drangle me");
// 	tft.println("with crinkly bindlewurdles,");
// 	tft.println("Or I will rend thee");
// 	tft.println("in the gobberwarts");
// 	tft.println("with my blurglecruncheon,");
// 	tft.println("see if I don't!");

// 	return micros() - start;
// }

// unsigned long testLines(uint16_t color) {
// 	unsigned long start, t;
// 	int           x1, y1, x2, y2,
// 		w = tft.width(),
// 		h = tft.height();

// 	tft.fillScreen(ILI9341_BLACK);

// 	x1 = y1 = 0;
// 	y2 = h - 1;
// 	start = micros();
// 	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	x2 = w - 1;
// 	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	t = micros() - start; // fillScreen doesn't count against timing

// 	tft.fillScreen(ILI9341_BLACK);

// 	x1 = w - 1;
// 	y1 = 0;
// 	y2 = h - 1;
// 	start = micros();
// 	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	x2 = 0;
// 	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	t += micros() - start;

// 	tft.fillScreen(ILI9341_BLACK);

// 	x1 = 0;
// 	y1 = h - 1;
// 	y2 = 0;
// 	start = micros();
// 	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	x2 = w - 1;
// 	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	t += micros() - start;

// 	tft.fillScreen(ILI9341_BLACK);

// 	x1 = w - 1;
// 	y1 = h - 1;
// 	y2 = 0;
// 	start = micros();
// 	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
// 	x2 = 0;
// 	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

// 	return micros() - start;
// }

// unsigned long testFastLines(uint16_t color1, uint16_t color2) {
// 	unsigned long start;
// 	int           x, y, w = tft.width(), h = tft.height();

// 	tft.fillScreen(ILI9341_BLACK);
// 	start = micros();
// 	for (y = 0; y<h; y += 5) tft.drawFastHLine(0, y, w, color1);
// 	for (x = 0; x<w; x += 5) tft.drawFastVLine(x, 0, h, color2);

// 	return micros() - start;
// }

// unsigned long testRects(uint16_t color) {
// 	unsigned long start;
// 	int           n, i, i2,
// 		cx = tft.width() / 2,
// 		cy = tft.height() / 2;

// 	tft.fillScreen(ILI9341_BLACK);
// 	n = min(tft.width(), tft.height());
// 	start = micros();
// 	for (i = 2; i<n; i += 6) {
// 		i2 = i / 2;
// 		tft.drawRect(cx - i2, cy - i2, i, i, color);
// 	}

// 	return micros() - start;
// }

// unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
// 	unsigned long start, t = 0;
// 	int           n, i, i2,
// 		cx = tft.width() / 2 - 1,
// 		cy = tft.height() / 2 - 1;

// 	tft.fillScreen(ILI9341_BLACK);
// 	n = min(tft.width(), tft.height());
// 	for (i = n; i>0; i -= 6) {
// 		i2 = i / 2;
// 		start = micros();
// 		tft.fillRect(cx - i2, cy - i2, i, i, color1);
// 		t += micros() - start;
// 		// Outlines are not included in timing results
// 		tft.drawRect(cx - i2, cy - i2, i, i, color2);
// 	}

// 	return t;
// }

// unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
// 	unsigned long start;
// 	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

// 	tft.fillScreen(ILI9341_BLACK);
// 	start = micros();
// 	for (x = radius; x<w; x += r2) {
// 		for (y = radius; y<h; y += r2) {
// 			tft.fillCircle(x, y, radius, color);
// 		}
// 	}

// 	return micros() - start;
// }

// unsigned long testCircles(uint8_t radius, uint16_t color) {
// 	unsigned long start;
// 	int           x, y, r2 = radius * 2,
// 		w = tft.width() + radius,
// 		h = tft.height() + radius;

// 	// Screen is not cleared for this one -- this is
// 	// intentional and does not affect the reported time.
// 	start = micros();
// 	for (x = 0; x<w; x += r2) {
// 		for (y = 0; y<h; y += r2) {
// 			tft.drawCircle(x, y, radius, color);
// 		}
// 	}

// 	return micros() - start;
// }

// unsigned long testTriangles() {
// 	unsigned long start;
// 	int           n, i, cx = tft.width() / 2 - 1,
// 		cy = tft.height() / 2 - 1;

// 	tft.fillScreen(ILI9341_BLACK);
// 	n = min(cx, cy);
// 	start = micros();
// 	for (i = 0; i<n; i += 5) {
// 		tft.drawTriangle(
// 			cx, cy - i, // peak
// 			cx - i, cy + i, // bottom left
// 			cx + i, cy + i, // bottom right
// 			tft.Color565(0, 0, i));
// 	}

// 	return micros() - start;
// }

// unsigned long testFilledTriangles() {
// 	unsigned long start, t = 0;
// 	int           i, cx = tft.width() / 2 - 1,
// 		cy = tft.height() / 2 - 1;

// 	tft.fillScreen(ILI9341_BLACK);
// 	start = micros();
// 	for (i = min(cx, cy); i>10; i -= 5) {
// 		start = micros();
// 		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
// 			tft.Color565(0, i, i));
// 		t += micros() - start;
// 		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
// 			tft.Color565(i, i, 0));
// 	}

// 	return t;
// }

// unsigned long testRoundRects() {
// 	unsigned long start;
// 	int           w, i, i2,
// 		cx = tft.width() / 2 - 1,
// 		cy = tft.height() / 2 - 1;

// 	tft.fillScreen(ILI9341_BLACK);
// 	w = min(tft.width(), tft.height());
// 	start = micros();
// 	for (i = 0; i<w; i += 6) {
// 		i2 = i / 2;
// 		tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.Color565(i, 0, 0));
// 	}

// 	return micros() - start;
// }

// unsigned long testFilledRoundRects() {
// 	unsigned long start;
// 	int           i, i2,
// 		cx = tft.width() / 2 - 1,
// 		cy = tft.height() / 2 - 1;

// 	tft.fillScreen(ILI9341_BLACK);
// 	start = micros();
// 	for (i = min(tft.width(), tft.height()); i>20; i -= 6) {
// 		i2 = i / 2;
// 		tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.Color565(0, i, 0));
// 	}

// 	return micros() - start;
// }


