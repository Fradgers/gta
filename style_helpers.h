#include "Texture_Collection.h"
#include <sstream>

class tile_reader {
public:
    tile_reader( const std::string& raw_pixel_data  )
    :   color_index_data( raw_pixel_data )
    { ; }

    std::stringstream color_index_data;
};


class palb_reader {
public:
    palb_reader( std::istream& stream )
    {
        tile_palette_base = 0;
        sprite_palette_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + tile_palette_base;
    }

    uint16_t tile_palette_base;
    uint16_t sprite_palette_base;
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

                // debug code !!! REMOVE ME !!!
                //if ( col == 0 ) ppalettes[col][pal] = 0xFFFFFFFF;
            }
    }

    uint32_t color_at( int ppalette_index, uint8_t color_index )
    {
        return ppalettes[ color_index ][ ppalette_index ];
    }

    uint32_t ppalettes[ 256 ][ 64 ];
};

#include <iomanip>


class sprb_reader {
public:
    sprb_reader( std::istream& stream )
    {
        // file holds sprite count for each category, here we convert to a sprite offset for each category
        car_sprite_base = 0;
        ped_sprite_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + car_sprite_base;
        code_obj_sprite_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + ped_sprite_base;
        map_obj_sprite_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + code_obj_sprite_base;
        user_sprite_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + map_obj_sprite_base;
        font_sprite_base = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + user_sprite_base;
        end = binary_reader( stream, 2 ).as<uint16_t>( 0 ) + font_sprite_base;
    }

    uint16_t car_sprite_base;
    uint16_t ped_sprite_base;
    uint16_t code_obj_sprite_base;
    uint16_t map_obj_sprite_base;
    uint16_t user_sprite_base;
    uint16_t font_sprite_base;
    uint16_t end;
};


class Car {
public:
    Car( std::istream& stream )
    {
        binary_reader car_data( stream, 14 );

        model = car_data.as<uint8_t>( 0 );
        sprite = car_data.as<uint8_t>( 1 );
        w = car_data.as<uint8_t>( 2 );
        h = car_data.as<uint8_t>( 3 );
        num_remaps = car_data.as<uint8_t>( 4 );

        binary_reader( stream, num_remaps ); // ignore remaps
        num_doors = binary_reader( stream, 1 ).as<uint8_t>( 0 );
        binary_reader( stream, num_doors*2 ); // ignore doors
    }

    void print()
    {
        std::cout << "Model(" << (int)model << ") ";
        std::cout << "Sprite(" << (int)sprite << ") ";
        std::cout << "w(" << (int)w << ") ";
        std::cout << "h(" << (int)h << ") ";
        std::cout << "Remaps(" << (int)num_remaps << ") ";
        std::cout << "Doors(" << (int)num_doors << ") ";
    }

    uint8_t model;
    uint8_t sprite;
    uint8_t w;
    uint8_t h;
    uint8_t num_remaps;
    uint8_t num_doors;
};

class cari_reader {
public:
    cari_reader( std::istream& stream )
    {
        uint8_t sprite_cumulative = 0;
        while ( stream )
        {
            Car car( stream );

            // the sprite value in the file is the number of sprites used by the car
            // we need the actual sprite index used by the car, given by sprite_cumulative
            sprite_cumulative += car.sprite;
            car.sprite = sprite_cumulative;

            cars.push_back( car );
            //std::cout << "car " << cars.size() << ": ";
            //car.print();
            //std::cout << std::endl;
        }
    }

    std::vector<Car> cars;
};

class Vehicles {
public:
    void combine( cari_reader* cari, sprb_reader* sprb )
    {
        for ( unsigned int car = 0; car != cari->cars.size(); ++car )
        {
            cari->cars[ car ].sprite += sprb->car_sprite_base;
        }
    }
};

class sprite_index {
public:
    sprite_index( std::istream& stream )
    {
        binary_reader pointer_data( stream, 4 );
        ptr = pointer_data.as<uint32_t>( 0 );

        binary_reader dimension_data( stream, 4 );
        w = dimension_data.as<uint8_t>( 0 );
        h = dimension_data.as<uint8_t>( 1 );
        // bytes 6 and 7 ignored (padding)
    }

    void print()
    {
        std::cout << "Width(" << (int)w << ") ";
        std::cout << "Height(" << (int)h << ") ";
    }

    uint32_t ptr;
    uint8_t w;
    uint8_t h;
};

class sprx_reader {
public:
    sprx_reader( std::istream& stream )
    {
        while ( stream )
        {
            sprite_index index( stream );

            sprite_indices.push_back( index );
        }
    }

    std::vector<sprite_index> sprite_indices;
};

class sprg_reader {
public:
    sprg_reader( const std::string& raw_pixel_data )
    :   indexed_pixels( raw_pixel_data.begin(), raw_pixel_data.end() )
    { std::cout << "Pixel count: " << indexed_pixels.size() << std::endl; }

    //uint8_t& at( uint32_t origin, uint8_t dx, uint8_t dy ) { return indexed_pixels[ origin + ( dy * 256 ) + dx ]; }
    uint8_t at( uint32_t location ) { return indexed_pixels[ location ]; }

    std::vector<uint8_t> indexed_pixels;
};

class object_data {
public:
    uint8_t model_number;
    uint8_t sprite_count;
};

class obji_reader {
public:
    obji_reader( std::istream& stream )
    {
        object_data obj;

        stream >> obj.model_number >> obj.sprite_count;

        objects.push_back( obj );
    }

    std::vector<object_data> objects;
};
