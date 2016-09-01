require './base'

class FlushallTest < Base

  def initialize
    super
    @answers = {}
    @rand_get = []
    (0..NUMBER_OF_TEST_CASES).each do |i|
      str1 = random_string
      str2 = random_string
      answers[str1] = str2
      @rand_get << str1
      queries << query_string(["set", str1, str2])
    end
  end

  def test
    sethash
    checkhash
    flushdb
    checkhashnull
  end

  def sethash
    queries.each do|q|
      send_message q
      expected = "+OK\r\n"
      msg = get_message
      assert(msg, expected, q)
    end
  end

  def checkhash
    (0..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(0..@rand_get.length-1)
      q = @rand_get[n]
      ans = answers[q]
      query = query_string(["get", q])
      expected = "$#{ans.length}\r\n#{ans}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def flushdb
    query = query_string(["flushall"])
    expected = "+OK\r\n"
    send_message query
    msg = get_message
    assert(msg, expected, query)
  end

  def checkhashnull
    (0..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(0..@rand_get.length-1)
      q = @rand_get[n]
      ans = answers[q]
      query = query_string(["get", q])
      expected = "$-1\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end