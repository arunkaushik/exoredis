require './base'

class WrongNumberOfArguments < Base

  def initialize
    super
    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (3..10).each{|i| strs << random_string}
      strs << "arun" if strs.length % 2 == 0
      q = "*#{strs.length+1}\r\n$3\r\nset\r\n"
      strs.each{|s| q = "#{q}$#{s.length}\r\n#{s}\r\n"}
      queries << q
    end

    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (2..8).each{|i| strs << random_string}
      q = "*#{strs.length+1}\r\n$3\r\nget\r\n"
      strs.each{|s| q = "#{q}$#{s.length}\r\n#{s}\r\n"}
      queries << q
    end
  end

  def test
    check_wrong_num_args
  end

  def check_wrong_num_args
    queries.each do |q|
      expected = "-ERR wrong number of arguments\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

end