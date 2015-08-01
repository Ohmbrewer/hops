# Returns a named expression of time as seconds after the epoch
# @param [String] time_expression An expression of a duration, such as "after 30 seconds" or "right now"
# @return [Integer] Seconds after the epoch represented by the given expression
def get_named_time(time_expression)
  time_expression.chomp!
  expr_parts = time_expression.split(' ')
  prefix = 0
  amount = 1
  unit = 2

  case expr_parts.length
    when 1
      if time_expression == 'right now'
        return Time.now.to_i
      end
    when 3
      if expr_parts[prefix] == 'after'
        case expr_parts[unit]
          when 'seconds'
            return (Time.now + expr_parts[amount].to_i).to_i
          when 'minutes'
            return (Time.now + (expr_parts[amount].to_i * 60)).to_i
          when 'hours'
            return (Time.now + (expr_parts[amount].to_i * 360)).to_i
          when 'days'
            return (Time.now + (expr_parts[amount].to_i * 360 * 24)).to_i
        end
      end
  end

  raise ArgumentError, "I don't know what you mean by: '#{time_expression}'..."
end
