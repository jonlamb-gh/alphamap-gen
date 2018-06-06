# alphamap-gen
Generate RGBA alpha map png file from a heightmap png

## TODO

- better color mapping and blending
- per channel constraints/offsets
- add some read-png() logic to `libattopng` and remove `lodepng`

## Dependencies

- libpopt: `sudo apt-get install libpopt-dev`

## Building

```bash
git submodule update --init

mkdir build

cd build/

cmake ..

make install
```

## Usage

```bash
Usage: alphamap-gen [OPTION...]
  -v, --verbose                Enable verbose output
  -i, --input-file=<path>      Input file path (default: "heightmap.png")
  -o, --output-file=<path>     Output file path (default: "alphamap.png")
      --blend-overlap=0:85     Blending overlap value (default: 10)
      --invert                 Invert heightmap values

Help options:
  -?, --help                   Show this help message
      --usage                  Display brief usage message
```

## Example

Using example image found online:

![heightmap](res/heightmap.png)

```bash
alphamap-gen -i res/heightmap.png -o alphamap.png --invert
```

Produces:

![alphamap](res/alphamap.png)
