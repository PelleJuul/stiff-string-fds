
```
#define pow2(x) (x * x)
```

```
#define pos(x) (x > 0 ? x : 0)
#define neg(x) (x < 0 ? x : 0)
```

```
template <typename T> int sgn(T val) {
```
#### return (T(0) < val) - (val < T(0))


```
}
```

```
static float mapRange(float x, float oldMin, float oldMax, float newMin, float newMax)
```
#### float r = (x - oldMin) / (oldMax - oldMin)


#### return newMin + r * (newMax - newMin)


```
}
