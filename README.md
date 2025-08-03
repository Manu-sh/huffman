### Huffman
###### 

Example of format

All integers in little endian:

`uint8_t magic[] { 0x4d, 0x61, 0x6e, 0x75 };` 

`uint8_t reserved[16]`

`uint32_t length` of symbol table

| uint8_t | uint16_t   | uint8_t[]   |
----------|------------|--------------
| symbol  | length > 0 | prefix code |

`uint64_t bitstream_length` ~~uint8_t last_byte_garbage_bits stream_size % 2**8, a compact way to express bitstream length since can be very long and it contains garbage at the end in the last byte (padding bits).~~

`uint8_t[] bitstream..`

- Maybe a crc32 of uncompressed data will be added later there is some space reserved.
- The length of every symbol table entry must be > 0.



##### Learning resources
- https://huffman-coding-online.vercel.app
- https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1234/resources/huffman.html
- https://www.math.unipd.it/~baldan/Alg2/Slide/Lezione2.pdf (italian)
- https://www.youtube.com/playlist?list=PLU4IQLU9e_OpnkbCS_to64F_vw5yyg4HB

##### Special Thanks

A special thanks to [Bill Bird](https://github.com/billbird) from the University of Victoria (Canada), 
who has shared on YouTube his wonderful video playlist on compression algorithms.
I loved the clarity and detail and the passion of his explanations. 

Thanks, Bill.


###### Copyright © 2025, [Manu-sh](https://github.com/Manu-sh), s3gmentationfault@gmail.com. Released under the [MIT license](LICENSE).
