/*
    adapted from github.com/mapbox/mapnik-vector-tile
    license: https://github.com/mapbox/mapnik-vector-tile/blob/master/LICENSE
*/

#include <vector_tile.pb.h>
#include <vector_tile_compression.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char** argv){
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    std::vector<std::string> args;

    if (argc != 2) {
        std::clog << "please pass the path to an uncompressed or zlib-compressed protobuf tile\n";
        return EXIT_FAILURE;
    }

    try {
        std::string filename = argv[1];
        std::ifstream stream(filename.c_str(),std::ios_base::in|std::ios_base::binary);
        if (!stream.is_open()) {
            throw std::runtime_error("could not open: '" + filename + "'");
        }

        // we are using lite library, so we copy to a string instead of using ParseFromIstream
        std::string message(std::istreambuf_iterator<char>(stream.rdbuf()),(std::istreambuf_iterator<char>()));
        stream.close();

        // now attemp to open protobuf
        vector_tile::Tile tile;
        if (mapnik::vector_tile_impl::is_zlib_compressed(message)) {
            std::clog << "compressed protobuf\n";
            std::string uncompressed;
            mapnik::vector_tile_impl::zlib_decompress(message,uncompressed);
            if (!tile.ParseFromString(uncompressed)) {
                std::clog << "failed to parse compressed protobuf\n";
            }
        }
        else {
            std::clog << "uncompressed protobuf\n";
            if (!tile.ParseFromString(message)) {
                std::clog << "failed to parse protobuf\n";
            }
        }

        for (unsigned i=0;i<tile.layers_size();++i) {
            vector_tile::Tile_Layer const& layer = tile.layers(i);
            std::cout << "layer: " << layer.name() << "\n";
            std::cout << "  version: " << layer.version() << "\n";
            std::cout << "  extent: " << layer.extent() << "\n";
            std::cout << "  keys size: " << layer.keys_size() << "\n";
            std::cout << "  keys: ";
            for (unsigned i=0;i<layer.keys_size();++i) {
                    std::string const& key = layer.keys(i);
                    std::cout << key << ",";
            }
            std::cout << "\n";
            std::cout << "  values size: " << layer.values_size() << "\n";
            std::cout << "  values: ";
            for (unsigned i=0;i<layer.values_size();++i) {
                    vector_tile::Tile_Value const & value = layer.values(i);
                    if (value.has_string_value()) {
                        std::cout << "'" << value.string_value();
                    } else if (value.has_int_value()) {
                        std::cout << value.int_value();
                    } else if (value.has_double_value()) {
                        std::cout << value.double_value();
                    } else if (value.has_float_value()) {
                        std::cout << value.float_value();
                    } else if (value.has_bool_value()) {
                        std::cout << value.bool_value();
                    } else if (value.has_sint_value()) {
                        std::cout << value.sint_value();
                    } else if (value.has_uint_value()) {
                        std::cout << value.uint_value();
                    } else {
                        std::cout << "null";
                    }
                    if (i<layer.values_size()-1) {
                    std::cout << ",";
                    }
            }
            std::cout << "\n";
            std::cout << "  features size: " << layer.features_size() << "\n";
            for (unsigned i=0;i<layer.features_size();++i) {
                vector_tile::Tile_Feature const & feat = layer.features(i);
                std::cout << "  feature: " << feat.id() << " " << feat.type() << "\n";
                std::cout << "    tags: ";
                for (unsigned j=0;j<feat.tags_size();++j) {
                    uint32_t tag = feat.tags(j);
                    std::cout << tag;
                    if (j<feat.tags_size()-1) {
                        std::cout << ",";
                    }
                }
                std::cout << "\n";
                std::cout << "    geometries: ";
                for (unsigned j=0;j<feat.geometry_size();++j) {
                    uint32_t geom = feat.geometry(j);
                    std::cout << geom;
                    if (j<feat.geometry_size()-1) {
                    std::cout << ",";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }
    }
    catch (std::exception const& ex) {
        std::clog << "error: " << ex.what() << "\n";
        return EXIT_FAILURE;
    }
    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
}
