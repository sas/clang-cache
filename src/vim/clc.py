from clc_if import clc_if
from clc_if import ttypes

import os
import sys
import vim
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TCompactProtocol

def clc_init():
    global transport
    global client

    path = '/run/user/%d/clang-cache/sock' % os.getuid()
    socket = TSocket.TSocket(unix_socket=path)
    transport = TTransport.TBufferedTransport(socket)
    protocol = TCompactProtocol.TCompactProtocol(transport)

    client = clc_if.Client(protocol)

    try:
        transport.open()
    except TTransport.TTransportException:
        return 0

    return 1

def clc_goto_definition():
    fname = vim.current.buffer.name
    line, col = vim.current.window.cursor

    c = ttypes.cursor(fname, line, col + 1)

    newc = client.find_definition_from_cursor(c)
    if newc.file == "" and newc.line == -1 and newc.column == -1:
        print >> sys.stderr, "Couldn't find definition of the cursor"

    if newc.file != vim.current.buffer.name:
        command = "edit %s" % newc.file
    else:
        command = "normal m"

    try:
        # For some unknown reason, whenever an exception occurs in
        # vim.command, vim goes crazy and output tons of useless python
        # errors, catch those.
        vim.command(command)
        vim.current.window.cursor = (newc.line, newc.column - 1)
    except:
        return
