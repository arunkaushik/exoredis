require './base'

class GetTest < Base

  def initialize
    super

    (0..NUMBER_OF_TEST_CASES).each do |i|
      str1 = random_string
      str2 = random_string
      answers[i] = [str1, str2]
      queries << "*3\r\n$3\r\nset\r\n$#{str1.length}\r\n#{str1}\r\n$#{str2.length}\r\n#{str2}\r\n"
    end
  end

  def test
    sethash
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

  def checkhash
    (0..NUMBER_OF_TEST_CASES).each do |i|
      n = rand(0..99)
      q = answers[n].first
      ans = answers[n][1]
      query = "*2\r\n$3\r\nget\r\n$#{q.length}\r\n#{q}\r\n"
      expected = "$#{ans.length}\r\n#{ans}\r\n"
      send_message query
      msg = get_message
      assert(msg, expected, query)
    end
  end

end