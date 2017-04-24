## Abstract

I'd like to detect specific movements in 2D and 3D space that are described as trajectories (called patterns).
Input trajectory (recorded movement) and pattern trajectory (description of a specific movement) are preprocessed and
compared by distance to each other. Dependent on the result, a function is called to process the result.

![system diagram](img/system-diagram.svg)

```c++
const auto result_stream = compare(preprocess(input_stream),
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

[Boost Geometry Point Concept]: http://www.boost.org/doc/libs/1_60_0/libs/geometry/doc/html/geometry/reference/concepts/concept_point.html


### Stream

I use the term _stream_ for _values distributed in time_,
which are represented by the [ReactiveX Observable][ReactiveX Intro] model implemented by [RxCpp].  

[ReactiveX Intro]: http://reactivex.io/intro.html
[RxCpp]: https://github.com/Reactive-Extensions/RxCpp


## System

![system diagram](img/system-diagram.svg)

It looks like this in C++

```c++
const auto result_stream = compare(preprocess(input_stream),
                                   preprocess(pattern_stream));

result_stream.subscribe([](auto &&result) {
    // process result
});
```

You define the source of your input trajectory (`input_stream`) and your pattern trajectory (`pattern_stream`).
Then you preprocess both of them to prepare them for comparison.
You compare the preprocessed trajectories and define what to do with the result.

`input_stream` and `pattern_stream` are streams of trajectories.
Every time a input trajectory is emitted it is compared with the (latest) pattern trajectory.
Usually, `pattern_stream` contains only one (static pattern) trajectory.
However, a pattern trajectory might be dynamically created.
For example, you could use another input trajectory as a pattern.

You can compare the input trajectory with multiple patterns like this:

```c++
const auto preprocessed_input_stream = preprocess(input_stream);

const auto result_1_stream = compare(preprocessed_input_stream,
                                     preprocess(pattern_1_stream));

const auto result_2_stream = compare(preprocessed_input_stream,
                                     preprocess(pattern_2_stream));

result_1_stream.subscribe([](auto &&result) {
    // process result
});

result_2_stream.subscribe([](auto &&result) {
    // process result
});
```



### Transformation

A transformation operation is a [Callable][Callable] with signature `Trajectory (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `translate_by(vector)` creates a transformation operation that translates a trajectory by `vector` and
returns the result.

[Callable]: http://en.cppreference.com/w/cpp/concept/Callable