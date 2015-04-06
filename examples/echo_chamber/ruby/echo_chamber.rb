#!/usr/bin/env ruby
require_relative 'lib/load_config.rb'
require 'ruby_spark'
hops_basedir = "#{File.dirname(__FILE__)}/../../.."
config_settings = "#{hops_basedir}/config/settings.yml"

puts "INFO: Going to read settings from this file:"
puts config_settings.inspect

settings = LoadConfig::from_file(config_settings)

puts "INFO: Found the following settings:"
puts settings.inspect

puts "INFO: Configuring the Spark.io connection"
RubySpark.configuration do |config|
  config.access_token = settings[:spark][:access_token]
  config.timeout = 60
end

puts "INFO: Pinging the Spark!"
spark = RubySpark::Core.new settings[:spark][:device_id]
puts spark.info

msg = "Hey! Listen!"
puts "INFO: Sending the following message to /echo > \"#{msg}\""
puts spark.function('tx', msg)

puts "INFO: Checking if the message was saved..."
puts spark.variable('rx')
