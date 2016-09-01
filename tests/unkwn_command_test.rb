require './base'

class UnknownCommand < Base

  KNOWN_COMMANDS = ["set", "get"]

  def initialize
    super
    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (3..10).each{|i| strs << random_string unless KNOWN_COMMANDS.include?(random_string.downcase)}
      q = query_string(strs)
      queries << q
    end
  end

  def test
    check_unknown_command
  end

  def check_unknown_command
    queries.each do |q|
      expected = "-ERR unknown command\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

end