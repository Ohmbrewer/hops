/* echo-chamber.ino
 * A simple example of setting/getting a variable for a Spark device.
 * The variable is represented as a String and is available at /echo
 */

int echo(String message);

char lastMessage[255];

void setup() {
  // Set the function that we'll use for working with our variable
  Spark.function("tx", echo);
  Spark.variable("rx", lastMessage, STRING);
}

void loop() {
  // Nothing to do here...
}

int echo(String message) {
  strcpy(lastMessage,message.c_str());
  return 1;
}
