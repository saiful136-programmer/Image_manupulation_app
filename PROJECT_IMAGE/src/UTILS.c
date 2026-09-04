#include <stdio.h>
#include <stdlib.h>

#include <iup.h>
#include <iupim.h>
#include <im.h>
#include <im_image.h>

#include "../include/custom.h"

int isAppStateValid()
{
    if (state.currentImage == NULL && state.currentImageFile == NULL)
    {
        Ihandle *dlg = IupMessageDlg();

        IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlg, "TITLE", "Warning!!!");
        IupSetAttribute(dlg, "BUTTONS", "OK");
        IupSetAttribute(dlg, "VALUE", "You haven't selected an image yet!!!\nBye bye!!!\n");

        IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);

        IupDestroy(dlg);
        return 0;
    }

    return 1;
}

void freeState()
{
    if (state.currentImageFile != NULL)
    {
        printf("%s\n", state.currentImageFile);
        free(state.currentImageFile);
        state.currentImageFile = NULL;
    }

    if (state.currentImage != NULL)
    {
        imImageDestroy(state.currentImage);
        state.currentImage = NULL;
    }

    // সব Undo Histroy মেমোরি ক্লিয়ার করা
    for (int i = 0; i < state.undoHistoryCount; i++)
    {
        if (state.undoHistory[i] != NULL)
        {
            imImageDestroy(state.undoHistory[i]);
            state.undoHistory[i] = NULL;
        }
    }
    state.undoHistoryCount = 0;
}

void printState()
{
    printf("Filename : %s\n", state.currentImageFile);
    printf("CurrentImage: %p\n", (void *)state.currentImage);
    printf("UndoCount: %d\n", state.undoHistoryCount);
    printf("ImageWidget: %p\n", (void *)state.imageWidget);
}