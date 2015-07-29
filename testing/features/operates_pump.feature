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
      | state    | on    |
      | speed    | 3     |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success
    When  I send this message to the Rhizome's Pump 1 function:
      | state    | off   |
    # Give a little time for network lag...
    And   I wait 5 seconds
    Then  I receive a webhook message confirming success
