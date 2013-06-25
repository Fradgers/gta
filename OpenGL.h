#ifndef _OPENGL_H_
#define _OPENGL_H_

#include <GL/glfw.h>
#include <GL/glext.h>

class OpenGL {
public:
    OpenGL()
    :   successful_init( true )
    {
        if ( successful_init ) successful_init = glfwInit();
        if ( successful_init ) successful_init = glfwOpenWindow( 800, 600, 0,0,0,0, 8, 0, GLFW_WINDOW );

        glfwSetWindowTitle( "GLFW Test" );

        // setup frustum
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        float aspect_ratio = ( static_cast<float>( 600 ) / 800 );
        glFrustum( -0.5, 0.5, -0.5 * aspect_ratio, 0.5 * aspect_ratio, 1, 500 );


        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glClearDepth( 1.0f );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glEnable( GL_BLEND );

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }

    bool good() { return successful_init; }

    ~OpenGL() { glfwTerminate(); }

private:
    bool successful_init;
};

#endif
