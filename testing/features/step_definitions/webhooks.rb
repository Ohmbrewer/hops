require 'particlerb'
require 'rspec/expectations'

And(/^the Rhizome has a webhook for Pump (\d+)$/) do |pump_id|
  if @particle_client.webhooks.any? { |wh| wh.event == "pumps/#{pump_id}" }
    @pump_webhook = @particle_client.webhooks.find { |wh| wh.event == "pumps/#{pump_id}" }
  else
    pending 'Get the webhook working correctly...'
    # expect {
    #   @pump_webhook = Particle.webhook()
    #                       .create
    # }.to_not raise_exception
  end
end

Then(/^I receive a webhook message confirming success$/) do
  @pump_webhook.get_attributes
  puts @pump_webhook.response
  puts @pump_webhook.error
  pending 'Need to confirm that the webhook message matches the args message sent earlier'
end
