require 'socket'

class Base

  NUMBER_OF_TEST_CASES = 500
  PORT = 15000
  URL = "localhost"
  attr_accessor :conn, :queries, :answers

  def initialize
    @queries = []
    @answers = {}
    @assertions = 0
    @success = 0
    @failure = 0
    @fail_cases = []
    @conn = TCPSocket.open(URL, PORT)
    @conn.setsockopt(Socket::SOL_SOCKET, Socket::SO_KEEPALIVE, true)
  end

  def perform
    puts "#{self.class.to_s} --- running tests..."
    flushdb
    test
    flushdb
    puts
    puts "#{@assertions} assertions | #{@success} successes | #{@failure} failures"
    print_fail_cases if @fail_cases.any?
    @conn.close
    @conn = nil
  end

  def send_message message
    raise "Not connected" unless @conn
    @conn.write (message)
  end

  def print_fail_cases
    puts "Printing first five failed cases"
    @fail_cases[0..5].each do |cas|
      puts "-"*50
      puts "Test for: #{cas[:test_for]} | query: #{cas[:query].inspect} | expected: #{cas[:expected].inspect} | Got: #{cas[:response].inspect}"
      puts "-"*50
    end
  end

  def get_message
    raise "Not connected" unless @conn
    message = ''
    message = @conn.recvmsg
    message
  end

  def random_string len = 50
    (0...rand(10..len)).map { ('a'..'z').to_a[rand(26)] }.join
  end

  def flushdb
    query = "*1\r\n$8\r\nflushall\r\n"
    expected = "+OK\r\n"
    send_message query
    msg = get_message
    assert(msg, expected, query)
  end

  def loaddb
    query = "*1\r\n$6\r\nloaddb\r\n"
    expected = "+OK\r\n"
    send_message query
    msg = get_message
    assert(msg, expected, query)
  end

  def savedb
    query = "*1\r\n$4\r\nsave\r\n"
    expected = "+OK\r\n"
    send_message query
    msg = get_message
    assert(msg, expected, query)
  end

  def assert msg, expected, query
    @assertions += 1
    if msg.first == expected
      @success += 1
      print "\e[32m.\e[0m"
    else
      @fail_cases << {
        :test_for => self.class.to_s,
        :query => query,
        :expected => expected.inspect,
        :response => msg.first.inspect
      }
      @failure += 1
      print "\e[31mx\e[0m"
    end
  end
end