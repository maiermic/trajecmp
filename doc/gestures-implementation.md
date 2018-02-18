# Gestures - Implementation
This document describes two kinds of implementations:
The first is the [old/functional design](#old-design) that uses streams and
immutable data.
The [new/procedural design](#new-design) uses mutations
to reduce copying to a minimum, resulting in increased performance.
It is recommended to use the new design, but some examples are implemented in
the old design.

## Old Design
The old design uses [streams](terminology.md#stream) to define gestures.
You define the source of your input trajectory (`input_stream`) and your
pattern trajectory (`pattern_stream`).
Then you preprocess both of them to prepare them for comparison.
You compare the preprocessed trajectories and define what to do with the result.

```c++
const auto result_stream = compare(preprocess(input_stream),
                                   preprocess(pattern_stream));

result_stream.subscribe([](auto &&result) {
    // process result
});
```

`input_stream` and `pattern_stream` are streams of trajectories.
Every time a input trajectory is emitted on `input_stream` it is preprocessed and
compared with the (latest) preprocessed pattern trajectory of `pattern_stream`.
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

### Preprocessing
I already mentioned in the [introduction](../README.md) that most commonly
input and pattern trajectory differ in the number of points, length/size,
position, etc.
Distance functions may assume specific preconditions.
For example, some require that both trajectories have equal number of points or
length.
The goal of the preprocessing is to prepare a trajectory for comparison with
another trajectory.
The appropriate preprocessing steps depend on the same factors as
the limit that is used for [deciding similarity](#deciding-similarity)
including the preprocessing itself in that way that the preprocessing steps
influence each other.

The preprocessing is a sequence of filter and/or map operations.

![preprocessing diagram](img/preprocessing.png) 


#### Map (Transform)
Mapping is part of the preprocessing.
It performs a transformation operation that takes a trajectory as input and
returns another trajectory as output without changing the input trajectory.

A transformation operation is a [Callable][Callable] with signature
`Trajectory (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `translate_by(vector)` (see
[translate.hpp](../src/trajecmp/transform/translate.hpp#L45))
creates a transformation operation that translates a trajectory by `vector`
and returns the result.

Examples for commonly used transformations are:
Translation, scaling, rotation, curve smoothing and outlier elimination.

[Callable]: http://en.cppreference.com/w/cpp/concept/Callable


### Filter
Filtering is part of the preprocessing.
It filters (input) trajectories that do not meet the requirements of the
comparison or if the result is foreseeable without further processing.

![filter diagram](http://reactivex.io/documentation/operators/images/filter.png)

A filter operation needs a predicate, which is a [Callable] with signature
`bool (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `has_min_num_points(2)` (see
[has_min_num_points.hpp](../src/trajecmp/predicate/has_min_num_points.hpp#L7))
only lets trajectories pass that have at least two points.


## New Design
Immutable data makes it easier to reason about shared data between multiple
transformations (for multiple gestures).
But an immutable transformation that does not change its input returns a
modified copy as output.
This leads to a higher memory consumption and lower runtime performance due to
the additional copy instructions.
Chaining map- and filter-operations requires to store each intermediate result.
Sharing intermediate results between multiple transformations (for multiple
gestures) is harder to get right with streams because you have to consider time,
which is unnecessary, because the processing of input does normally not depend
on the previous trajectory in time.

### Filter
A function that usually takes a `const Trajectory&` and maybe some other
parameters and returns a boolean as output.

```c++
bool has_length_greater_than(int length, const Trajectory& trajectory);
```

It can be used in an `if`-statement to cancel preprocessing if the result is
foreseeable:

```c++
if (!has_length_greater_than(min_length, input)) {
    // cancel preprocessing because input trajectory is too short to match
    return;
}
// continue preprocessing
```

### Map
It is preferable to do a transformation in-place.
If it is possible, you can pass `Trajectory&` to the transformation function
and modify the trajectory directly.
For example, it is possible to translate each point of a trajectory by a vector
in-place:

```c++
void translate_by(const Vector &vector, Trajectory &trajectory);
```

If a transformation can not be done in-place you can fallback to the old design
and use a function with signature `Trajectory (const Trajectory &)`.

### Filter-Map
Sometimes it is advantageous (e. g. for performance reasons or encapsulation)
to combine filter and map in a single function.
`close_with_max_distance` (see [close.hpp](../src/trajecmp/transform/close.hpp))
closes a trajectory only if it is nearly closed (distance of start and end
point is less than a maximum distance (tolerance value)).

```c++
bool close_with_max_distance(Distance max_distance, Trajectory &trajectory);
```

It can be used in an `if`-statement to cancel preprocessing if the result is
foreseeable:

```c++
if (!close_with_max_distance(max_distance, input)) {
    // cancel preprocessing because input trajectory is not nearly closed
    return;
}
// continue preprocessing; input trajectory is closed (start- equals end-point)
```

Only if `true` is returned by `close_with_max_distance`, the transformation
(here closing) has been successfully applied.