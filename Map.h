
#include "binary_reader.h"
#include "OpenGL.h"

    enum Shape {
        /// standard cube
        Default = 0, // 0
        /// 26 slopes
        North26_1, North26_2, South26_1, South26_2, West26_1, West26_2, East26_1, East25_2, // 1-8
        /// 7 slopes
        North7_1, North7_2, North7_3, North7_4, North7_5, North7_6, North7_7, North7_8, // 9-16
        South7_1, South7_2, South7_3, South7_4, South7_5, South7_6, South7_7, South7_8, // 17-24
        West7_1, West7_2, West7_3, West7_4, West7_5, West7_6, West7_7, West7_8, // 25-32
        East7_1, East7_2, East7_3, East7_4, East7_5, East7_6, East7_7, East7_8, // 33-40
        /// 45 slopes
        North45, South45, West45, East45, // 41-44
        /// diagonals
        Diagonal_NW, Diagonal_NE, Diagonal_SW, Diagonal_SE, // 45-48
        /// diagonal slopes
        Diagonal_Slope_NW, Diagonal_Slope_NE, Diagonal_Slope_SW, Diagonal_Slope_SE, // 49-52
        /// partial blocks
        Partial_W, Partial_E, Partial_N, Partial_S,
        Partial_NW, Partial_NE, Partial_SE, Partial_SW, // 53-60
        Partial_Centre, // 61
        /// internal use
        Reserved, Under_Slope // 62-63
    };

    enum Orientation { zero_degrees = 0, ninety_degrees, one_eighty_degrees, two_seventy_degrees };

class block_binary_info {
public:
    enum Block_Face { Left = 0, Right, Rear, Front, Lid };


    enum Face_Lighting { None = 0, Low, Medium, High };

    block_binary_info( std::istream& stream )
    {
        binary_reader block_reader( stream, 12 );
        faces[Right] = block_reader.as<uint16_t>( 0 ); // [2b]
        faces[Left] = block_reader.as<uint16_t>( 2 ); // [2b]
        faces[Front] = block_reader.as<uint16_t>( 4 ); // [2b]
        faces[Rear] = block_reader.as<uint16_t>( 6 ); // [2b]
        faces[Lid] = block_reader.as<uint16_t>( 8 ); // [2b]

        arrows = block_reader.as<uint8_t>( 10 ); // [1b]
        slope_type = block_reader.as<uint8_t>( 11 ); // [1b]
    }

    ///                                             8   4   2   1   0
    /// 15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
    ///

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
/*
    Orientation geometry_rotation()
    {
        switch (( slope_type >> 2 ) & 0x3F )
        {
            case Cube :
            case North26_1 :
                return zero_degrees;
            default :
                return two_seventy_degrees;
        }
    }*/

    uint16_t faces[5]; // left, right, rear, front, lid;
    uint8_t arrows;
    uint8_t slope_type;
};

class Style;
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

    void draw( const std::vector<block_binary_info>& block_info, const Style& style );

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
    void draw( const Style& style )
    {
        for ( int x = 0; x != 256; ++x )
        {
            for ( unsigned int y = 0; y != 256; ++y )
            {
                uint32_t column_index = base[ y*256 + x ];

                if ( column_index < columns_data_size_bytes )
                {
                    glPushMatrix();
                        glTranslatef( x, y, 0.0f );
                        columns.at( column_index ).draw( blocks, style );
                    glPopMatrix();
                }
                else
                    std::cout << "(" << x << "," << y << ") column index: " << column_index << "/" << columns.size() << std::endl;
            }
        }
    }

    std::vector<int32_t> base;
    uint32_t columns_data_size_bytes;
    std::map<uint32_t, dmap_column_binary_info> columns;
    uint32_t num_blocks;
    std::vector<block_binary_info> blocks;
};
