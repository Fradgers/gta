#include "Texture_Collection.h"

class tile_reader {
public:
    tile_reader( const std::string& raw_pixel_data  )
    :   color_index_data( raw_pixel_data )
    { ; }

    std::stringstream color_index_data;
};

class palx_reader {
public:
    palx_reader( std::istream& stream )
    {
        for ( int i = 0; i != 16384; ++i )
        {
            ppalette_lookup[ i ] = binary_reader( stream, 2 ).as<uint16_t>( 0 );
        }
    }

    uint16_t ppalette( uint16_t vpalette_index )
    {
        return ppalette_lookup[ vpalette_index ];
    }

    uint16_t ppalette_lookup[ 16384 ];
};

class ppal_reader {
public:
    ppal_reader( std::istream& stream )
    {
        for ( int col = 0; col != 256; ++col )
            for ( int pal = 0; pal != 64; ++pal )
            {
                // the alpha channel is not used by the game
                // for all palettes, colour 0 is Black with 0x00 alpha value
                // for all other colours, alpha value is set to 0xFF

                uint8_t alpha_value = ( col == 0 ) ? 0x00 : 0xFF;

                binary_reader pixel( stream, 4 );
                ppalettes[ col ][ pal ] = (( pixel.data[0]&0xFF) << 24 ) + ( ( pixel.data[1]&0xFF) << 16 ) + ( ( pixel.data[2]&0xFF) << 8 ) + ( alpha_value );
            }
    }

    uint32_t color_at( int ppalette_index, uint8_t color_index )
    {
        return ppalettes[ color_index ][ ppalette_index ];
    }

    uint32_t ppalettes[ 256 ][ 64 ];
};

#include <iomanip>

class Tiles {
public:
    Tiles() { depaletted_pixels = new uint32_t[ 4*64 * 4*64 * 62 ]; }

    void combine( tile_reader* tile, ppal_reader* ppal, palx_reader* palx )
    {
        // for each row of textures
        for ( size_t texture_row = 0; texture_row != 4 * 62; ++texture_row )
        {
            // read 4 textures
            for ( unsigned int pixel_row = 0; pixel_row != 64; ++pixel_row )
            {
                for ( unsigned int column = 0; column != 4*64; ++column )
                {
                    int texture_column = column / 64;
                    int pixel_column = column - ( texture_column * 64 );

                    int texture_idx = texture_row * 4 + texture_column;
                    int pixel_idx = pixel_row * 64 + pixel_column;

                    uint8_t pixel = binary_reader( tile->color_index_data, 1 ).as<uint8_t>( 0 );
                    depaletted_pixels[ (texture_idx*64*64) + pixel_idx ] = ppal->color_at( palx->ppalette( texture_idx ), pixel );
                }
            }

            // after reading the 4 textures in the row of textures,
            // load the 4 textures into opengl and save their texture names
            for ( size_t texture_column = 0; texture_column != 4; ++texture_column )
            {
                int texture_idx = texture_row * 4 + texture_column;
                texture_lookup[ texture_idx ] = textures.load_texture( depaletted_pixels + ( texture_idx*64*64 ));
            }
        }
    }

    ~Tiles() { delete [] depaletted_pixels; }

    Texture_Collection textures;
    Texture_Collection::Texture_Name texture_lookup[992];
    uint32_t* depaletted_pixels;
};
