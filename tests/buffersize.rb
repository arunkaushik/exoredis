require './base'

class BufferSize < Base

  def initialize
    super
    @answers = {}
    @rand_get = []
    puts "Building long payloads, it may take some time..."
    (0..NUMBER_OF_TEST_CASES).each do |i|
      str1 = random_string
      str2 = (0..3500).map { ('a'..'z').to_a[rand(26)] }.join
      answers[str1] = str2
      @rand_get << str1
      queries << query_string(["set", str1, str2])
    end
  end

  def test
    settest
    gettest
  end

  def settest
    queries.each do |q|
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

  def gettest
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

end