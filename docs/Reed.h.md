
```
float fs = 44100;
float k = 1 / fs;
```

```
float H0 = 4e-4;        // Equilibrium position (m)
float Mr = 3.37e-6;     // Mass (kg)
float Sr = 1.46e-4;     // Surface area (m^2)
float omega0 = 23250;   // Resonant frequency (rad/s)
float sigma0 = 1500;    // Damping (s^-1)
float omega1 = 316;     // Collision stiffness
float alpha = 3;        // Non-linearity parameter
float w = 1e-3;         // Width of reed channel
```

```
float y = 0;   // Current state of the reed
float yp = 0;  // Previous state of the reed
```

```
float dtb()
```
#### return fs * (y - yp)


```
}
```
