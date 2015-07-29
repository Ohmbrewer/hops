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
              rhizome: '{{SPARK_CORE_ID}}'
          }).create
    }.to_not raise_exception
  end
end

Then(/^I receive a webhook message confirming success$/) do
  webhook_result = JSON.parse(HTTParty.get("#{@global_settings[:endpoint]}/last/pumps")
                                      .parsed_response,
                              symbolize_names: true)

  args = @last_args_str.split(',')
  expect(webhook_result[:id]).to eq args[0]
  expect(webhook_result[:state]).to eq args[1]
  expect(webhook_result[:speed]).to eq args[2] if args.length > 2

end
