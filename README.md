Floor Music Visualiser
======================

A simple music visualiser using magnum and libvisual.

[!FMV Screenshot with bars](/screenshot.png?raw=true)

The visualizer just uses [libvisual's](http://libvisual.org) mplayer plugin to visualise the current mplayer stream.

Run mplayer with `mplayer my-audio-file -af export` to get a visualisation.

Depends on [magnum](https://github.com/mosra/magnum) for rendering.
