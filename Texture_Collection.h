#include "OpenGL.h"

#include <map>
#include <string>

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

private:
    std::map<Texture_File, Texture_Name> texture_lookup;
};
