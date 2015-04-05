require 'lib/load_config.rb'
require 'ruby_spark'
require 'rspec/expectations'

Given(/^the Spark is configured$/) do
  hops_basedir = "#{File.dirname(__FILE__)}/../../../../.."
  config_settings = "#{hops_basedir}/config/settings.yml"
  settings = LoadConfig::from_file(config_settings)

  RubySpark.configuration do |config|
    config.access_token = settings[:spark][:access_token]
    config.timeout = 60
  end

  @spark = RubySpark::Core.new settings[:spark][:device_id]
end

Given(/^the Spark is alive$/) do
  info = @spark.info
  expect(info).to_not be_nil
  expect(info).to be_a(Hash)
  expect(info).to_not be_empty
end

Given(/^I post this message to the Spark's (.*) path:$/) do |spark_func, msg_table|
  msg = msg_table.raw.first.first
  result = @spark.function(spark_func, msg)
  expect(result).to eq 1
end

When(/^I check the Spark's (.*) variable$/) do |spark_var|
  instance_variable_set "@#{spark_var}", @spark.variable(spark_var)
end

Then(/^the (.*) variable reads:$/) do |spark_var, msg_table|
  msg = msg_table.raw.first.first
  expect(instance_variable_get("@#{spark_var}")).to eq msg
end