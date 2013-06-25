
#include <vector>
#include <stdint.h>

class Animation {
public:
    Animation()
    {
        updates_per_frame = 1;
        current_frame_index = 0;
        num_updates_until_next_frame = 1;
    }

    unsigned int updates_per_frame;
    unsigned int current_frame_index;
    unsigned int num_updates_until_next_frame;

    uint16_t current_frame() const
    {
        return frames[ current_frame_index ];
    }

    std::vector<uint16_t> frames;
};

#include "Style.h"
#include "Map.h"

class Tiles {
public:
    Texture_Collection::Texture_Name placeholder_texture;
    Tiles()
    {
        placeholder_texture = textures.load_texture( "placeholder.tga" );
        depaletted_pixels = new uint32_t[ 4*64 * 4*64 * 62 ];
    }

    void init( const Style& style, const Map& map )
    {
        if ( ! ( style.ppal && style.tile && style.palx )) return;

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
                    int pixel_idx = (63-pixel_row) * 64 + pixel_column;

                    uint8_t pixel = binary_reader( style.tile->color_index_data, 1 ).as<uint8_t>( 0 );
                    depaletted_pixels[ (texture_idx*64*64) + pixel_idx ] = style.ppal->color_at( style.palx->ppalette( texture_idx ), pixel );
                }
            }

            // after reading the 4 textures in the row of textures,
            // load the 4 textures into opengl and save their texture names
            for ( size_t texture_column = 0; texture_column != 4; ++texture_column )
            {
                int texture_idx = texture_row * 4 + texture_column;
                texture_lookup[ texture_idx ] = textures.load_texture( depaletted_pixels + ( texture_idx*64*64 ), 64, 64);
            }
        }

        // initialise animated tiles
        if ( ! map.anim ) return;

        for ( unsigned int a = 0; a != map.anim->animations.size(); ++a )
        {
            Animation animation;

            for ( unsigned int frame = 0; frame != map.anim->animations[a].frames.size(); ++frame )
            {
                animation.updates_per_frame = map.anim->animations[a].game_cycles_per_frame;
                animation.frames.push_back( map.anim->animations[a].frames[ frame ] );
            }

            animations.insert( std::make_pair( map.anim->animations[a].base_tile, animation ));
        }
    }


    Texture_Collection::Texture_Name tile( uint16_t texture_id ) const
    {
        ///
       ///     std::cout << "Texture ID: " << texture_lookup[ texture_id ] << std::endl;
          ///  return placeholder_texture;
        ///

        std::map<uint16_t, Animation>::const_iterator iter = animations.find( texture_id );
        if ( iter != animations.end() )
        {
            return texture_lookup[ iter->second.current_frame() ];
        }

        if ( texture_id < 992 )
            return texture_lookup[ texture_id ];
        else
            return placeholder_texture;
    }

    void update_animations()
    {
        for (
            std::map<uint16_t, Animation>::iterator iter = animations.begin();
            iter != animations.end();
            ++iter
        ) {
            Animation& animation = iter->second;

            animation.num_updates_until_next_frame -= 1;

            if ( animation.num_updates_until_next_frame <= 0 )
            {
                animation.num_updates_until_next_frame = animation.updates_per_frame;
                ++animation.current_frame_index;

                if ( animation.current_frame_index >= animation.frames.size() )
                    animation.current_frame_index = 0;
            }
        }
    }


    ~Tiles() { delete [] depaletted_pixels; }

    std::map<uint16_t, Animation> animations;

    Texture_Collection textures;
    Texture_Collection::Texture_Name texture_lookup[992];
    uint32_t* depaletted_pixels;
};

