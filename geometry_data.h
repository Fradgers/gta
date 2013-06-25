
enum Block_Type {
    /// standard cube
    Cube = 0, // 0
    /// 26 slopes
    North26_1, North26_2, South26_1, South26_2, West26_1, West26_2, East26_1, East26_2, // 1-8
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
    Diagonal_Slope1_NW, Diagonal_Slope1_NE, Diagonal_Slope1_SW, Diagonal_Slope1_SE, // 49-52
    /// partial blocks
    Partial_W, Partial_E, Partial_N, Partial_S,
    Partial_NW, Partial_NE, Partial_SE, Partial_SW, // 53-60
    Partial_Centre, // 61
    /// internal use
    Reserved, Under_Slope, // 62-63
    /// alternate diagonal slopes
    Diagonal_Slope3_NW, Diagonal_Slope3_NE, Diagonal_Slope3_SW, Diagonal_Slope3_SE,
    BLOCK_TYPE_COUNT
};


enum Face_Type {
    No_Face = 0,
    Full_Square, Half_Square, Quarter_Square,
    Seven_Eighth_Height, Three_Quarter_Height, Five_Eighth_Height, Half_Height, Three_Eighth_Height, Quarter_Height, Eighth_Height,
    Lower26L, Lower26R, Upper26L, Upper26R,
    Triangle_NW, Triangle_NE, Triangle_SE, Triangle_SW,
    Slope_NS45, Slope_EW45,
    Slope_NS26, Slope_EW26,
    Slope_NS7, Slope_EW7,
    Triangle_Point_Top,
    Triangle_Point_Bottom,

    FACE_TYPE_COUNT
};

enum Axis { No_Axis = 0, X_Axis = 1, Y_Axis = 2, Z_Axis = 4 };

class Face {
public:
    Face() { }

    Face( Face_Type face, GLfloat rot, Axis ax, GLfloat off, GLfloat ln = 0.0f )
    :   type( face ), rotation(rot), axis(ax), offset(off), lean(ln)
    { }

    Face_Type type;
    GLfloat rotation;
    Axis axis;
    GLfloat offset;
    GLfloat lean;
};

const GLfloat eight_segment_slope = 7.125f;
const GLfloat two_segment_slope = 26.565f;
const GLfloat one_segment_slope = 45.0f;

Face blocks[ BLOCK_TYPE_COUNT ][ 5 ] = {
    /// Cube (x1)
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0.5f ) }, /// Cube

    /// 26 degree slopes (x8)
    // North26_x
    /// Rear                                                      Right                                                     Front                                                     Left                                                       Lid
    { Face( Half_Height, 0, No_Axis, 0 ), Face( Lower26R, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Lower26L, 0, No_Axis, 0 ), Face( Slope_NS26, two_segment_slope, X_Axis, -0.25f ) }, /// North26_x
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Upper26R, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Upper26L, 0, No_Axis, 0 ), Face( Slope_NS26, two_segment_slope, X_Axis,  0.25f ) }, //

    // South26_x
    { Face( No_Face,     0, No_Axis, 0 ), Face( Lower26L, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Lower26R, 0, No_Axis, 0 ), Face( Slope_NS26, -two_segment_slope, X_Axis, -0.25f ) }, /// South26_x
    { Face( Half_Height, 0, No_Axis, 0 ), Face( Upper26L, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Upper26R, 0, No_Axis, 0 ), Face( Slope_NS26, -two_segment_slope, X_Axis,  0.25f ) }, //

    // West26_x
    { Face( Lower26R, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Lower26L, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Slope_EW26, two_segment_slope, Y_Axis, -0.25f ) }, /// West26_x
    { Face( Upper26R, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Upper26L, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Slope_EW26, two_segment_slope, Y_Axis,  0.25f ) }, //

    // East26_x
    { Face( Lower26L, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Lower26R, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Slope_EW26, -two_segment_slope, Y_Axis, -0.25f ) }, /// East26_x
    { Face( Upper26L, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Upper26R, 0, No_Axis, 0 ), Face( Half_Height, 0, No_Axis, 0 ), Face( Slope_EW26, -two_segment_slope, Y_Axis,  0.25f ) }, //

    /// 7 degree slopes (x32)
    // North7_x
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, -0.4375f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, -0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, -0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, -0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, +0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, +0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, +0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, eight_segment_slope, X_Axis, +0.4375f ) },

    // South7_x
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, -0.4375f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, -0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, -0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, -0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, +0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, +0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, +0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_NS7, -eight_segment_slope, X_Axis, +0.4375f ) },

    // West7_x
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, -0.4375f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, -0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, -0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, -0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, +0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, +0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, +0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, eight_segment_slope, Y_Axis, +0.4375f ) },

    // East7_x
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, -0.4375f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, -0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, -0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, -0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, +0.0625f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, +0.1875f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, +0.3125f ) },
    { Face(), Face(), Face(), Face(), Face( Slope_EW7, -eight_segment_slope, Y_Axis, +0.4375f ) },

    /// 45 degree slopes (x4)
    /// Rear                                                      Right                                                                      Front                                                       Left                                                       Lid
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_SE, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Triangle_SW, 0, No_Axis, 0 ), Face( Slope_NS45,  one_segment_slope, X_Axis, 0 ) }, /// North45
    { Face( No_Face,     0, No_Axis, 0 ), Face( Triangle_SW, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_SE, 0, No_Axis, 0 ), Face( Slope_NS45, -one_segment_slope, X_Axis, 0 ) }, /// South45
    { Face( Triangle_SE, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Triangle_SW, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Slope_EW45, one_segment_slope, Y_Axis, 0 ) }, /// West45
    { Face( Triangle_SW, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_SE, 0, No_Axis, 0 ), Face( No_Face,     0, No_Axis, 0 ), Face( Slope_EW45, -one_segment_slope, Y_Axis, 0 ) }, /// East45

    /// Diagonals (x4)
    { Face( No_Face, 0, No_Axis, 0 ),     Face( Full_Square,     0, No_Axis,    0 ),       Face( Full_Square, 0, No_Axis, 0 ),    Face( Slope_EW45, -45.0f, Z_Axis,  -0.5f ),  Face( Triangle_SE, 0, No_Axis, 0.5f ) }, /// Diagonal_NW
    { Face( No_Face,   0, No_Axis, 0 ),  Face( Slope_EW45,  45.0f, Z_Axis, -0.5f ),  Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square,     0, No_Axis,     0 ), Face( Triangle_SW, 0, No_Axis, 0.5f ) }, /// Diagonal_NE
    { Face( Full_Square,   0, No_Axis, 0 ),  Face( Full_Square, 0, No_Axis, 0 ),        Face( No_Face,     0, No_Axis,     0 ), Face( Slope_EW45, 45.0f, Z_Axis, -0.5f ), Face( Triangle_NE, 0, No_Axis, 0.5f ) }, /// Diagonal_SW
    { Face( Full_Square,     0, No_Axis, 0 ),    Face( Slope_EW45,  -45.0f, Z_Axis,  -0.5f ), Face( No_Face, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_NW, 0, No_Axis, 0.5f ) }, /// Diagonal_SE

    /// Diagonal Slopes ( three low corners, one raised corner ) (x4)
    { Face(), Face(), Face(), Face( Triangle_Point_Top, -45.0f, Z_Axis, -0.8535f, -35.264f ), Face() }, // NW
    { Face(), Face( Triangle_Point_Top, 45.0f, Z_Axis, -0.8535f, -35.264f ), Face(), Face(), Face() }, // NE
    { Face(), Face(), Face(), Face( Triangle_Point_Top, 45.0f, Z_Axis, -0.8535f, -35.264f ), Face() }, // SW
    { Face(), Face( Triangle_Point_Top, -45.0f, Z_Axis, -0.8535f, -35.264f ), Face(), Face(), Face() }, // SE

    /// Partial Blocks (x9)
    {  },
    {  },
    {  },
    {  },
    {  },
    {  },
    {  },
    {  },
    {  },

    /// Reserved (x2)
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0.5f ) }, /// Reserved
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0.5f ) }, /// Under_Slope

    /// Alternate Diagonal Slopes ( 3 raised corners, one low corner ) (x4)
    { Face(), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_Point_Bottom, -45.0f, Z_Axis, -0.1464f, -35.264f ), Face( Triangle_SE, 0, No_Axis, 0.5f ) }, // NW
    { Face(), Face( Triangle_Point_Bottom, 45.0f, Z_Axis, -0.1464f, -35.264f ), Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_SW, 0, No_Axis, 0.5f ) }, // NE
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Full_Square, 0, No_Axis, 0 ), Face(), Face( Triangle_Point_Bottom, 45.0f, Z_Axis, -0.1464f, -35.264f ), Face( Triangle_NE, 0, No_Axis, 0.5f ) }, // SW
    { Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_Point_Bottom, -45.0f, Z_Axis, -0.1464f, -35.264f ), Face(), Face( Full_Square, 0, No_Axis, 0 ), Face( Triangle_NW, 0, No_Axis, 0.5f ) }, // SE
};


static GLfloat faces[FACE_TYPE_COUNT][12] = {
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 }, // No_Face

    { 0, 1, 0,   0, 0, 0,   1, 0, 0,   1, 1, 0 }, // Full_Square
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },

    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 }, // Seven_Eighth_Height
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },
    { 0, 0, 0,    1, 0, 0,     1, 0.5f, 0,      0, 0.5f, 0 },
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },
    { 0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0 },

    { 0, 0.5f, 0,   0, 0, 0,   1, 0, 0,   1, 0, 0 }, // Lower26L
    { 0, 0, 0,    1, 0, 0,     1, 0.5f, 0,      0, 0, 0 },
    { 0, 1, 0,    0, 0, 0,     1, 0, 0,      1, 0.5f, 0 },
    { 0, 0.5f, 0,    0, 0, 0,     1, 0, 0,      1, 1, 0 },

    { 0, 1, 0,    0, 0, 0,     1, 1, 0,      1, 1, 0 }, // Triangle_NW
    { 0, 1, 0,    1, 0, 0,     1, 1, 0,      1, 1, 0 }, // Triangle_NE
    { 0, 0, 0,    1, 0, 0,     1, 1, 0,      1, 1, 0 }, // Triangle_SE
    { 0, 1, 0,    0, 0, 0,     1, 0, 0,      1, 0, 0 }, // Triangle_SW

    { 0, 1.207f, 0,   0, -0.207f, 0,   1, -0.207f, 0,   1, 1.207f, 0 }, // Slope_NS45
    { -0.207f, 1, 0,   -0.207f, 0, 0,   1.207f, 0, 0,   1.207f, 1, 0 }, // Slope_EW45

    { 0, 1.059f, 0,   0, -0.059f, 0,   1, -0.059f, 0,   1, 1.059f, 0 }, // Slope_NS26
    { -0.059f, 1, 0,   -0.059f, 0, 0,   1.059f, 0, 0,   1.059f, 1, 0 }, // Slope_EW26

    { 0, 1.0078f, 0,   0, -0.0078f, 0,   1, -0.0078f, 0,   1, 1.0078f, 0 }, // Slope_NS7
    { -0.0078f, 1, 0,   -0.0078f, 0, 0,   1.0078f, 0, 0,   1.0078f, 1, 0 }, // Slope_EW7

    { 0.5f, 1.112f, 0,   -.207f, -.112f, 0,    1.207f, -.112f, 0,    0.5f, 1.112f, 0  }, // Triangle_Point_Top
    { -.207f, 1.112f, 0,    0.5f, -.112f, 0,    0.5f, -.112f, 0,   1.207f, 1.112f, 0  }, // Triangle_Point_Bottom
};
