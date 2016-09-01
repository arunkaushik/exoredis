require './base'

class SaveTest < Base

  def initialize
    super
    @hashmap = {}
    @answers = {}
    @rand_get = []
    (0..NUMBER_OF_TEST_CASES).each do |i|
      key = random_string
      val = random_string
      @hashmap[key] = val;
    end


    (0..5).each do |i|
      str = random_string(20)
      answers[str] = {}
    end

    answers.keys.each do |set|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        str = random_string(20)
        score = rand(1.2...7632778.9).round(6)
        score = -1 * score if rand(0..1)
        answers[set][str] = score
      end
    end

    @key_pool = []
    @bitmap = {}
    (1..3).each do |i|
      str = random_string
      @key_pool << str
      @bitmap[str] = {}
    end
  end

  def test
    set
    zadd
    setbit
    savedb
    flushdb
    loaddb
    checkget
    checkgetbit
    zcard
    zcount
  end

  def set
    @hashmap.each do |key, val|
      q = query_string(["set", key, val])
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end

    # save a key to expire early
    q = query_string(["set", "special_key", "bar", "px", 1000])
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

  def checkget
    (0..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(0..@hashmap.keys.length-1)
      q = @hashmap.keys[n]
      ans = @hashmap[q]
      query = query_string(["get", q])
      expected = "$#{ans.length}\r\n#{ans}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end

    print("Checking expired key")
    q =  query_string(["get", "special_key"])
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

  def zadd
    answers.each do|set, data|
      data.each do |key, score|
        q = query_string(["zadd", set, score, key])
        expected = ":1\r\n"
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def zcard
    answers.each do |set, data|
      q = query_string(["zcard", set])
      expected = ":#{data.keys.length}\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

  def zcount
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES).each do |i|
        min = -1 * rand(1.2...78778.9).round(6)
        max = rand(1.2...7678778.9).round(6)
        q = query_string(["zcount", set, min, max])
        res = answers[set].values.select{|k| k if k >= min && k <= max}.length
        expected = ":#{res.to_s}\r\n"
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def setbit
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(0..4294967295)
      bit = rand(0..1)
      query = query_string(["setbit", str, pos, bit])
      ans = @bitmap[str][pos]
      @bitmap[str][pos] = bit
      expected = ":#{ans.to_i}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def checkgetbit
    (1..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(1..@key_pool.length-1)
      str = @key_pool[n]
      pos = rand(0..4294967295)
      query = query_string(["getbit", str, pos])
      ans = @bitmap[str][pos]
      expected = ":#{ans.to_i}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end