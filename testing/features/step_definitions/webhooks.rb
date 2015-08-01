require 'particlerb'
require 'rspec/expectations'
require 'httparty'
require 'json'

And(/^the Rhizome has a webhook for Pump (\d+)$/) do |pump_id|
  if @particle_client.webhooks.any? { |wh| wh.event == "pumps/#{pump_id}" }
    @pump_webhook = @particle_client.webhooks.find { |wh| wh.event == "pumps/#{pump_id}" }
  else
    expect {
      @pump_webhook = @particle_client.webhook(
          mydevices: true,
          deviceid: @rhizome.id,
          event: "pumps/#{pump_id}",
          url: "#{@global_settings[:endpoint]}/pumps",
          json: {
              id:      '{{id}}',
              state:   '{{state}}',
              speed:   '{{speed}}',
              stopTime:   '{{stopTime}}',
              rhizome: '{{SPARK_CORE_ID}}'
          }).create
    }.to_not raise_exception
  end
end

Then(/^I receive a webhook message confirming success$/) do
  k = {
      id:       0,
      state:    1,
      stopTime: 2,
      speed:    3
  }
  get_result = HTTParty.get("#{@global_settings[:endpoint]}/last/pumps")
                       .parsed_response
  webhook_result = JSON.parse(get_result,
                              symbolize_names: true)

  args = @last_args_str.split(',')
  expect(webhook_result[:id]).to eq args[k[:id]]
  expect(webhook_result[:state]).to eq args[k[:state]]
  expect(webhook_result[:stopTime]).to eq args[k[:stopTime]] if args.length > 2
  expect(webhook_result[:speed]).to eq args[k[:speed]] if args.length > 3

end


Then(/^I receive a webhook message confirming the Rhizome shutdown Pump (\d+) on its own$/) do |pump_id|
  get_result = HTTParty.get("#{@global_settings[:endpoint]}/last/pumps")
                       .parsed_response
  webhook_result = JSON.parse(get_result,
                              symbolize_names: true)

  stop_time_arg = @last_args_str.split(',')[2]
  expect(webhook_result[:id]).to eq pump_id
  expect(webhook_result[:state]).to match /#{'off'}/i
  expect(webhook_result[:stopTime]).to eq stop_time_arg
  # We're assuming that the shutoff method doesn't wipe the stopTime value as shutting off manually does.
end