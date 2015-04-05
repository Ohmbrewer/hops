Feature: Sending and Receiving From A Spark Device

  Background:
    Given the Spark is configured
    And   the Spark is alive

  Scenario: Save a message
    Given I post this message to the Spark's tx path:
      | Hey! Listen! |
    When  I check the Spark's rx variable
    Then  the rx variable reads:
      | Hey! Listen! |
