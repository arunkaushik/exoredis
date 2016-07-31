require './base'
require 'json'

class SortedsetTest < Base

  def initialize
    super
    @answers = {}
    @rand_get = []
    (0..NUMBER_OF_TEST_CASES/10).each do |i|
      str = random_string
      answers[str] = {}
    end

    answers.keys.each do |set|
      (0..NUMBER_OF_TEST_CASES/2).each do |i|
        str = random_string
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
    zcard
    zcount
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

  def zaddxx
    answers.each do|set, data|
      (0..NUMBER_OF_TEST_CASES/10).each do |i|
        data.each do |key, score|
          key_exists = rand(0..1)
          key = key_exists ? key : random_string
          q = "*5\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$2\r\nxx\r\n$#{score.to_s.length}\r\n#{score.to_s}\r\n$#{key.length}\r\n#{key}\r\n"
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
          q = "*6\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$2\r\nxx\r\n$2\r\nch\r\n$#{score.to_s.length}\r\n#{score.to_s}\r\n$#{tar.length}\r\n#{tar}\r\n"
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
          q = "*5\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$2\r\nnx\r\n$#{score.to_s.length}\r\n#{score.to_s}\r\n$#{tar.length}\r\n#{tar}\r\n"
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
          q = "*6\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$2\r\nnx\r\n$2\r\nch\r\n$#{score.to_s.length}\r\n#{score.to_s}\r\n$#{tar.length}\r\n#{tar}\r\n"
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
        q = "*5\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$4\r\nincr\r\n$#{new_score.to_s.length}\r\n#{new_score.to_s}\r\n$#{key.length}\r\n#{key}\r\n"
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
        q = "*6\r\n$4\r\nzadd\r\n$#{set.length}\r\n#{set}\r\n$2\r\n#{switch}\r\n$4\r\nincr\r\n$#{new_score.to_s.length}\r\n#{new_score.to_s}\r\n$#{key.length}\r\n#{key}\r\n"
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

end