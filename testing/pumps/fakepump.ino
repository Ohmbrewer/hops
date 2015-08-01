#define publish_delay 10000

// Used to store the intensity level of the individual LEDs, intitialized to off (0)
int redIntensity = 0;
int greenIntensity = 0;
int blueIntensity = 0;

// Provide some named default colors
String RED = "255,0,0";
String YELLOW = "255,255,0";
String BLUE = "0,0,255";
String GREEN = "0,255,0";
String FULL_BLAST = "255,255,255";
String BLACKOUT = "0,0,0";

// We'll also map the default colors to pump speeds, for a nice visual
const int isRed    = 1;
const int isYellow = 2;
const int isGreen  = 3;
const int isBlue   = 4;
const int isOut    = 0;

// Here's some things that we need to keep track of:
String pumpID   = "1";
unsigned int lastPublish = 0;
int stopTime    = -1;
String state    = "off";
String speed    = "-1";

// Event stream names
String error_stream = "rhizome_errors";
String debug_stream = "rhizome_debug";
String pump_stream = "pumps/1"; // Probably want to actually set this programmatically in the real world

/**
 * Sets the LED intensity values according to the global variables.
 */
void refreshLEDs() {
    RGB.color(redIntensity, greenIntensity, blueIntensity);
}

/**
 * Sets the LED intensity values to off (0).
 */
void turnOffLEDs() {
    RGB.color(0, 0, 0);
    state = "off";
}

/**
 * Convenience function for another way to say refreshLEDs()
 */
void turnOnLEDs() {
    refreshLEDs();
    state = "on";
}

/**
 * Sets up stuff
 */
void setup() {
  //set LED pins to outputs
  RGB.control(true);
  Spark.function("pumps", pumpCtrl);
  
  // For good measure, let's also make sure the LED is off when we start:
  refreshLEDs();
}

/**
 * Parses a String of RGB values and uses the result to set the LED intensities and change the LED color.
 * @param command The comma-delimited RGB value string
 */
int setColor(String command) {
    String jsonDebugMsg = "{ \"msg\": \"Attempting to set color!\", \"command\": \"" + command + "\"}";
    Spark.publish(debug_stream, jsonDebugMsg, 60, PRIVATE);
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
    String jsonDebugMsg = "{ \"msg\": \"Attempting to toggle LEDs!\", \"command\": \"" + command + "\"}";
    Spark.publish(debug_stream, jsonDebugMsg, 60, PRIVATE);
    if (command.equalsIgnoreCase("on")) {
        turnOnLEDs();
        return 1;
    } else if (command.equalsIgnoreCase("off")) {
        turnOffLEDs();
        return 0;
    } else {
        String jsonErrorMsg = "{ \"msg\": \"Toggle did not work...\", \"command\": \"" + command + "\"}";
        Spark.publish(error_stream, jsonErrorMsg, 60, PRIVATE);
        return -136;
    }
}

/**
 * Sets the "pump" speed, here represented by the LED.
 * @param speed The provided pump speed
 */
int setSpeed(String speedUpdate) {
    String jsonDebugMsg = "{ \"msg\": \"Attempting to set pump speed!\", \"command\": \"" + speedUpdate + "\"}";
    Spark.publish(debug_stream, jsonDebugMsg, 60, PRIVATE);
    int speedSuccess = -1;
    if(speedUpdate != NULL) {
        // Set the color
        switch (speedUpdate.toInt()) {
          case isRed:
            speedSuccess = setColor(RED);
            break;
          case isYellow:
            speedSuccess = setColor(YELLOW);
            break;
          case isGreen:
            speedSuccess = setColor(GREEN);
            break;
          case isBlue:
            speedSuccess = setColor(BLUE);
            break;
          case isOut:
            // This code should make the color saved in memory equal to no light.
            speedSuccess = setColor(BLACKOUT);
            break;
          default:
            setColor(FULL_BLAST);
            speedSuccess = -2;
            String jsonErrorMsg = "{ \"msg\": \"Invalid color supplied...\", \"speed\": \"" + speedUpdate + "\"}";
            Spark.publish(error_stream, jsonErrorMsg, 60, PRIVATE);
        }
    }
    
    if(speedSuccess == 1) {
        speed = speedUpdate;
    }
    return speedSuccess;
}

/**
 * Compares the current time against the stop time provided in the last instruction and kills the pump
 * if we've reached quitting time.
 */
void checkForQuittingTime() {
    // First of all, there's nothing to do if we aren't pumping. That'll happen if
    // 1) Stop time < 0 (it starts at -1)
    // 2) State == Off
    if(stopTime > 0 || state.equalsIgnoreCase("on") ) {
        if(Time.now() <= stopTime) {
            int toggleSuccess = toggleLEDs("off");
            String jsonErrorMsg = "{ \"msg\": \"Stopping time reached! Tried to stop the pump, but for some reason it did not stop!\","
                                   " \"state\": \"" + state + "\", \"error_code\": \"" + toggleSuccess + "\"}";
                                   
            if(toggleSuccess != 0) {
                // Pump failed to shut off
                Spark.publish(error_stream, jsonErrorMsg, 60, PRIVATE);
            } else {
                // Success!
                String jsonSuccessMsg = "{ \"msg\": \"Stopping time reached. Pump deactivated.\", \"state\": \"" + state + "\"}";
                Spark.publish(error_stream, jsonSuccessMsg, 60, PRIVATE);
            }
        }
    }
}

/**
 * Nothing to do here but see if it's time to stop...
 */
void loop() {
    // checkForQuittingTime();
}

//** Handlers **//


/**
 * Publishes the status of the pump
 * @param state The current state (on/off)
 * @param speed The current speed (1..4)
 */
void publishStatus() {
    unsigned long now = millis();
    String jsonStatusA = "{ \"id\": \"" + pumpID + "\", \"state\": \"" + state + "\", \"stopTime\":\"" + stopTime + "\", \"speed\": \"" + speed + "\"}";
    String jsonStatusB = "{ \"id\": \"" + pumpID + "\", \"state\": \"" + state + "\", \"stopTime\":\"" + stopTime + "\"}";
    
    if(speed != NULL) {
        Spark.publish(pump_stream, jsonStatusA, 60, PRIVATE);
    } else {
        Spark.publish(pump_stream, jsonStatusB, 60, PRIVATE);
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
    int    speedSuccess  = -1;
    int    toggleSuccess = -1;
    char * params        = new char[command.length() + 1];
    strcpy(params, command.c_str());
    
    // Parse the parameters
    String desiredPumpID  = String(strtok(params, ","));
    String stateUpdate    = String(strtok(NULL, ","));
    int stopTimeUpdate    = atoi(strtok(NULL, ","));
    String speedUpdate    = "";
    if(stopTimeUpdate > 0) {
        // Let's try to avoid null pointer exceptions...
        speedUpdate = "" + String(strtok(NULL, ",")); // Note that the quotes are a workaround for a known issue with the String class
    }

    if((desiredPumpID == NULL) || (!desiredPumpID.equalsIgnoreCase(pumpID))) {
        // Not doing anything with it right now, but we'll error out if it isn't provided properly
        return -9000;
    }
    
    // if(stateUpdate.equalsIgnoreCase("on") && (stopTimeUpdate < Time.now())) {
    //     // You need to provide a stop time in the future...
    //     return -Time.now();
    // } else {
    //     // Otherwise, go ahead and set the stop time
    //     stopTime = stopTimeUpdate;
    // }
    
    speedSuccess = setSpeed(speedUpdate);
    
    if(state != NULL) {
        // Toggle the LED, if appropriate
        toggleSuccess = toggleLEDs(stateUpdate);
    }
    
    publishStatus();
    if(speedSuccess > 0 && toggleSuccess != -136) {
        return speedSuccess;
    }
    
    return toggleSuccess;
}
