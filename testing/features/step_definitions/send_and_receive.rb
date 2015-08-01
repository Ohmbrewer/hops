require 'particlerb'
require 'rspec/expectations'
require_relative '../../lib/utilities'

Given(/^I post this message to the Rhizome's (.*) path:$/) do |spark_func, msg_table|
  msg = msg_table.raw.first.first
  result = @rhizome.function(spark_func, msg)
  expect(result).to eq 1
end

When(/^I check the Rhizome's (.*) variable$/) do |spark_var|
  instance_variable_set "@#{spark_var}", @rhizome.variable(spark_var)
end

Then(/^the (.*) variable reads:$/) do |spark_var, msg_table|
  msg = msg_table.raw.first.first
  expect(instance_variable_get("@#{spark_var}")).to eq msg
end

When(/^I send this message to the Rhizome's Pump (\d+) function:$/) do |pump_id, args_table|
  # Unfortunately, order matters... we want ID,STATE,SPEED
  # ID and STATE are required, speed is not.
  @last_args_str = "#{pump_id},#{args_table.rows_hash['state']}"

  @last_args_str += ",#{get_named_time(args_table.rows_hash['stop time'])}" unless args_table.rows_hash['stop time'].nil?

  @last_args_str += ",#{args_table.rows_hash['speed']}" unless args_table.rows_hash['speed'].nil?

  expect { @rhizome.function('pumps', @last_args_str) }.to_not raise_exception
end

And(/^I wait (\d+) seconds$/) do |secs|
  sleep secs.to_i
end
