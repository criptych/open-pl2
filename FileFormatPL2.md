# PL2 File Format #
PL2 files are just pak files, they contain all the other files that PL2 uses.
The pak file format is simple, but unfortunately uses an odd compression system. At the moment this is unknown (although a tool called MltPL2.exe can decompress these files).

## Layout Overview ##
| **Offset** | **Type**                    | **Description** |
|:-----------|:----------------------------|:----------------|
| 00h      | `u32[4]`              | Always 0's? What is this for? |
| 10h      | `pl2PL2_Entry[]`      | Entries in the pak file |


## pl2PL2\_Entry ##
| **Offset** | **Type**         | **Description** |
|:-----------|:-----------------|:----------------|
| 00h      | `char[32]` | Name of the entry |
| 20h      | `u32`      | Offset in bytes to start of file |
| 24h      | `u32`      | Size of packed data in bytes |
| 28h      | `u32`      | Size of unpacked data in bytes |
| 2Ch      | `u32`      | Padding <sup>???</sup> |

## Compression Format ##
The data is not stored raw, even when uncompressed.
I am relatively sure the compression system is based on LZ77 (Sliding Window) compression.
Here's a rough overview of what I know so far.

There is a flag byte followed by eight data elements. A data element is either an uncompressed byte or a two byte compression pointer. Then after this another flag byte, etc, etc.

A binary 1 in the flag byte corresponds to an uncompressed byte, a binary 0 in the flag byte corresponds to a lz77 pair (this is two bytes)

|**Binary**      | **Size**   |**Description**|
|:---------------|:-----------|:--------------|
|`01111111`| `9`  |Seven uncompressed bytes, one lz77 pair |
|`11001111`| `10` |Four uncompressed bytes, two lz77 pairs, two uncompressed bytes.  |

Of course, the format of the LZ77 pair is crucial to decompressing the data. It seems to be in 12/4 format. As in 12bits for the offset and 4bits for the length.

`OOOOOOO OOOOLLLL` (Where O is offset bit, L is length bit)

If we add three to the length we can "decompress" (with trash data) to the correct file size. Why three? Well it'd be stupid to point to two bytes of data with a two byte pointer, so three bytes is the minimum you'd want to point to, therefor you might as well make 0=3.

I can not relate the offset to data being referenced yet though. But we're getting closer!

A pair set entirely to 0s (O=0, L=0) seems to terminate the stream.