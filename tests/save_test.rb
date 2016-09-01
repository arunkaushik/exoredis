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
      q = "*3\r\n$3\r\nset\r\n$#{key.length}\r\n#{key}\r\n$#{val.length}\r\n#{val}\r\n"
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end

    # save a key to expire early
    q = "*5\r\n$3\r\nset\r\n$11\r\nspecial_key\r\n$3\r\nbar\r\n$2\r\npx\r\n$4\r\n1000\r\n"
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
      query = "*2\r\n$3\r\nget\r\n$#{q.length}\r\n#{q}\r\n"
      expected = "$#{ans.length}\r\n#{ans}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end

    print("Checking expired key")
    q = "*2\r\n$3\r\nget\r\n$11\r\nspecial_key\r\n"
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

  def zadd
    answers.each do|set, data|
      data.each do |key, score|
        q = "*4\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$#{score.to_s.length}\r\n#{score.to_s}\r\n$#{key.length}\r\n#{key}\r\n"
        expected = ":1\r\n"
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def zcard
    answers.each do |set, data|
      q = "*2\r\n$5\r\nzcard\r\n$#{set.length}\r\n#{set}\r\n" 
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
        q = "*4\r\n$6\r\nzcount\r\n$#{set.length}\r\n#{set}\r\n$#{min.to_s.length}\r\n#{min.to_s}\r\n$#{max.to_s.length}\r\n#{max.to_s}\r\n"
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
      query = "*4\r\n$6\r\nsetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n$1\r\n#{bit}\r\n"
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
      query = "*3\r\n$6\r\ngetbit\r\n$#{str.length}\r\n#{str}\r\n$#{pos.to_s.length}\r\n#{pos}\r\n"
      ans = @bitmap[str][pos]
      expected = ":#{ans.to_i}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end