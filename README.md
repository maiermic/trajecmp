# Trajectory Comparison

The project is called _trajecmp_ which is a abbreviation of _trajectory comparison_.

## Abstract

I'd like to detect specific movements in 2D and 3D space that are described as trajectories (called patterns).
Input trajectory (recorded movement) and pattern trajectory (description of a specific movement) are preprocessed and
compared by distance to each other. Dependent on the result, a function is called to process the result.

![system diagram](doc/img/system-diagram.png)

```c++
const auto result_stream = compare(preprocess(input_stream),
                                   preprocess(pattern_stream));

result_stream.subscribe([](auto &&result) {
    // process result
});
```

## Documentation

- [Terminology](doc/terminology.md)
- [System Description](doc/system.md)
- [How To Build](doc/how-to-build.md)
