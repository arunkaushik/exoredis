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
      queries << "*3\r\n$3\r\nset\r\n$#{str1.length}\r\n#{str1}\r\n$#{str2.length}\r\n#{str2}\r\n"
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
      query = "*2\r\n$3\r\nget\r\n$#{q.length}\r\n#{q}\r\n"
      expected = "$#{ans.length}\r\n#{ans}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def flushdb
    query = "*1\r\n$8\r\nflushall\r\n"
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
      query = "*2\r\n$3\r\nget\r\n$#{q.length}\r\n#{q}\r\n"
      expected = "$-1\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end