# TMB File Format #
TMB files contain textures, materials, mesh and bones information.
Work on fully reversing this format is not complete, there are still one or two things that are a mystery.
All data is stored as little endian, uncompressed.



## Layout Overview ##
| **Offset** | **Type**                    | **Description** |
|:-----------|:----------------------------|:----------------|
| 00h      | `char[4]`             | Magic "TMB0" ( 0x30424d54)|
| +00h     | `u32`                 | Texture count |
| +04h     | `pl2TMB_Texture[]`    | Bunch of textures |
| +00h     | `u32`                 | Material count |
| +04h     | `pl2TMB_Material[]`   | Bunch of materials |
| +00h     | `u32`                 | Mesh count |
| +04h     | `pl2TMB_Mesh[]`       | Bunch of meshes |
| +00h     | `u32`                 | Bone count |
| +04h     | `float[][16]`         | Bunch of matrices (4x4) |
| +00h     | `u32`                 | Unknown count |
| +04h     | `pl2TMB_Unknown[]`    | Bunch of unknown things |
| +04h     | `char[]`              | Bunch of nul terminated strings |



## pl2TMB\_Texture ##
All textures are 32BPP RGBA textures, and always seem to have power-of-two dimensions.

| **Offset** | **Type**         | **Description** |
|:-----------|:-----------------|:----------------|
| 00h      | `char[32]` | Texture name (Include the extension of the source file) |
| 20h      | `u16`      | Width in pixels |
| 24h      | `u16`      | Height in pixels |
| 26h      | `u8[][4]`  | Width\*Height pixels |



## pl2TMB\_Material ##
The materials just carry the various colors used for lighting as well as a texture index.
Currently the use of the colors is guessed, but seem to be working okay.

| **Offset** | **Type**         | **Description** |
|:-----------|:-----------------|:----------------|
| 00h      | `float[4]` | Ambient color <sup>???</sup> |
| 10h      | `float[4]` | Diffuse color <sup>???</sup> |
| 20h      | `float[4]` | Specular color <sup>???</sup> |
| 30h      | `float[4]` | Emission color <sup>???</sup> |
| 40h      | `float`    | Shininess <sup>???</sup> |
| 44h      | `u32`      | Texture ID (Points to one of the textures above) |

## pl2TMB\_Mesh ##
The unknown field seems to be either 0 or 1....

| **Offset** | **Type**                | **Description** |
|:-----------|:------------------------|:----------------|
| 00h      | `char[32]`        | Mesh name |
| 20h      | `float[16]`       | Cooked matrix |
| 60h      | `u32`             | Triangle count |
| 64h      | `u32`             | Unknown! <sup>???</sup>  |
| 68h      | `u32`             | Material bind count |
| 6Ch      | `pl2TMB_Bind[]`   | Material bindings |
| +00h     | `pl2TMB_Vertex[]` | Vertex data (Triangle count\*3) |

### pl2TMB\_Bind ###

| **Offset** | **Type** | **Description** |
|:-----------|:---------|:----------------|
| 00h      | `u32`    | Material ID (Points to one of the materials above) |
| 04h      | `u32`    | Start vertex (Index of the first vertex using this material)   |
| 08h      | `u32`    | Triangle count (Number of triangle following start vertex that use this material) |

### pl2TMB\_Vertex ###
An unused bone always has ID 0xff.
Weights should always add up to 1.0 (100%)

| **Offset** | **Type**         | **Description** |
|:-----------|:-----------------|:----------------|
| 00h      | `float[3]` | Vertex position |
| 0Ch      | `float[3]` | Weights of each bone |
| 18h      | `u8[4]`    | Bone IDs (forth ID isn't used) |
| 1Ch      | `float3[]` | Vertex normal |
| 28h      | `u8[4]`    | Vertex color <sup>???</sup> |
| 2Ch      | `float[2]` | Texture coordinate |

### pl2TMB\_Unknown ###
Not sure what this is, something to do with the strings that follow after. Each entry is 40 bytes.