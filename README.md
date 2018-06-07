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
alphamap-gen --help

Usage: alphamap-gen [OPTION...]
  -v, --verbose                Enable verbose output
  -i, --input-file=<path>      Input file path (default: "heightmap.png")
  -o, --output-file=<path>     Output file path (default: "alphamap.png")
      --invert                 Invert heightmap values
  -b, --blur                   Apply a blur filter
      --blur-factor=f > 0      Blurring factor (default: 0.111111)
      --blend-overlap=0:85     Blending overlap value (default: 10)
      --red-offset=0:255       Blending red offset value (default: 25)
      --red-min=0:255          Blending red min value (default: 100)
      --red-max=0:255          Blending red max value (default: 255)
      --green-offset=0:255     Blending green offset value (default: 25)
      --green-min=0:255        Blending green min value (default: 100)
      --green-max=0:255        Blending green max value (default: 255)
      --blue-offset=0:255      Blending blue offset value (default: 25)
      --blue-min=0:255         Blending blue min value (default: 100)
      --blue-max=0:255         Blending blue max value (default: 255)

Help options:
  -?, --help                   Show this help message
      --usage                  Display brief usage message
```

## Example

Using example image found online:

![heightmap](res/heightmap.png)

```bash
alphamap-gen -i res/heightmap.png -o alphamap.png --invert --blur
```

Produces:

![alphamap](res/alphamap.png)
