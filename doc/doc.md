## Abstract

I'd like to detect specific movements in 2D and 3D space that are described as trajectories (called patterns).
Input trajectory (recorded movement) and pattern trajectory (description of a specific movement) are preprocessed and
compared by distance to each other. Dependent on the result, a function is called to process the result.

![system diagram](img/system-diagram.svg)

```c++
const auto result_stream =
  compare(preprocess(input_stream),
          preprocess(pattern_stream));

result_stream.subscribe([](auto &&result) {
    // process result
  });
```

## Terminology

### Trajectory

A trajectory is a curve defined by a finite sequence of points with linear interpolation between consecutive points.
I reuse the [Boost Geometry Linestring Concept].

![trajectory example](img/trajectory-L.svg)

```c++
trajectory<point> {
      {0, 2},
      {0, 0},
      {1, 0},
};
```

[Boost Geometry Linestring Concept]: http://www.boost.org/doc/libs/1_60_0/libs/geometry/doc/html/geometry/reference/concepts/concept_linestring.html


### Point

> A point is an entity that has a location in space or on a plane,
> but has no extent.  
> [see Wikipedia](http://en.wikipedia.org/wiki/Point_(geometry))

I focus on 2D and 3D points. I reuse the [Boost Geometry Point Concept].


```c++
point<CoordinateType, DimensionCount>
```

[Boost Geometry Point Concept]: http://www.boost.org/doc/libs/1_60_0/libs/geometry/doc/html/geometry/reference/concepts/concept_point.html


### Transformation

A transformation operation is a [Callable][Callable] with signature `Trajectory (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `translate_by(vector)` creates a transformation operation that translates a trajectory by `vector` and
returns the result.

[Callable]: http://en.cppreference.com/w/cpp/concept/Callable