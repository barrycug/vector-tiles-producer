PROTOBUF_CXXFLAGS=$(shell pkg-config protobuf --cflags)
PROTOBUF_LDFLAGS=$(shell pkg-config protobuf --libs-only-L) -lprotobuf-lite
MAPNIK_CXXFLAGS=$(shell mapnik-config --cflags) -Wsign-compare
MAPNIK_LDFLAGS=$(shell mapnik-config --libs --ldflags --dep-libs)
MAPNIK_VECTOR=path/to/your/vector_tile.pb.cc
MAPNIK_VECTOR_PATH=path/to/your/mapnik-vector-tile/src

tileinfo: 
	$(CXX) -o tileinfo tileinfo.cpp $(MAPNIK_VECTOR) -lprotobuf-lite -lz -I$(MAPNIK_VECTOR_PATH) 

create: 
	$(CXX) -o create_tiles $(MAPNIK_VECTOR) create_tiles.cpp -I$(MAPNIK_VECTOR_PATH) $(MAPNIK_CXXFLAGS) $(MAPNIK_LDFLAGS) $(PROTOBUF_CXXFLAGS) $(PROTOBUF_LDFLAGS) -std=c++0x

clean:
	@rm -f ./tileinfo
	@rm -f ./create_tiles
