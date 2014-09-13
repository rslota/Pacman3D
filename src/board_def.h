#ifndef board_def_h
#define board_def_h

#include <osg/ref_ptr>

double boardSizeX = 500.0;
double boardSizeY = 500.0;

// y (y field: 0, position: boardSizeY)
// |
// |
// |
// |
// |
// |
// |
// |
// |                   (position: 0, 0)
// |
// |
// |
// |
// |
// |
// |
// |-----------------------------------------------------> x
// (x field: 0, position: 0)

std::vector<std::string> boardDefinition = {

                "**********************************",
                "*               **               *",
                "*               **               *",
                "*  ****  *****  **  *****  ****  *",
                "*  ****  *****  **  *****  ****  *",
                "*  ****  *****  **  *****  ****  *",
                "*                                *",
                "*                                *",
                "*  ****  **  ********  **  ****  *",
                "*  ****  **  ********  **  ****  *",
                "*        **     **     **        *",
                "*        **     **     **        *",
                "*******  *****  **  *****  *******",
                "      *  *****  **  *****  *      ",
                "      *  **            **  *      ",
                "      *  **            **  *      ",
                "      *  **  ***  ***  **  *      ",
                "*******  **  *      *  **  *******",
                "             *      *             ",
                "             *      *             ",
                "*******  **  *      *  **  *******",
                "      *  **  ********  **  *      ",
                "      *  **            **  *      ",
                "      *  **            **  *      ",
                "      *  **  ********  **  *      ",
                "*******  **  ********  **  *******",
                "*               **               *",
                "*               **               *",
                "*  ****  *****  **  *****  ****  *",
                "*  ****  *****  **  *****  ****  *",
                "*    **                    **    *",
                "*    **                    **    *",
                "***  **  **  ********  **  **  ***",
                "***  **  **  ********  **  **  ***",
                "*        **     **     **        *",
                "*        **     **     **        *",
                "*  ***********  **  ***********  *",
                "*  ***********  **  ***********  *",
                "*                                *",
                "*                                *",
                "**********************************"
};

#endif // board_def_h
