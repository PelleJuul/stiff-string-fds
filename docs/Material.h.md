
The material struct provides an easy way to create material parameter
presets, which can be used with 1D and 2D models.
```
struct Material
```
```
float E = 200e6;    // Young's modulus, Pa (N / m^2) (steel)
float p = 8000;    // Density, kg / m^3 (steel) was (8000)
```
describe the material and shape of the domain.

#### static Material createSteelMaterial()


Creates a material with the parameters of steel.

#### static Material createAirMaterial()


Create a material with the parameters of air.
