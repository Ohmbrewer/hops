Feature: Rhizome operates a pump

  Background:
    Given the Rhizome is configured
    And   the Rhizome is connected
    And   the Rhizome has a webhook for Pump 1
    # Additionally, the Rhizome must be physically connected to the pump equipment
    # or the "Fake Pump" rig must be wired up.

  @uc_re_1
  Scenario: Rhizome operates a pump based on a message sent over the network
    When  I send this message to the Rhizome's Pump 1 function:
      | state     | on                |
      | stop time | after 60 minutes  |
      | speed     | 1                 |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success

    When  I send this message to the Rhizome's Pump 1 function:
      | state    | off   |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success

  @uc_re_3
  Scenario: Rhizome varies the pump speed
    When  I send this message to the Rhizome's Pump 1 function:
      | state     | on                |
      | stop time | after 30 minutes  |
      | speed     | 2                 |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success

    When  I send this message to the Rhizome's Pump 1 function:
      | state     | on                |
      | stop time | after 60 minutes  |
      | speed     | 3                 |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success

  @uc_re_4
  Scenario: Rhizome deactivates a pump based on a given stopping time
    When  I send this message to the Rhizome's Pump 1 function:
      | state     | on                |
      | stop time | after 15 seconds  |
      | speed     | 3                 |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success

    When I wait 10 seconds
    Then I receive a webhook message confirming the Rhizome shutdown Pump 1 on its own
