require './base'

class IllegalResp < Base

  def initialize
    super
    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (1..2).each{|i| strs << random_string}
      q = "*#{strs.length+1}\r\n$3\r\nset\r\n"
      strs.each{|s| q = "#{q}$#{s.length}\r\n#{s}\r\n"}
      queries << q
    end

    (0..NUMBER_OF_TEST_CASES).each do |i|
      str = random_string
      q = query_string(["get", str])
      queries << q
    end

    queries.each do |q|
      r = rand(1..q.length-1)
      q.slice!(r)
    end
  end

  def test
    check_protocol_err
  end

  def check_protocol_err
    queries.each do |q|
      expected = "-ERR Protocol error\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

end