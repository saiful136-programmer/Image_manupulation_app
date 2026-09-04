#include <stdio.h>
#include <stdlib.h>

#include <iup.h>
#include <iupim.h>
#include <im.h>
#include <im_image.h>

#include "../include/custom.h"

AppState state;

int main(int argc, char **argv)
{
    state.currentImageFile = NULL;
    state.currentImage = NULL;
    state.undoHistoryCount = 0;
    state.imageWidget = NULL;
    state.imageContainer = NULL;

    for (int i = 0; i < MAX_UNDO_LEVELS; i++)
    {
        state.undoHistory[i] = NULL;
    }

    IupOpen(&argc, &argv);
    IupImOpen(); 
    imFormatRegisterInternal(); 

    setupGui();

    IupClose();

    freeState();
    return EXIT_SUCCESS;
}