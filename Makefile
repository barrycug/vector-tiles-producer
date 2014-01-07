PROTOBUF_CXXFLAGS=$(shell pkg-config protobuf --cflags)
PROTOBUF_LDFLAGS=$(shell pkg-config protobuf --libs-only-L) -lprotobuf-lite
MAPNIK_CXXFLAGS=$(shell mapnik-config --cflags) -Wsign-compare
MAPNIK_LDFLAGS=$(shell mapnik-config --libs --ldflags --dep-libs)
MAPNIK_VECTOR?='mapnik-vector-tile/src'

tileinfo:
	$(CXX) -o tileinfo tileinfo.cpp $(MAPNIK_VECTOR)/vector_tile.pb.cc -lprotobuf-lite -lz -I$(MAPNIK_VECTOR) 

create:
	$(CXX) -o create_tiles $(MAPNIK_VECTOR)/vector_tile.pb.cc create_tiles.cpp -I$(MAPNIK_VECTOR) $(MAPNIK_CXXFLAGS) $(MAPNIK_LDFLAGS) $(PROTOBUF_CXXFLAGS) $(PROTOBUF_LDFLAGS) -std=c++0x

clean:
	@rm -f ./tileinfo
	@rm -f ./create_tiles
