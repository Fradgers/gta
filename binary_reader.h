#ifndef _BINARY_READER_H_
#define _BINARY_READER_H_

#include <iostream>
#include <stdint.h>
//#include <stream>

class binary_reader {
public:
    binary_reader( std::istream& stream, size_t num_bytes )
    :   data_length( num_bytes ),
        data( new char[ data_length ] )
    {
        stream.read( data, data_length );
    }

    template <typename T>
    T as( const std::streamoff& offset )
    {
        if ( offset + sizeof(T) > data_length )
        {
            std::cerr << "stream ERROR - reinterpret overflow" << std::endl;
            throw;
        }

        return *reinterpret_cast<T*>( data + offset );
    }

    std::string as_string() { return std::string( data, data_length ); }

    ~binary_reader() { delete [] data; }

public:
    uint32_t data_length;
    char* data;
};

class chunk_reader {
public:
    chunk_reader( std::istream& file_stream )
    {
        binary_reader chunk_header( file_stream, 8 );

        chunk_type       = chunk_header.as<uint32_t>( 0 );
        chunk_size_bytes = chunk_header.as<uint32_t>( 4 );
    }

    std::string body( std::istream& file_stream )
    {
        return binary_reader( file_stream, chunk_size_bytes ).as_string();
    }

    uint32_t chunk_type;
    uint32_t chunk_size_bytes;
};

#endif
