#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include <vector>

// mapnik
#include <mapnik/map.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/value_error.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/box2d.hpp>
#include <mapnik/proj_transform.hpp>
#include <mapnik/image_util.hpp>

// mapnik vector tiles generation
#include <vector_tile.pb.h>
#include <vector_tile_projection.hpp>
#include <vector_tile_processor.hpp>
#include <vector_tile_backend_pbf.hpp>
#include <vector_tile_datasource.hpp>
#include <vector_tile_util.hpp>
#include <vector_tile_compression.hpp>

using namespace std;
using namespace mapnik;

string style_path;
bool compression = false;

void create_tiles(int z, int maxz, int x, int y) ;
void create_single_tile(int z, int x, int y) ;
void create_path(int z, int x) ;

int main (int argc, char* argv[]) {
    for (int i=0 ; i<argc ; i++){
        if (compression)
            argv[i - 1] = argv[i];
        if (strncmp(argv[i], "--compress", 10) == 0)
            compression = true;
    }

    if (argc != 6 && argc != 7) {
        clog << "command: " << argv[0] << "[--compress] minz maz x y path/to/stylesheet.xml\n";
        clog << "    x and y are the values at the minz zoom_level\n";
        return EXIT_FAILURE;
    }

    int minz = atoi(argv[1]);
    int maxz = atoi(argv[2]);
    int x = atoi(argv[3]);
    int y = atoi(argv[4]);
    style_path = argv[5];

    //add plugins to be able to load the shapefile (and also other formats) in load_map()
    datasource_cache::instance().register_datasources("/usr/lib/mapnik/input/");
    
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    create_path(minz, x);
    create_tiles(maxz, minz, x, y);
    cout << "Tiles created.\n";

    google::protobuf::ShutdownProtobufLibrary();

    return EXIT_SUCCESS;
}

void create_path(int z, int x) {
    string pathname = to_string(z) + "/" + to_string(x);
    clog << "Making path " << pathname << "\n";
    boost::filesystem::path path(pathname);
    boost::filesystem::create_directories(pathname);
}

void create_tiles(int maxz, int z, int x, int y) {
    //create the current tile
    create_single_tile(z, x, y);
 
    if (z+1 > maxz) { return; }

    create_path(z+1, 2*x);
    create_path(z+1, 2*x+1);

    //recursively create the four subtiles (at the next zoom level) of this tile
    create_tiles(maxz, z+1, 2*x,   2*y  );
    create_tiles(maxz, z+1, 2*x+1, 2*y  );
    create_tiles(maxz, z+1, 2*x,   2*y+1);
    create_tiles(maxz, z+1, 2*x+1, 2*y+1);
}

void create_single_tile(int z, int x, int y) {
    // initialize map
    int tile_size = 256;
    Map map(tile_size, tile_size);
    load_map(map, style_path, false);
    
    // bounding box
    mapnik::vector::spherical_mercator merc(tile_size);
    double minx, miny, maxx, maxy;
    merc.xyz(x, y, z, minx, miny, maxx, maxy);

    box2d<double> bbox;
    bbox.init(minx, miny, maxx, maxy);

    map.zoom_to_box(bbox);

    mapnik::vector::tile tile;
    mapnik::vector::backend_pbf backend(tile, 16);
    request m_req(map.width(), map.height(), map.get_current_extent());
    mapnik::vector::processor<mapnik::vector::backend_pbf> ren(backend, map, m_req);

    ren.apply();

    string buffer, compressed;
    tile.SerializeToString(&buffer);
    if (compression) {
        mapnik::vector::compress(buffer, compressed);        
        buffer = compressed;
        cout << "Creating compressed tile for z = " << z << " ; x = " << x << " ; y = " << y <<  " ; stylesheet: " << style_path << "\n";
    } else {
        cout << "Creating tile for z = " << z << " ; x = " << x << " ; y = " << y <<  " ; stylesheet: " << style_path << "\n";
    }
    ofstream output(to_string(z) + "/" + to_string(x) + "/" + to_string(y) + ".pbf");
    output << buffer ;
}
