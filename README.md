### Huffman (Work in progress)
###### 

Nice to have / Future or TODOs:
- improve bitarray classe removing the dipendence from std::vector and use a custom one or a simple block of memory (i need to access the underlying capacity), the memory could be aligned but with posix_memalign or similar to exploit avx etc
- HuffmanCode buffer could live into the stack aligned memory chunk since it's at most uint8_t[256] this affect memory because mostly are smaller but can produce a better performance, using a custom allocator is also a solution but introduce more complexity
- reconstruct a tree from symbol table path stored in hafe file (the hashmap solution for decoding require a bit of overhead but still constant), since different huffman impl. can produce differents huffman codes if you want store tree there are 2 possibility: describe how to store tree into the format specs. or reconstruct from the symbol table.
- HuffmanNode doesn't need to keep frequency, frequency could be stored into a different structure this reduce memory consumpion but introduce more complexity (IMHO it's bad)


```bash
# compress
./huffman < ../data/divina_commedia.txt > divina_commedia.txt.hafe

# decompress
./huffman - < divina_commedia.txt.hafe > divina_commedia.txt

# checksum
md5sum divina_commedia.txt ../data/divina_commedia.txt
```

Example of format

All integers in **Little Endian**:


| Offset   | Size       |  Name              | Example                                         | Description |
-----------|------------|--------------------|-------------------------------------------------|-------------|
| 0        | 4 byte     | magic              | 4D 61 6E 75                                     | magic number |
| 4        | 16 byte    | reserved           | 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D 1D | reserved bytes |
| 20       | 4 byte    | symbol table length | 3C 00 00 00                                     | length in bytes of symbol table 32 bit unsigned integer in little endian |
| 24       | variable  | symbol table        | -                                               | see [here](#symbol-table-on-disk) |
| variable | 8 byte    | bitstream length    | -                                               | the length of bitstream **in bits** except padding bits if none, 64 bit unsigned integer in little endian ~~uint8_t last_byte_garbage_bits stream_size % 2**8, a compact way to express bitstream length since can be very long and it contains garbage at the end in the last byte (padding bits).~~ |
| variable | variable  | bitstream           | -                                               | the compressed bitstream |


#### Symbol Table On Disk
The symbol table is stored as contiguos block of variable size every block, each block is a prefix table entry that describe a symbol and his prefix code
empty entries are not allowed.

| uint8_t | uint8_t                                                                           | uint8_t[]                     |
----------|------------------------------------------------------------------------------------|-------------------------------
| symbol  | the length > 0 of prefix code **in bits** excluding padding, this value cannot be 0 | prefix code (at least 1 byte) |


Example, suppose you have to store a 3 bit prefix code `101` entry for the symbol `'c'` you will write:

- 1 byte (the symbol `'c'`)
- 1 byte the number of bits that compose the prefix code (3)
- 1 byte (the prefix code) `101`+ 5 bit of garbage

To read this prefix code the decompressor will read `(bits / 8.0).ceil()` bytes and will consider only the first 3 bits.

You may also find helping the following C-Like pseudo-code:

```c
struct {
  uint8_t symbol;
  uint8_t prefix_len_in_bits = 1; // at least 1 bit, no empty entries allowed
  uint8_t[??] prefix_code;         // unknow but at least 1 element
};

uint8_t magic[4] = { 0x4d, 0x61, 0x6e, 0x75 };
uint8_t reserved[16];
uint32_t sym_table_length;
sym_table_entry_t[??] sym_table;

uint64_t bitstream_length;
uint8_t[??] bitstream;
```



- Maybe a crc32 of uncompressed data will be added later there is some space reserved.
- The length of every symbol table entry must be > 0.


##### Learning resources
- https://huffman-coding-online.vercel.app
- https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1234/resources/huffman.html
- https://www.math.unipd.it/~baldan/Alg2/Slide/Lezione2.pdf (italian)
- https://www.youtube.com/playlist?list=PLU4IQLU9e_OrY8oASHx0u3IXAL9TOdidm

##### Special Thanks

A special thanks to [Bill Bird](https://github.com/billbird) from the University of Victoria (Canada), 
who has shared on YouTube his wonderful video playlist on compression algorithms.
I loved the clarity and detail and the passion of his explanations. 

Thanks, Bill.


###### Copyright © 2025, [Manu-sh](https://github.com/Manu-sh), s3gmentationfault@gmail.com. Released under the [MIT license](LICENSE).
