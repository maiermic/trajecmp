# System

![system diagram](img/system-diagram.png)

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


## Preprocessing

The preprocessing is a sequence of filter and/or map operations.

![preprocessing diagram](img/preprocessing.png)


### Map (Transform)

Mapping is part of the preprocessing.
It performs a transformation operation that takes a trajectory as input and returns another trajectory as output
without changing the input trajectory.

A transformation operation is a [Callable][Callable] with signature `Trajectory (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `translate_by(vector)` creates a transformation operation that translates a trajectory by `vector` and
returns the result.

[Callable]: http://en.cppreference.com/w/cpp/concept/Callable


### Filter

Filtering is part of the preprocessing.
It filters (input) trajectories that do not meet the requirements of the comparison or
if the result is foreseeable without further processing.

![filter diagram](http://reactivex.io/documentation/operators/images/filter.png)

A filter operation needs a predicate, which is a [Callable] with signature `bool (const Trajectory &)`.
It might be a functor that depends on input arguments.
For example, `has_min_num_points(2)` only lets trajectories pass that have at least two points.
