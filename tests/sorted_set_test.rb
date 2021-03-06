require './base'
require 'json'

class SortedsetTest < Base

  def initialize
    super
    @answers = {}
    @rand_get = []
    (0..4).each do |i|
      str = random_string(20)
      answers[str] = {}
    end

    answers.keys.each do |set|
      (0..NUMBER_OF_TEST_CASES/500).each do |i|
        str = random_string(20)
        score = rand(1.2...76778.9).round(6)
        score = -1 * score if rand(0..1)
        answers[set][str] = score
      end
    end
  end

  def test
    zadd
    zcard
    zcount
    zaddxx
    zaddnx
    zaddxxch
    zaddnxch
    zaddincr
    zaddxxnxincr
    zrange
    zcard
    zcount
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

      (0..10).each do |i|
        min = "-inf"
        max = "+inf"
        q = query_string(["zcount", set, min, max])
        res = answers[set].values.length
        expected = ":#{res.to_s}\r\n"
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def zaddxx
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        data.each do |key, score|
          key_exists = rand(0..1)
          key = key_exists ? key : random_string
          q = query_string(["zadd", set, "xx", score, key])
          expected = ":0\r\n"
          send_message q
          msg = get_message
          assert(msg, expected, q)
        end
      end
    end
  end

  def zaddxxch
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        data.each do |key, score|
          key_exists = rand(0..1)
          str = random_string
          tar = key_exists ? key : str
          verdict = data[tar]
          q = query_string(["zadd", set, "xx", "ch", score, tar])
          expected = verdict ? ":1\r\n" : ":0\r\n"
          answers[set][tar] = score
          send_message q
          msg = get_message
          assert(msg, expected, q)
        end
      end
    end
  end

  def zaddnx
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        data.each do |key, score|
          key_exists = rand(0..1)
          str = random_string
          tar = key_exists ? key : str
          verdict = data[tar]
          q = query_string(["zadd", set, "nx", score, tar])
          expected = verdict ? ":0\r\n" : ":1\r\n"
          answers[set][tar] = score
          send_message q
          msg = get_message
          assert(msg, expected, q)
        end
      end
    end
  end

  def zaddnxch
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        data.each do |key, score|
          key_exists = rand(0..1)
          str = random_string
          tar = key_exists ? key : str
          verdict = data[tar]
          q = query_string(["zadd", set, "nx", "ch", score, tar])
          expected = verdict ? ":0\r\n" : ":1\r\n"
          answers[set][tar] = score
          send_message q
          msg = get_message
          assert(msg, expected, q)
        end
      end
    end
  end

  def zaddincr
    answers.each do|set, data|
      data.each do |key, score|
        prev_score = score
        new_score = rand(1.2...76778.9).round(6)
        final_score = (prev_score + new_score).round(6)
        fin_scr = final_score.to_s + "0"*(6 - final_score.to_s.split('.').last.length)
        q = query_string(["zadd", set, "incr", new_score, key])
        expected = "$#{fin_scr.length}\r\n#{fin_scr}\r\n"
        answers[set][key] = final_score
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def zaddxxnxincr
    answers.each do|set, data|
      data.each do |key, score|
        switch = rand(0..1) == 0 ? "xx" : "nx"
        prev_score = score
        new_score = rand(1.2...76778.9).round(6)
        final_score = (prev_score + new_score).round(6)
        fin_scr = final_score.to_s + "0"*(6 - final_score.to_s.split('.').last.length)
        q = query_string(["zadd", set, switch, "incr", new_score, key])
        if switch == "xx"
          expected = "$#{fin_scr.length}\r\n#{fin_scr}\r\n"
          answers[set][key] = final_score
        else
          expected = "$-1\r\n"
        end
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end

  def zrange
    mems = []
    scores = []
    answers.each do|set, data|
      data.each do |key, score|
        left = rand(0..data.size)
        left = left * -1 if rand(0..1)
        right = rand(0..data.size*2)
        right = right * -1 if rand(0..1)
        withscore = rand(0..1)
        q = ["zrange", set, left, right]
        q << "withscores" if withscore
        q = query_string(q)
        arr = answers[set].to_a.dup
        arr.sort!{|x, y| x.last <=> y.last}
        mems = arr.map{|i| i[0]}
        scores = arr.map{|i| i[1]}
        expected = "*"
        if(mems[left..right])
          exp_len = withscore ? mems[left..right].length*2 : mems[left..right].length
          expected = expected + "#{exp_len}\r\n"
          arr[left..right].each_with_index do |e|
            expected = expected + "$#{e[0].length}\r\n#{e[0]}\r\n"
            final_score = e[1]
            fin_scr = final_score.to_s + "0"*(6 - final_score.to_s.split('.').last.length)
            expected = expected + "$#{fin_scr.length}\r\n#{fin_scr}\r\n" if withscore
          end
        else
            expected = expected + "0\r\n"
        end
        send_message q
        msg = get_message
        assert(msg, expected, q)
      end
    end
  end
end
