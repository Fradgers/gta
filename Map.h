#ifndef _MAP_H_
#define _MAP_H_

#include "map_helpers.h"

class Map {
public:
    enum Chunk_Id {
        CMAP = 1346456899,
        DMAP = 1346456900,
        ZONE = 1162760026,
        EDIT = 1414087749,
        LGHT = 1414022988,
        RGEN = 1313163090,
        ANIM = 1296649793,
        PSXM = 1297634128
    };
    enum Endian_Check { GBMP = 1347240519, PMBG = 1195527504 };

    Map( std::istream& stream )
    :   dmap( NULL )
    {
        binary_reader map_header_reader( stream, 6 );

        if ( map_header_reader.as<uint32_t>( 0 ) == GBMP )
            std::cout << "<<GBMP>> :: Endian correct" << std::endl;
        else if ( map_header_reader.as<uint32_t>( 0 ) == PMBG )
            std::cout << "<<GBMP>> :: Endian inverse" << std::endl;

        while ( ! stream.eof() )
        {
            chunk_reader map_chunk_reader( stream );

            switch ( map_chunk_reader.chunk_type )
            {
                case DMAP :
                {
                    std::cout << "<DMAP>" << std::endl;
                    std::stringstream body_stream( map_chunk_reader.body( stream ));
                    dmap = new dmap_reader( body_stream );
                    break;
                }
                case CMAP :
                case ZONE :
                case PSXM :
                case ANIM :
                {
                    std::cout << "<ANIM>" << std::endl;
                    std::stringstream body_stream( map_chunk_reader.body( stream ));
                    anim = new anim_reader( body_stream );
                    break;
                }
                case LGHT :
                case EDIT :
                case RGEN :
                {
                    map_chunk_reader.body( stream );
                    std::cout << "<OTHER>" << std::endl;
                    break;
                }
                default :
                {
                    std::cout << "<OTHER> - " << map_chunk_reader.chunk_type << std::endl;
                    map_chunk_reader.body( stream );
                    break;
                }
            }
        }
    }

    ~Map()
    {
        if ( dmap ) delete dmap;
        if ( anim ) delete anim;
    }

    dmap_reader* dmap;
    anim_reader* anim;
};

#endif
