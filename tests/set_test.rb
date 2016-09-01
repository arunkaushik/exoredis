require './base'

class SetTest < Base

  def initialize
    super
    (0..NUMBER_OF_TEST_CASES).each do |i|
      strs = []
      (1..2).each{|i| strs << random_string}
      q = "*#{strs.length+1}\r\n$3\r\nset\r\n"
      strs.each{|s| q = "#{q}$#{s.length}\r\n#{s}\r\n"}
      queries << q
    end
  end

  def test
    check_set
    set_with_wrong_args
    set_with_nx_xx
  end

  def check_set
    queries.each do |q|
      expected = "+OK\r\n"
      send_message q
      msg = get_message
      assert(msg, expected, q)
    end
  end

  def set_with_wrong_args
    q = "*1\r\n$3\r\nset\r\n"
    expected = "-ERR wrong number of arguments\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*2\r\n$3\r\nset\r\n$3\r\nfoo\r\n"
    expected = "-ERR wrong number of arguments\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*4\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$3\r\nexx\r\n"
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*4\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$3\r\npxx\r\n"
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*5\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nex\r\n$2\r\n3e\r\n"
    expected = "-ERR value is not an integer or out of range\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*7\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nex\r\n$2\r\n10\r\n$2\r\npx\r\n$2\r\n3e\r\n"
    expected = "-ERR value is not an integer or out of range\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*7\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nex\r\n$2\r\n10\r\n$2\r\npx\r\n$2\r\n10\r\n"
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*7\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nex\r\n$1\r\n1\r\n$2\r\npx\r\n$1\r\n1\r\n"
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*2\r\n$3\r\nget\r\n$3\r\nfoo\r\n"
    expected = "$3\r\nbar\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    #let it expire
    print("Cheking expiry with sleep, hold on")
    sleep(1.5)

    q = "*2\r\n$3\r\nget\r\n$3\r\nfoo\r\n"
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

  def set_with_nx_xx
    flushdb

    q = "*4\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nnx\r\n"
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*2\r\n$3\r\nget\r\n$3\r\nfoo\r\n"
    expected = "$3\r\nbar\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*4\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbaz\r\n$2\r\nnx\r\n"
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*4\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbaz\r\n$2\r\nxx\r\n"
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*4\r\n$3\r\nset\r\n$3\r\nloo\r\n$3\r\nbaz\r\n$2\r\nxx\r\n"
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*5\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbaz\r\n$2\r\nxx\r\n$2\r\nnx\r\n"
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = "*8\r\n$3\r\nset\r\n$3\r\nfoo\r\n$3\r\nbar\r\n$2\r\nex\r\n$1\r\n1\r\n$2\r\npx\r\n$1\r\n1\r\n$3\r\nnxx\r\n"
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

end