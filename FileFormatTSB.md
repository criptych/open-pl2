# TSB File Format #
TSB files contain skinned animation data.
Work on fully reversing this format is not complete, there are still one or two things that are a mystery.
All data is stored as little endian, uncompressed.



## pl2TSB ##
| **Offset** | **Type/Name**                                  | **Description** |
|:-----------|:-----------------------------------------------|:----------------|
| 00h      | `char Magic[4]`                          | Set to "TSB0" (0x30425354) |
| 04h      | `u32 Pad[3]`                             | Padding?? |
| 10h      | `u32 BoneCount`                          | Number of bones |
| 14h      | `u32 FrameCount`                         | Number of animation frames |
| 18h      | `u32 UnknownCount`                       | Unknown value (count?): always 0?) |
| 1Ch      | `u32 SpecialCount`                       | Special count |
| 20h      | `float Bones[FrameCount][BoneCount][16]` | Bunch of 4x4 matrices |
| +00h     | `pl2TSB_Special Specials[SpecialCount]`  | Special Sauce |
| +00h     | `char SpecialNames[SpecialCount][]`      | NUL Terminated Special Sauce names |

## pl2TSB\_Special ##
36 bytes per entry.