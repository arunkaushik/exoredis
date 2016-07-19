require './base'

class SetTest < Base

  def initialize
    super
    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (3..10).each{|i| strs << random_string}
      strs << "arun" if strs.length % 2 == 1
      q = "*#{strs.length+1}\r\n$3\r\nset\r\n"
      strs.each{|s| q = "#{q}$#{s.length}\r\n#{s}\r\n"}
      queries << q
    end
  end

  def test
    check_set
  end

  def check_set
    queries.each do |q|
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end
end