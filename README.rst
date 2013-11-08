Vector tiles creator
====================


This is an experimental project to create vector tiles.


What does this do?
------------------

This should create vector tiles for a given area at chosen zoom levels.

Dependancies:
-------------

    mapnik

    mapnik-vector-tile


Install and compile
-------------------

Install Mapnik. <https://github.com/mapnik/mapnik>


Get mapnik-vector-tiles sources. <https://github.com/mapbox/mapnik-vector-tile>

Install the dependancies.

Change the path to the vector_tile.pb.cc file and to the mapnik-vector-tile sources directory (you can find them using the 'locate' command. If you do so, do not forget to updatedb before searching). Then:

::

    export MAPNIK_VECTOR=path/to/your/vector_tile.pb.cc
    export MAPNIK_VECTOR_PATH=path/to/your/mapnik-vector-tile-source-directory


::

    make create


to compile the project.


Use it!
-------

Chose a tile (for the whole world: z=0, x=0, y=0). This is the area for which you want to create subtiles until the maxz zoom level. Give then a path to your Mapnik stylesheet (xml). If you want compressed PBF tiles, add the --compress option, else you will get uncompressed PBF tiles. 

::

    ./create_tiles z maxz x y [--compress] path/to/stylesheet


To get informations about the created tiles, I have modified a bit the Mapnik-vector-tile's "tileinfo" example to read easily the informations of a vector tile.

::

    make tileinfo

