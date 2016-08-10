require './base'

class SaveTest < Base

  def initialize
    super
    @sets = {}
    @answers = {}
    @rand_get = []
    (0..NUMBER_OF_TEST_CASES).each do |i|
      key = random_string
      val = random_string
      @sets[key] = val;
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
  end

  def test
    set
    zadd
  end

  def set
    @sets.each do |key, val|
      q = "*3\r\n$3\r\nset\r\n$#{key.length}\r\n#{key}\r\n$#{val.length}\r\n#{val}\r\n"
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
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

end

SaveTest.new().perform
