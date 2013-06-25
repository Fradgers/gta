#ifndef _TEXTURE_COLLECTION_H_
#define _TEXTURE_COLLECTION_H_

#include "OpenGL.h"

#include <map>
#include <string>
#include <stdint.h>

class Texture_Collection {
public:
    typedef std::string Texture_File;
    typedef GLuint Texture_Name;

    /// function assumes that each texture filename is unique
    /// and that each texture file will only need to be loaded once
    Texture_Name load_texture( const Texture_File& filename )
    {
        // if file has already been loaded, return its name identifier
        std::map<Texture_File, Texture_Name>::iterator iter =
            texture_lookup.find( filename );

        if ( iter != texture_lookup.end() )
            return iter->second;

        // otherwise, load the file, generate a name identifier for it, and return that
        Texture_Name texture_name( 0 );
        glGenTextures( 1, &texture_name );

        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texture_name );

        if ( glfwLoadTexture2D( filename.c_str(), GLFW_BUILD_MIPMAPS_BIT ) != GL_TRUE )
        {
            std::cout << "Failed to load texture file, \"" << filename << "\"." << std::endl;
            return 0;
        }

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glDisable( GL_TEXTURE_2D );

        texture_lookup.insert( make_pair( filename, texture_name ));

        return texture_name;
    }

    Texture_Name load_texture( uint32_t* data, uint8_t width = 64, uint8_t height = 64 )
    {
        Texture_Name texture_name( 0 );
        glGenTextures( 1, &texture_name );

        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, texture_name );

        ///glTexImage2D( GL_TEXTURE_2D, 0, 4, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, data );
        ///glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );

        gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, data );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1 );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glDisable( GL_TEXTURE_2D );

        return texture_name;
    }

private:
    std::map<Texture_File, Texture_Name> texture_lookup;
};

#endif

/**
        unsigned int texture_idx = 0;
        unsigned int pixel_idx = 0;

        for ( unsigned int row = 0; row != 4*64; ++row )
        {
            int texture_row = row / 64;
            int pixel_row = row - ( texture_row * 64 );

//            cout << "[" << texture << "] " << flush;
//            cout << hex << setw(8) << setfill('0') << ppal->color_at( palx->ppalette( texture ), pixel_index ) << dec;
            for ( unsigned int column = 0; column != 4*64; ++column )
            {
                int texture_column = column / 64;
                int pixel_column = column - ( texture_column * 64 );

                texture_idx = texture_row * 4 + texture_column;
                pixel_idx = pixel_row * 64 + pixel_column;//(( row % 64 ) * 64 ) + ( column % 64 );

                uint8_t pixel = binary_reader( tile->color_index_data, 1 ).as<uint8_t>( 0 );
                depaletted_pixels[ (texture_idx*64*64) + pixel_idx ] = ppal->color_at( palx->ppalette( texture_idx ), pixel );
            }
       //     cout << "." << endl;

            Texture_Collection::Texture_Name bleh = textures.load_texture( depaletted_pixels + (texture_idx*64*64) );
            if ( texture_idx == 0 ) first_texture = bleh;
        }
*/
