ReiTrei5
========

This is a ray tracer I built some time ago, and I'm currently refactoring it to support textures, meshes, soft shadows, and focal blur.

See the wiki at https://github.com/axnjaxn/ReiTrei5/wiki for documentation of the scene file format and command line arguments.

Compilation
-----------

There isn't an autoconf script yet. Someday! For now, call `make` to build ReiTrei, or `make run` to build and test using the tricol.ray file.

Usage
-----

On the console, type `./ReiTrei5 [OPTIONS] yourscene.ray` to parse and render a file called yourscene.ray.

See the [wiki page](https://github.com/axnjaxn/ReiTrei/wiki/Command-Line-Arguments) for a list of options.

Dependencies
------------

SDL 2.0
