
#include "binary_reader.h"
#include "OpenGL.h"

enum Orientation { zero_degrees = 0, ninety_degrees, one_eighty_degrees, two_seventy_degrees };
enum Block_Face { Rear = 0, Right, Front, Left, Lid };


class block_binary_info {
public:
    enum Face_Lighting { None = 0, Low, Medium, High };

    block_binary_info( std::istream& stream )
    {
        binary_reader block_reader( stream, 12 );
        faces[Left] = block_reader.as<uint16_t>( 0 ); // [2b]
        faces[Right] = block_reader.as<uint16_t>( 2 ); // [2b]
        faces[Rear] = block_reader.as<uint16_t>( 4 ); // [2b]
        faces[Front] = block_reader.as<uint16_t>( 6 ); // [2b]
        faces[Lid] = block_reader.as<uint16_t>( 8 ); // [2b]

        arrows = block_reader.as<uint8_t>( 10 ); // [1b]
        slope_type = block_reader.as<uint8_t>( 11 ); // [1b]
    }

    uint16_t face_texture( const Block_Face& face ) const
    {
        return faces[ face ] & 0x3FF;
    }

    Orientation face_orientation( const Block_Face& face ) const
    {
        switch (( faces[ face ] >> 14 ) & 0x3 )
        {
            case 0x0 : return zero_degrees;
            case 0x1 : return ninety_degrees;
            case 0x2 : return one_eighty_degrees;
            case 0x3 : return two_seventy_degrees;
            default : return zero_degrees;
        }
    }

    bool face_xflip( const Block_Face& face ) const
    {
       return (( faces[ face ] >> 13 ) & 0x1 ) == 1;
    }

    Face_Lighting face_ambient_light( const Block_Face& face ) const
    {
        if ( face != Lid ) return Medium;

        switch (( faces[ face ] >> 10 ) & 0x3 )
        {
           case 0x0 : return None;
           case 0x1 : return Low;
           case 0x2 : return Medium;
           case 0x3 : return High;
           default : return Medium;
        }
    }

    uint16_t faces[5]; // rear, right, front, left, lid
    uint8_t arrows;
    uint8_t slope_type;
};

#include <vector>
class dmap_column_binary_info {
public:
    dmap_column_binary_info( std::istream& stream )
    {
        binary_reader reader( stream, 4 );
        height = reader.as<uint8_t>( 0 ); // [1b]
        offset = reader.as<uint8_t>( 1 ); // [1b]
        padding = reader.as<uint16_t>( 2 ); // [2b]

        try {
            blockd.resize( height - offset );
        } catch ( ... ) {
            std::cerr << stream.tellg() << std::endl;
            throw;
        }

        for ( size_t block = 0; block != blockd.size(); ++block )
        {
            blockd[ block ] = binary_reader( stream, 4 ).as<uint32_t>( 0 ); // [4b] * ( height - offset )
        }
    }



    uint8_t height;
    uint8_t offset;
    uint16_t padding;
    std::vector<uint32_t> blockd;
};


#include <map>
#include <sstream>
#include <iostream>
//#include "grid_iterators.h"
class dmap_reader {
public:
    dmap_reader( std::istream& stream )
    {
        binary_reader base_reader( stream, 256*256*4 );
        base.resize( 256*256 );
        for ( size_t tile = 0; tile != base.size(); ++tile )
        {
            base[ tile ] = base_reader.as<uint32_t>( tile * 4 );
        }

        columns_data_size_bytes = binary_reader( stream, 4 ).as<uint32_t>( 0 );
        columns_data_size_bytes *= 4; /// 669472

            binary_reader columns_reader( stream, columns_data_size_bytes );

            std::stringstream columns_data( columns_reader.as_string(), std::ios::in | std::ios::binary );
            while ( columns_data.tellg() < columns_data_size_bytes )
            {
                uint32_t current_column_offset = columns_data.tellg() / 4;
                //cout << current_column_offset << endl;
                columns.insert( std::make_pair( current_column_offset, dmap_column_binary_info( columns_data )));
            }

        num_blocks = binary_reader( stream, 4 ).as<uint32_t>( 0 );

        for ( size_t i = 0; i != num_blocks; ++i )
        {
            blocks.push_back( block_binary_info( stream ));
        }
    }

    std::vector<int32_t> base;
    uint32_t columns_data_size_bytes;
    std::map<uint32_t, dmap_column_binary_info> columns;
    uint32_t num_blocks;
    std::vector<block_binary_info> blocks;
};


class anim_data {
public:
    anim_data( std::istream& stream )
    {
        binary_reader anim( stream, 6 );

        base_tile = anim.as<uint16_t>( 0 );
        game_cycles_per_frame = anim.as<uint8_t>( 2 );
        repeats = anim.as<uint8_t>( 3 );
        frame_count = anim.as<uint8_t>( 4 );

        for ( unsigned int fr = 0; fr != frame_count; ++fr )
            frames.push_back( binary_reader( stream, 2 ).as<uint16_t>( 0 ));
    }

    uint16_t base_tile;
    uint8_t game_cycles_per_frame;
    uint8_t repeats;
    uint8_t frame_count;

    std::vector<uint16_t> frames;
};


class anim_reader {
public:
    anim_reader( std::istream& stream )
    {
        while ( stream )
            animations.push_back( anim_data( stream ));
    }

    std::vector<anim_data> animations;
};


class mobj_data {
public:
    uint16_t x;
    uint16_t y;
    uint8_t rotation;
    uint8_t object_type;
};


class mobj_reader {
public:
    mobj_reader( std::istream& stream )
    {
        mobj_data data;

        stream >> data.x >> data.y >> data.rotation >> data.object_type;

        map_objects.push_back( data );
    }

    std::vector<mobj_data> map_objects;
};
