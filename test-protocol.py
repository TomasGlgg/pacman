from pwn import *
from struct import unpack

context.log_level = 'debug'

io = remote('0', 4444)


name = b'python-client'
MAGIC = p32(0xabcdfe01)
CONNECT_PTYPE = p32(0x01)
namelen = p32(len(name))
first = MAGIC + CONNECT_PTYPE + namelen + name
io.send(first)

res = io.read(3*4)
magic, ptype, size = unpack('<III', res)
log.info('Magic: 0x%x, ptype: 0x%x, size: %d (0x%x)', magic, ptype, size, size)
io.read(size)

READY_TYPE = p32(0x02)
second = MAGIC + READY_TYPE + p32(0)
io.send(second)

res = io.read(3*4)
magic, ptype, size = unpack('<III', res)

frame_timeout, player_count = unpack('<II', io.read(2*4))
log.info('Frame timeout: %d, player count: %d', frame_timeout, player_count)
for _ in range(player_count):
    start_x, start_y, start_direction, player_name_len = unpack('<IIII', io.read(4*4))
    name = io.read(player_name_len)
    log.info('Start x: %d, start y: %d, start direction: %d, player name length: %d, name: %s', start_x, start_y, start_direction, player_name_len, name)


io.interactive()
