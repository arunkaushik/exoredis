require './base'
require './unkwn_command_test'
require './wrong_num_args_test'
require './protocol_err_test'
require './set_test'
require './get_test'
require './buffersize'
require './ping_test'
require './flushall_test'

UnknownCommand.new().perform
WrongNumberOfArguments.new().perform
IllegalResp.new().perform
SetTest.new().perform
GetTest.new().perform
BufferSize.new().perform
PingTest.new().perform
FlushallTest.new().perform