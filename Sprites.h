#ifndef _SPRITES_H_
#define _SPRITES_H_

#include "Style.h"

class Sprites {
public:
    Sprites()
    :   texture_lookup( NULL )
    { ; }

    void init( const Style& style )
    {
        if ( ! ( style.sprg && style.sprb && style.ppal && style.palx && style.palb ) )
            return;

        if ( texture_lookup ) { delete [] texture_lookup; texture_lookup = NULL; }
        texture_lookup = new Texture_Collection::Texture_Name[ style.sprb->end ];

        // for each sprite index in sprx
        for ( unsigned int sprite = style.sprb->car_sprite_base; sprite != style.sprb->end; ++sprite )
        {
            ///std::cout << " Sprite(" << sprite << ") " << std::flush;
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

    ~Sprites()
    {
        if ( texture_lookup )
            delete [] texture_lookup;
    }

    Texture_Collection textures;
    Texture_Collection::Texture_Name* texture_lookup;
};

#endif // _SPRITES_H_
