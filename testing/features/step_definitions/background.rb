require_relative '../../lib/load_config'
require 'particlerb'
require 'rspec/expectations'

Given(/^the Rhizome is configured$/) do
  @global_settings = Hash.new if @global_settings.nil?
  @global_settings[:rig] ||= LoadConfig::from_file(ENV['rhizomes'])['development'][ENV['rig'].to_sym]
  @global_settings[:endpoint] = ENV['endpoint']

  @particle_client = Particle::Client.new(access_token: @global_settings[:rig][:access_token])
  @rhizome = @particle_client.device(@global_settings[:rig][:core_id])
end

Given(/^the Rhizome is connected$/) do
  info = @rhizome.attributes
  expect(info).to_not be_nil
  expect(info).to be_a(Hash)
  expect(info).to_not be_empty
  expect(info[:connected]).to be true
end