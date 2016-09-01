require './base'

class DelTest < Base

  def initialize
    super
    @answers = {}
    @rand_get = []
    (0..NUMBER_OF_TEST_CASES*2).each do |i|
      str1 = random_string
      str2 = random_string
      answers[str1] = str2
      @rand_get << str1
      queries << query_string(["set", str1, str2])
    end
  end

  def test
    sethash
    delcheck
    checkhash
  end

  def sethash
    queries.each do|q|
      send_message q
      expected = "+OK\r\n"
      msg = get_message
      assert(msg, expected, q)
    end
  end

  def delcheck
    (0..NUMBER_OF_TEST_CASES/2).each do |i|
      strs = []
      ans = 0;
      (0..NUMBER_OF_TEST_CASES/200).each do |l|
        n = rand(0..@rand_get.length-1)
        q = @rand_get[n]
        ans += 1 if answers[q]
        answers.delete(q)
        strs << q
      end

      query = "*#{strs.length + 1}\r\n$3\r\ndel\r\n"

      strs.each do |s|
        query = query + "$#{s.length}\r\n#{s}\r\n"
      end

      expected = ":#{ans}\r\n"

      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

  def checkhash
    (0..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(0..@rand_get.length-1)
      q = @rand_get[n]
      ans = answers[q]
      query = query_string(["get", q])
      if ans
        expected = "$#{ans.length}\r\n#{ans}\r\n"
      else
        expected = "$-1\r\n"
      end
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end