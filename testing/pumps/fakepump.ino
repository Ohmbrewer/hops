#define publish_delay 10000

unsigned int lastPublish = 0;

// // First, let's create our "shorthand" for the pins:
const int RED_LED_PIN = A4;
const int GREEN_LED_PIN = A1;
const int BLUE_LED_PIN = A0;

// Used to store the intensity level of the individual LEDs, intitialized to off (255)
int redIntensity = 255;
int greenIntensity = 255;
int blueIntensity = 255;

// Provide some named default colors
String RED = "0,255,255";
String YELLOW = "0,0,255";
String BLUE = "255,255,0";
String GREEN = "255,0,255";
String FULL_BLAST = "0,0,0";
String BLACKOUT = "255,255,255";

// Provide the pump ID
int pumpID = 1;

/**
 * Sets the LED intensity values according to the global variables.
 */
void refreshLEDs() {
    analogWrite(RED_LED_PIN, redIntensity);
    analogWrite(GREEN_LED_PIN, greenIntensity);
    analogWrite(BLUE_LED_PIN, blueIntensity);
}

/**
 * Sets the LED intensity values to off (255).
 */
void turnOffLEDs() {
    analogWrite(RED_LED_PIN, 255);
    analogWrite(GREEN_LED_PIN, 255);
    analogWrite(BLUE_LED_PIN, 255);
}

/**
 * Convenience function for another way to say refreshLEDs()
 */
void turnOnLEDs() {
    refreshLEDs();
}

/**
 * Sets up stuff
 */
void setup() {
  //set LED pins to outputs
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  Spark.function("pump", pumpCtrl);
  
  // For good measure, let's also make sure the LED is off when we start:
  refreshLEDs();
}

/**
 * Parses a String of RGB values and uses the result to set the LED intensities and change the LED color.
 * @param command The comma-delimited RGB value string
 */
int setColor(String command) {
    if (command) {
        char inputStr[64];
        command.toCharArray(inputStr,64);
        char *p = strtok(inputStr,",");
        redIntensity = atoi(p);
        p = strtok(NULL,",");
        greenIntensity = atoi(p);
        p = strtok(NULL,",");
        blueIntensity = atoi(p);
        p = strtok(NULL,",");
        
        refreshLEDs();
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * Parses a String of RGB values and uses the result to set the LED intensities and change the LED color.
 * @param command Either "on" or "off". Not case sensitive.
 */
int toggleLEDs(String command) {
    /* Spark.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */

    if (command.equalsIgnoreCase("on")) {
        turnOnLEDs();
        return 1;
    }
    else if (command.equalsIgnoreCase("off")) {
        turnOffLEDs();
        return 0;
    }
    else {
        return -1;
    }
}

/**
 * Nothing to do here...
 */
void loop() {}

//** Handlers **//


/**
 * Publishes the status of the pump
 * @param state The current state (on/off)
 * @param speed The current speed (1..4)
 */
void publishStatus(String state, String speed) {
    unsigned long now = millis();
    String eventName = "pumps/" + pumpID;
    String jsonStatusA = "{ \"state\": \"" + state + "\", \"speed\": \"" + speed + "\"}";
    String jsonStatusB = "{ \"state\": \"" + state + "\"}";
    
    if(speed != NULL) {
        Spark.publish(eventName, jsonStatusA, 60, PRIVATE);
    } else {
        Spark.publish(eventName, jsonStatusB, 60, PRIVATE);
    }
    
    lastPublish = now;
}

/*
 * This funtion will turn on/off the LED and set it to the appropriate color
 * Usage - "args=ON,1" will turn the LED on and set the color to Red
 * Color settings are:
 * 1 - RED
 * 2 - YELLOW
 * 3 - GREEN
 * 4 - BLUE
 * Errors (invalid color choices) will set the LED to full power for all RGB.
 * @param command The current state (on/off) and speed (1..4), comma-delimited and case-insensitive
 */
int pumpCtrl(String command) {
    char * params = new char[command.length() + 1];
    int colorSuccess = -1;
    int toggleSuccess = -1;

    strcpy(params, command.c_str());
    String state = String(strtok(params, ","));
    String speed = String(strtok(NULL, ","));

    if(speed != NULL) {
        // Set the color
        switch (speed.toInt()) {
          case 1:
            colorSuccess = setColor(RED);
            break;
          case 2:
            colorSuccess = setColor(YELLOW);
            break;
          case 3:
            colorSuccess = setColor(GREEN);
            break;
          case 4:
            colorSuccess = setColor(BLUE);
            break;
          default:
            setColor(FULL_BLAST);
            colorSuccess = -2;
        }
    }
    
    if(state != NULL) {
        // Toggle the LED, if appropriate
        toggleSuccess = toggleLEDs(state);
    }
    
    publishStatus(state, speed);
    if(colorSuccess > 0 && toggleSuccess != -1) {
        return colorSuccess;
    }
    
    return -1;
}
