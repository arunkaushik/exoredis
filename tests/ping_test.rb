require './base'

class PingTest < Base

  def initialize
    super
  end

  def test
    pingtest
  end

  def pingtest
    query = "*1\r\n$4\r\nping\r\n"
    (0..NUMBER_OF_TEST_CASES).each do |i|
      expected = "+PONG\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end