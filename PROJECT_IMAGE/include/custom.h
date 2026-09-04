#ifndef CUSTOM_H
#define CUSTOM_H

#include <iup.h>
#include <im.h>
#include <im_image.h>

#define MAX_UNDO_LEVELS 10

typedef struct
{
    char *currentImageFile;
    imImage *currentImage;
    imImage *undoHistory[MAX_UNDO_LEVELS];
    int undoHistoryCount;
    Ihandle *imageWidget;
    Ihandle *imageContainer;
} AppState;

extern AppState state;

// Function Prototypes
void setupGui(void);
int isAppStateValid(void);
void freeState(void);
void printState(void);
void updateUIImage(Ihandle *self);

// Callback Prototypes
int open_clb(Ihandle *self);
int save_clb(Ihandle *self);
int saveAs_clb(Ihandle *self);
int ext_clb(Ihandle *self);

int grayScale_clb(Ihandle *self);
int Inversion_clb(Ihandle *self);
int horizontalFlip_clb(Ihandle *self);
int verticalFlip_clb(Ihandle *self);
int rotate90_clb(Ihandle *self);
int blur_clb(Ihandle *self);

int crop_clb(Ihandle *self);
int cropDialog_clb(Ihandle *self);
int brightness_clb(Ihandle *self);
int brightnessDialog_clb(Ihandle *self);
int undo_clb(Ihandle *self);

#endif