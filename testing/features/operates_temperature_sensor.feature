Feature: Rhizome operates a temperature sensor

  Background:
    Given the Rhizome is configured
    And   the Rhizome is connected
    And   the Rhizome has a webhook for Temperature Sensor 1
    # Additionally, the Rhizome must be physically connected to the sensor equipment
    # or the "Fake Temperature Sensor" rig must be wired up.

  @uc_re_5
  Scenario: Rhizome retrieves a temperature sensor reading
    When  I wait for a webhook message from Temperature Sensor 1 for no longer than 30 seconds
    Then  the temperature reading for Temperature Sensor 1 is within 0.5 degrees of 68.888 degrees Celsius
    # Note that this doesn't do the display testing listed in the Use Case, you'll need to verify that manually.