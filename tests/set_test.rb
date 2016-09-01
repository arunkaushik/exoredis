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
    q = query_string(["set"])
    expected = "-ERR wrong number of arguments\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo"])
    expected = "-ERR wrong number of arguments\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "exx"])
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "pxx"])
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "ex", "3e"])
    expected = "-ERR value is not an integer or out of range\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "ex", "3", "px", "3e"])
    expected = "-ERR value is not an integer or out of range\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "ex", 1, "px", 3])
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "bar", "ex", 1, "px", 1])
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["get", "foo"])
    expected = "$3\r\nbar\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    #let it expire
    print("Cheking expiry with sleep, hold on")
    sleep(1.5)

    q = query_string(["get", "foo"])
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

  def set_with_nx_xx
    flushdb
    q = query_string(["set", "foo", "bar", "nx"])
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["get", "foo"])
    expected = "$3\r\nbar\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "baz", "nx"])
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "foo", "baz", "xx"])
    expected = "+OK\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "loo", "baz", "xx"])
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "loo", "baz", "xx", "nx"])
    expected = "$-1\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)

    q = query_string(["set", "loo", "bar", "ex", 1, "px", 1, "nxx"])
    expected = "-ERR syntax error\r\n"
    send_message q
    msg = get_message
    assert(msg, expected, q)
  end

end