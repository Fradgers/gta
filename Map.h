
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

class Style;
#include <vector>
#include "Vec3.h"
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

    void draw( const std::vector<block_binary_info>& block_info, const Style& style, Vec3& coords );

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


    /// change draw order to spiral around viewer
    void draw( const Style& style, const Vec3& camera )
    {
        for ( unsigned int z = 0; z != 7; ++z )
        {
            /// 0 to 256
            for ( int x = 0; x != 256; ++x )
            {
                for ( unsigned int y = 0; y != 256; ++y )
                {
                    uint32_t column_index = base[ y*256 + x ];

                    // map is defined bottom-to-top
                    Vec3 coords = Vec3( x, 255-y, z );

                    if ( column_index < columns_data_size_bytes )
                    {
                        glPushMatrix();
                            columns.at( column_index ).draw( blocks, style, coords );
                        glPopMatrix();
                    }
                    else
                        std::cout << "(" << x << "," << y << ") column index: " << column_index << "/" << columns.size() << std::endl;
                } // y
            } // x
        } // z
    }

    std::vector<int32_t> base;
    uint32_t columns_data_size_bytes;
    std::map<uint32_t, dmap_column_binary_info> columns;
    uint32_t num_blocks;
    std::vector<block_binary_info> blocks;
};
