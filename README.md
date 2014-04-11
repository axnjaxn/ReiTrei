ReiTrei
=======

ReiTrei v1.1b1

ReiTrei is a ray tracer, a program for rendering 3D scenes by simulating the path light takes as it enters a camera at different angles. ReiTrei has a number of interesting features: soft shadows, focal blur, and its own scene description language.

See the wiki at https://github.com/axnjaxn/ReiTrei/wiki for documentation of the scene file format and command line arguments.

Compilation
-----------

There isn't an autoconf script yet. Someday! For now, call `make` to build ReiTrei, or `make run` to build and test using the tricol.ray file.

Usage
-----

On the console, type `./ReiTrei [OPTIONS] yourscene.ray` to parse and render a file called yourscene.ray.

See the [wiki page](https://github.com/axnjaxn/ReiTrei/wiki/Command-Line-Arguments) for a list of options.

Dependencies
------------

SDL 2.0
libMagick++ (optional)