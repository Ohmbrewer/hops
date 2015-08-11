#include "Ohmbrewer_Rhizome_Pump.h"
#include "Touch_4Wire/Touch_4Wire.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_mfGFX/Adafruit_mfGFX.h"
// #include "OneWire.h"
// #include "ds18b20.h"

//temp variable definitions
int sensor_dig = D1;
int sensor_analog = D0;
char temperatureInfo[64];
float fahrenheit = 156;
float celsius = 68.8889;
double f;

double target_temp_1 = 100; //0;
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
int relays[6] = {0,0,0,0,0,0};


//object initialization
// DS18B20 ds18b20 = DS18B20(sensor_dig);

// Set the screen object
Adafruit_ILI9341 tft = Adafruit_ILI9341(D6, D7, A6);

// Using these variables to capture presses
int xcord;
int ycord;

// Button stuff
#define XP A1
#define YP A0
#define XM A7
#define YM A2
#define MINPRESSURE 50
#define MAXPRESSURE 4000

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 500
#define TS_MINY 300
#define TS_MAXX 3650
#define TS_MAXY 3650
// #define TS_MINX 150
// #define TS_MINY 130
// #define TS_MAXX 3800
// #define TS_MAXY 4000
// #define TS_MINX 150
// #define TS_MINY 120
// #define TS_MAXX 920
// #define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// In the example it was 285 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 0); // 285);

#define BUTTONSIZE 60
#define LEFT 0
#define RIGHT 240
#define TOP 0
#define BOTTOM 320
#define BUTTONTOP 260



Ohmbrewer_Pump ob_p = Ohmbrewer_Pump("1");

//####################################################   temps

/**
 * Preps any pins or variables that aren't set above. Setup runs once when device is powered or rebooted.
 */
void setup() {
    initScreen();
    
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
    
    //set LED pins to outputs
    RGB.control(true);
    Spark.function("pumps", relayCtrl);
    
    // For good measure, let's also make sure the LED is off when we start:
    ob_p.refreshLEDs();
	
}


/**
 * The meat of the program. Runs over and over while device is powered, after setup() has been called.
 */
void loop() {
    //do initial search to find all connected 1-Wire devices on the bus
//     if(!ds18b20.search()){
//     Serial.println("No more addresses.");
//     Serial.println();
//     ds18b20.resetsearch();
//   // delay(250);
//     return;
//     }
    //get temperature data
    // celsius = ds18b20.getTemperature();
    // fahrenheit = ds18b20.convertToFahrenheit(celsius);
    //convert data to more useful formats
    f = (double)fahrenheit;
    sprintf(temperatureInfo, "%2.2f", fahrenheit);
    //publish temperature string to any subscribed (listening) devices
    // Spark.publish("temperatureInfo", temperatureInfo);
    //output same string to the debug serial
    // Serial.println(temperatureInfo);
    
    //interface and check for updates? or analyze updates sent. .... or the webapp just changes the variables it wants to.

    // TODO: need an update for turning on and off the relays??

    // Do relay stuff. Flaunt it if ya got it.
    ob_p.checkForQuittingTime();
    relays[2] = ob_p.isRelayOn();

    // Finally, refresh the display

    // xcord = check_x();
    // ycord = check_y();
    // testcords(xcord,ycord);
    
    captureButtonPress();
    refreshDisplay();

}

// Handlers:
int relayCtrl(String command) {
    // Toggle the pump, etc.
    ob_p.pumpCtrl(command);
}



/* ============ Tactile Functions ============ */

/**
 * Captures a button press and does stuff with it.
 */
unsigned long captureButtonPress() {
	unsigned long start = micros();
	char printx [10];
	char printy [10];
	char status [40];

    // Retrieve a point
    TSPoint p = ts.getPoint();
    int z = p.z;
    
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (z < MINPRESSURE || z > MAXPRESSURE) {
        displayStatusUpdate("                                        ");
        return micros() - start;
    }
    
    // Get out of here if no one is touching
    if (p.z < 0) {
        displayStatusUpdate("                                        ");
        return micros() - start;
    }
    
    // Scale from ~0->1000 to tft.width using the calibration #'s
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width()) - 35; // This -35 is a dirty hack. We need to fix the scaling to get this working without it.
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    
    // Each of these should pad out with spaces on the right
	sprintf(printx, "x is %-5d", p.x);
	sprintf(printy, "y is %-5d", p.y);

    String statusUpdate = String(printx);
    statusUpdate.concat(printy);
    
    if (p.y >= BUTTONTOP) {
    
        if (p.x > 0 && p.x <= BUTTONSIZE) {
            // +                 12345678901234567890
            statusUpdate.concat("Pressing +!         ");
        } else if (p.x > BUTTONSIZE && p.x <= BUTTONSIZE*2) {
            // -
            statusUpdate.concat("Pressing -!         ");
        } else if (p.x > BUTTONSIZE*2 && p.x <= BUTTONSIZE*3) {
            // Menu
            statusUpdate.concat("Pressing Menu!      ");
        } else if (p.x > BUTTONSIZE*3 && p.x <= BUTTONSIZE*4) {
            // Select
            statusUpdate.concat("Pressing Select!    ");
        } else {
            // Nothing. Weird.
            statusUpdate.concat("                    ");
        }
        
        // Barf it onto the display...
        statusUpdate.toCharArray(status, 40);
        displayStatusUpdate(status);
    }

    
	return micros() - start;
}


//checks the x coordinate on screen touch
int check_x(){
    pinMode(YP, OUTPUT);
    pinMode(YM, OUTPUT);
    pinMode(XP,INPUT_PULLUP);
    pinMode(XM, INPUT);
    
    digitalWrite(YP, HIGH);
    digitalWrite(YM, LOW);
    delay(10);
    return analogRead(XP);
}

//checks the y coordinate on screen touch
int check_y(){
    pinMode(XP, OUTPUT);
    pinMode(XM, OUTPUT);
    pinMode(YP,INPUT_PULLUP);
    pinMode(YM, INPUT);
    
    digitalWrite(XP, HIGH);
    digitalWrite(XM, LOW);
    delay(10);
    return analogRead(YP);
}

//prints out the current x and y on the screen
unsigned long testcoords(int xc, int yc) {
	unsigned long start = micros();
	char printx [10];
	char printy [10];
	char status [21];

    // Each of these should pad out with spaces on the right
	sprintf(printx, "x is %-5d", xc);
	sprintf(printy, "y is %-5d", yc);

    String statusUpdate = String(printx);
    statusUpdate.concat(printy);
    statusUpdate.toCharArray(status, 21);
    
    displayStatusUpdate(status);

	return micros() - start;
}

/* ============ Display Functions ============ */

/**
 * Prints out a status message in the two rows above the buttons.
 * @param char* statusUpdate The status message to display. 40 characters or less.
 * @returns Time it took to run the function
 */
unsigned long displayStatusUpdate(char *statusUpdate) {
	unsigned long start = micros();

	tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.setCursor(LEFT, BUTTONTOP - 40);
	tft.setTextSize(2);
	tft.println(statusUpdate);
	
	return micros() - start;
}


/**
 * Initializes the display screen
 */
void initScreen() {
    
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
	drawButtons();
	
	// Reset the cursor
	tft.setCursor(LEFT, TOP);
}

/**
 * Initializes the display screen
 */
void drawButtons() {
    
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    
    // make the buttons
    tft.fillRect(LEFT, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_BLACK);
    tft.fillRect(BUTTONSIZE, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_BLACK);
    tft.fillRect(BUTTONSIZE*2, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_BLACK);
    tft.fillRect(BUTTONSIZE*3, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_BLACK);
    tft.drawRect(LEFT, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_GREEN);
    tft.drawRect(BUTTONSIZE, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_GREEN);
    tft.drawRect(BUTTONSIZE*2, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_GREEN);
    tft.drawRect(BUTTONSIZE*3, BUTTONTOP, BUTTONSIZE, BUTTONSIZE, ILI9341_GREEN);
	
	// Add symbols to the buttons
	tft.setTextSize(4);
	tft.setCursor(LEFT + 20, BUTTONTOP + 15);
	tft.print("+");
	tft.setCursor(BUTTONSIZE + 20, BUTTONTOP + 15);
	tft.print("-");
	
	tft.setTextSize(2);
	tft.setCursor((BUTTONSIZE*2) + 7, BUTTONTOP + 20);
	tft.print("Menu");
	tft.setTextSize(2);
	tft.setCursor((BUTTONSIZE*3) + 14, BUTTONTOP + 20);
	tft.print("Sel");
	
	tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
}

/**
 * Refreshes/redraws the touchscreen display
 * @returns Time it took to run the function
 */
unsigned long refreshDisplay() {
	unsigned long start = micros();
	
    displayHeader();
    displayTemps();
    displayRelays();
    
    // 500 seems like a good refresh delay
    delay(500);

	return micros() - start;
}

/**
 * Prints the status information for our current relays onto the touchscreen
 * @returns Time it took to run the function
 */
unsigned long displayRelays() {
	unsigned long start = micros();
	char relay_id[2];
    
	tft.println("====== Relays ======");
	for(int x=2; x<6; x++){
	    
	    // Print a fancy identifier
        tft.print(" [");
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        
        sprintf(relay_id,"%d", x-1);
        tft.print(relay_id);
        
        tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
        tft.print("]:");
        
        // Print the conditional status
	    if (relays[x]){
	        tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	        tft.println(" ON ");
	        
	    } else {
	        tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
	        tft.println(" OFF");
	    }
	    
	    // Always reset to green
	    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	}

	return micros() - start;
}

/**
 * Prints the temperature information for our sensors onto the touchscreen
 * @returns Time it took to run the function
 */
unsigned long displayTemps() {
	unsigned long start = micros();
	char probe_1 [24]; 
	char target_1 [24];
	
	sprintf(probe_1,"%2.2f",celsius);
	sprintf(target_1,"%2.2f",target_temp_1); 
	
	tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	tft.setTextSize(2);
	tft.println("");
	tft.print("= Temperature (");
	tft.write(247); // Degree symbol
	tft.println("C) =");

    // Print out the current temp
    tft.print(" Current: ");
    if(celsius > target_temp_1) {
	    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    } else if(celsius < target_temp_1) {
	    tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
    } else {
	    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    }
	tft.println(probe_1);
    
    // Print out the target temp
	tft.setTextColor(ILI9341_GREEN);
    tft.print(" Target:  ");
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
	tft.println(target_1);
    
    // Reset to green
	tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	
	// Add a wee space
	tft.println("");

	return micros() - start;
}

/**
 * Prints the "Ohmbrewer" title onto the touchscreen
 * @returns Time it took to run the function
 */
unsigned long displayHeader() {
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);  
	tft.setTextSize(3);
	tft.println("  OHMBREWER");
	return micros() - start;
}
