require './base'

class PingTest < Base

  def initialize
    super
  end

  def test
    pingtest
  end

  def pingtest
    query = query_string(["ping"])
    (0..NUMBER_OF_TEST_CASES).each do |i|
      expected = "+PONG\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end