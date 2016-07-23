require './base'

class BitmapTest < Base

  def initialize
    super
    @answers = {}
    @key_pool = []
    (1..100).each do |i|
      str = random_string
      @key_pool << str
      @answers[str] = {}
    end
  end

  def test
    getbit
    setbit
    getbit
    setbit
    wrong_bit
    wrong_range
  end

  def getbit
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(0..4294967295)
      query = "*3\r\n$6\r\ngetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n"
      ans = @answers[str][pos]
      expected = ":#{ans.to_i}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def setbit
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(0..4294967295)
      bit = rand(0..1)
      query = "*4\r\n$6\r\nsetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n$1\r\n#{bit}\r\n"
      ans = @answers[str][pos]
      @answers[str][pos] = bit
      expected = ":#{ans.to_i}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def wrong_bit
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(0..4294967295)
      bit = rand(2..99998)
      query = "*4\r\n$6\r\nsetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n$#{bit.to_s.length}\r\n#{bit}\r\n"
      expected = "-ERR bit is not an integer or out of range\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def wrong_range
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(4294967296..5294967296)
      bit = rand(0..1)
      query = "*4\r\n$6\r\nsetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n$#{bit.to_s.length}\r\n#{bit}\r\n"
      expected = "-ERR bit offset is not an integer or out of range\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end
