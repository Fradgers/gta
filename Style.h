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




class Sprites {
public:
    Sprites()
    {
        texture_lookup = NULL;
    }

    void init( const Style& style )
    {
        if ( ! ( style.sprg && style.sprb && style.ppal && style.palx && style.palb ) )
            return;

        if ( texture_lookup ) { delete [] texture_lookup; texture_lookup = NULL; }
        texture_lookup = new Texture_Collection::Texture_Name[ style.sprb->end ];

        // for each sprite index in sprx
        for ( unsigned int sprite = style.sprb->car_sprite_base; sprite != style.sprb->end; ++sprite )
        {
            std::cout << " Sprite(" << sprite << ") " << std::flush;
            // use sprite index to get sprite data from sprg
            sprite_index& sprite_data = style.sprx->sprite_indices.at( sprite );

            // use ( sprite index + palb ) to get virtual palette index from palx
            // use virtual palette index to get physical palette index from ppal
            uint16_t physical_palette_index = style.palx->ppalette( sprite + style.palb->sprite_palette_base );

            {
                uint32_t* depaletted_sprite = new uint32_t[ sprite_data.w * sprite_data.h ];

               /// std::cout << std::endl;
                // for each pixel in sprite
                for ( int y = 0; y != sprite_data.h; ++y )
                {
                    for ( int x = 0; x != sprite_data.w; ++x )
                    {
                       /// std::cout << '.' << std::flush;
                        uint32_t pixel_location = sprite_data.ptr + ( y * 256 ) + x;
                        // replace pixel with corresponding pixel from physical palette
                        uint8_t pixel_value = style.sprg->at( pixel_location );
                       // std::cout << (int)pixel_value << std::endl;
                        depaletted_sprite[ ( y * sprite_data.w ) + x ] = style.ppal->color_at( physical_palette_index, pixel_value );
                    }
                   /// std::cout << std::endl;
                }

                // load the sprite into opengl and save the texture name
                texture_lookup[ sprite ] = textures.load_texture( depaletted_sprite, sprite_data.w, sprite_data.h );

                delete [] depaletted_sprite;
            }
        }
    }


  /*  void draw( GLfloat x, GLfloat y, GLfloat z )
    {
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texture_lookup[ 40 ] );

        glMatrixMode( GL_TEXTURE );
            glLoadIdentity();
        glMatrixMode( GL_MODELVIEW );

        glPushMatrix();
            // move the frame of reference to the x,y,z coords of the block being drawn
            glTranslatef( x, y, z );

            /// draw the textured quad
            glBegin( GL_QUADS );
                glTexCoord2f( 0,1 );
                glVertex3f( 0,1,0 );
                glTexCoord2f( 0,0 );
                glVertex3f( 0,0,0 );
                glTexCoord2f( 1,0 );
                glVertex3f( 1,0,0 );
                glTexCoord2f( 1,1 );
                glVertex3f( 1,1,0 );
            glEnd();
        glPopMatrix();

        glDisable( GL_TEXTURE_2D );
    }*/


    ~Sprites()
    {
        if ( texture_lookup ) delete [] texture_lookup;
    }

    Texture_Collection textures;
    Texture_Collection::Texture_Name* texture_lookup;
};

