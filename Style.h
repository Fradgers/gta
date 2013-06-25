#ifndef _STYLE_H_
#define _STYLE_H_

#include "style_helpers.h"

class Style {
public:
    enum Chunk_Id {
        TILE = 1162627412,
        PPAL = 1279348816,
        PALB = 1112293712,
        PALX = 1481392464,
        SPRG = 1196576851,
        SPRX = 1481789523,
        SPRB = 1112690771,
        //FONB = 1112428358,
        //DELS = 1397507396,
        //DELX = 1481393476,
        CARI = 1230127427,
        OBJI = 1229603407,
        //RECY = 1497580882,
        SPEC = 1128616019
    };
    enum Endian_Check { GBST = 1414742599, TSBG = 1195529044 };

    Style( std::istream& stream )
    :   tile( NULL )
    {
        tile = NULL;
        ppal = NULL;
        palx = NULL;
        palb = NULL;

        sprb = NULL;
        sprx = NULL;
        sprg = NULL;
        cari = NULL;

        obji = NULL;

        binary_reader style_header_reader( stream, 6 );

        if ( style_header_reader.as<uint32_t>( 0 ) == GBST )
            std::cout << "<<GBST>> :: Endian correct" << std::endl;
        else if ( style_header_reader.as<uint32_t>( 0 ) == TSBG )
            std::cout << "<<GBST>> :: Endian inverse" << std::endl;

        while ( ! stream.eof() )
        {
            chunk_reader style_chunk_reader( stream );

            switch ( style_chunk_reader.chunk_type )
            {
                case TILE :
                {
                    std::cout << "<TILE>" << std::endl;
                    tile = new tile_reader( style_chunk_reader.body( stream ));
                    break;
                }
                case PALX :
                {
                    std::cout << "<PALX>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    palx = new palx_reader( body_stream );
                    break;
                }
                case PPAL :
                {
                    std::cout << "<PPAL>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    ppal = new ppal_reader( body_stream );
                    break;
                }
                case PALB :
                {
                    std::cout << "<PALB>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    palb = new palb_reader( body_stream );
                    break;
                }
                case SPRX :
                {
                    std::cout << "<SPRX>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    sprx = new sprx_reader( body_stream );
                    break;
                }
                case SPRB :
                {
                    std::cout << "<SPRB>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    sprb = new sprb_reader( body_stream );
                    break;
                }
                case SPRG :
                {
                    std::cout << "<SPRG>" << std::endl;
                    sprg = new sprg_reader( style_chunk_reader.body( stream ));
                    break;
                }
                case CARI :
                {
                    std::cout << "<CARI>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    cari = new cari_reader( body_stream );
                    break;
                }
                case OBJI :
                {
                    std::cout << "<OBJI>" << std::endl;
                    std::stringstream body_stream( style_chunk_reader.body( stream ));
                    obji = new obji_reader( body_stream );
                    break;
                }
                default :
                {
                    std::cout << "<OTHER> - " << style_chunk_reader.chunk_type << std::endl;
                    style_chunk_reader.body( stream );
                    break;
                }
            }
        }
    }

    ~Style()
    {
        if ( tile ) delete tile;
        if ( ppal ) delete ppal;
        if ( palx ) delete palx;
        if ( palb ) delete palb;
        if ( sprb ) delete sprb;
        if ( sprx ) delete sprx;
        if ( sprg ) delete sprg;
        if ( obji ) delete obji;
    }

    tile_reader* tile;
    ppal_reader* ppal;
    palx_reader* palx;
    palb_reader* palb;

    sprb_reader* sprb;
    sprx_reader* sprx;
    sprg_reader* sprg;
    cari_reader* cari;
    obji_reader* obji;

    //Vehicles vehicles;
    //Tiles tiles;
    //Sprites sprites;
};

#endif

