#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupim.h>
#include <im.h>
#include <im_image.h>

#include "../include/custom.h"

/* =========================================================
   UNDO STACK HELPERS
   ========================================================= */

void pushUndoState(imImage *img)
{
    if (!img) return;

    if (state.undoHistoryCount >= MAX_UNDO_LEVELS)
    {
        if (state.undoHistory[0])
            imImageDestroy(state.undoHistory[0]);

        for (int i = 0; i < MAX_UNDO_LEVELS - 1; i++)
        {
            state.undoHistory[i] = state.undoHistory[i + 1];
        }
        state.undoHistoryCount--;
    }

    state.undoHistory[state.undoHistoryCount++] = imImageDuplicate(img);
}

/* =========================================================
   CROP
   ========================================================= */

int crop_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    Ihandle *cropXInput = (Ihandle *)IupGetAttributeHandle(self, "CROP_X");
    Ihandle *cropYInput = (Ihandle *)IupGetAttributeHandle(self, "CROP_Y");
    Ihandle *cropWInput = (Ihandle *)IupGetAttributeHandle(self, "CROP_W");
    Ihandle *cropHInput = (Ihandle *)IupGetAttributeHandle(self, "CROP_H");

    char *strX = IupGetAttribute(cropXInput, "VALUE");
    char *strY = IupGetAttribute(cropYInput, "VALUE");
    char *strW = IupGetAttribute(cropWInput, "VALUE");
    char *strH = IupGetAttribute(cropHInput, "VALUE");

    int startX = (strX && strlen(strX) > 0) ? atoi(strX) : 0;
    int startY = (strY && strlen(strY) > 0) ? atoi(strY) : 0;
    int cropW = (strW && strlen(strW) > 0) ? atoi(strW) : 0;
    int cropH = (strH && strlen(strH) > 0) ? atoi(strH) : 0;

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    if (cropW <= 0 || cropH <= 0 || startX >= w || startY >= h || startX < 0 || startY < 0)
    {
        Ihandle *dlg = IupMessageDlg();
        IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlg, "TITLE", "Error!!!");
        IupSetAttribute(dlg, "BUTTONS", "OK");
        IupSetAttribute(dlg, "VALUE", "Invalid Crop Inputs");

        IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
        IupDestroy(dlg);
        return IUP_DEFAULT;
    }

    if (startX + cropW > w) cropW = w - startX;
    if (startY + cropH > h) cropH = h - startY;

    pushUndoState(state.currentImage);

    imImage *croppedImage = imImageCreate(cropW, cropH, state.currentImage->color_space, state.currentImage->data_type);

    unsigned char *src_r = state.currentImage->data[0];
    unsigned char *src_g = state.currentImage->data[1];
    unsigned char *src_b = state.currentImage->data[2];

    unsigned char *dst_r = croppedImage->data[0];
    unsigned char *dst_g = croppedImage->data[1];
    unsigned char *dst_b = croppedImage->data[2];

    for (int i = 0; i < cropH; i++)
    {
        for (int j = 0; j < cropW; j++)
        {
            int old_col = startX + j;
            int old_row = startY + i;

            int src_idx = (old_row * w) + old_col;
            int dst_idx = (i * cropW) + j;

            dst_r[dst_idx] = src_r[src_idx];
            dst_g[dst_idx] = src_g[src_idx];
            dst_b[dst_idx] = src_b[src_idx];
        }
    }

    imImageDestroy(state.currentImage);
    state.currentImage = croppedImage;

    updateUIImage(self);
    return IUP_CLOSE; // পপ-আপ অটো বন্ধ হবে
}

/* =========================================================
   BRIGHTNESS
   ========================================================= */

int brightness_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    Ihandle *inputBox = IupGetAttributeHandle(self, "MY_INPUT_TXT");
    char *str = IupGetAttribute(inputBox, "VALUE");

    if (!str || strlen(str) == 0)
        return IUP_DEFAULT;

    int adjustment = atoi(str);

    if (adjustment < -255 || adjustment > 255)
    {
        Ihandle *dlg = IupMessageDlg();
        IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlg, "TITLE", "Error!!!");
        IupSetAttribute(dlg, "BUTTONS", "OK");
        IupSetAttribute(dlg, "VALUE", "Invalid Brightness Adjustment Inputs");

        IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
        IupDestroy(dlg);
        return IUP_DEFAULT;
    }

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    for (int i = 0; i < w * h; i++)
    {
        int _r = r[i] + adjustment;
        int _g = g[i] + adjustment;
        int _b = b[i] + adjustment;

        if (_r > 255) _r = 255;
        if (_g > 255) _g = 255;
        if (_b > 255) _b = 255;

        if (_r < 0) _r = 0;
        if (_g < 0) _g = 0;
        if (_b < 0) _b = 0;

        r[i] = (unsigned char)_r;
        g[i] = (unsigned char)_g;
        b[i] = (unsigned char)_b;
    }

    updateUIImage(self);
    return IUP_CLOSE; // পপ-আপ অটো বন্ধ হবে
}

/* =========================================================
   GRAYSCALE
   ========================================================= */

int grayScale_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    for (int i = 0; i < w * h; i++)
    {
        unsigned char gray = 0.299 * r[i] + 0.587 * g[i] + 0.114 * b[i];

        r[i] = gray;
        g[i] = gray;
        b[i] = gray;
    }

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   INVERSION
   ========================================================= */

int Inversion_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    for (int i = 0; i < w * h; i++)
    {
        r[i] = 255 - r[i];
        g[i] = 255 - g[i];
        b[i] = 255 - b[i];
    }

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   HORIZONTAL FLIP
   ========================================================= */

int horizontalFlip_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w / 2; j++)
        {
            int idx1 = i * w + j;
            int idx2 = i * w + (w - 1 - j);

            unsigned char temp;
            temp = r[idx1]; r[idx1] = r[idx2]; r[idx2] = temp;
            temp = g[idx1]; g[idx1] = g[idx2]; g[idx2] = temp;
            temp = b[idx1]; b[idx1] = b[idx2]; b[idx2] = temp;
        }
    }

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   VERTICAL FLIP
   ========================================================= */

int verticalFlip_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    for (int i = 0; i < h / 2; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int idx1 = i * w + j;
            int idx2 = (h - 1 - i) * w + j;

            unsigned char temp;
            temp = r[idx1]; r[idx1] = r[idx2]; r[idx2] = temp;
            temp = g[idx1]; g[idx1] = g[idx2]; g[idx2] = temp;
            temp = b[idx1]; b[idx1] = b[idx2]; b[idx2] = temp;
        }
    }

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   ROTATE 90 DEGREE
   ========================================================= */

int rotate90_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    imImage *newImage = imImageCreate(h, w, state.currentImage->color_space, state.currentImage->data_type);

    unsigned char *n_r = newImage->data[0];
    unsigned char *n_g = newImage->data[1];
    unsigned char *n_b = newImage->data[2];

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int src_index = i * w + j;
            int dst_index = (w - 1 - j) * h + i;

            n_r[dst_index] = r[src_index];
            n_g[dst_index] = g[src_index];
            n_b[dst_index] = b[src_index];
        }
    }

    imImageDestroy(state.currentImage);
    state.currentImage = newImage;

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   BLUR
   ========================================================= */

int blur_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    pushUndoState(state.currentImage);

    int w = state.currentImage->width;
    int h = state.currentImage->height;

    unsigned char *r = state.currentImage->data[0];
    unsigned char *g = state.currentImage->data[1];
    unsigned char *b = state.currentImage->data[2];

    imImage *newImage = imImageCreate(w, h, state.currentImage->color_space, state.currentImage->data_type);

    unsigned char *n_r = newImage->data[0];
    unsigned char *n_g = newImage->data[1];
    unsigned char *n_b = newImage->data[2];

    int dx[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            unsigned int sum_r = 0, sum_g = 0, sum_b = 0;
            unsigned int divider = 0;

            for (int k = 0; k < 9; k++)
            {
                int neighbor_row = i + dx[k];
                int neighbor_col = j + dy[k];

                if (neighbor_row >= 0 && neighbor_row < h && neighbor_col >= 0 && neighbor_col < w)
                {
                    int src = neighbor_row * w + neighbor_col;
                    sum_r += r[src];
                    sum_g += g[src];
                    sum_b += b[src];
                    divider++;
                }
            }

            int dst = i * w + j;
            n_r[dst] = (unsigned char)(sum_r / divider);
            n_g[dst] = (unsigned char)(sum_g / divider);
            n_b[dst] = (unsigned char)(sum_b / divider);
        }
    }

    imImageDestroy(state.currentImage);
    state.currentImage = newImage;

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   UNDO (Multiple undo support)
   ========================================================= */

int undo_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_CLOSE;

    if (state.undoHistoryCount == 0)
        return IUP_DEFAULT;

    imImageDestroy(state.currentImage);
    state.currentImage = state.undoHistory[--state.undoHistoryCount];

    updateUIImage(self);
    return IUP_DEFAULT;
}

/* =========================================================
   DIALOGS
   ========================================================= */

int cropDialog_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_DEFAULT;

    Ihandle *xInput = IupText(NULL);
    Ihandle *yInput = IupText(NULL);
    Ihandle *wInput = IupText(NULL);
    Ihandle *hInput = IupText(NULL);

    IupSetAttribute(xInput, "MASK", IUP_MASK_UINT);
    IupSetAttribute(yInput, "MASK", IUP_MASK_UINT);
    IupSetAttribute(wInput, "MASK", IUP_MASK_UINT);
    IupSetAttribute(hInput, "MASK", IUP_MASK_UINT);

    IupSetAttribute(xInput, "VISIBLECOLUMNS", "6");
    IupSetAttribute(yInput, "VISIBLECOLUMNS", "6");
    IupSetAttribute(wInput, "VISIBLECOLUMNS", "6");
    IupSetAttribute(hInput, "VISIBLECOLUMNS", "6");

    Ihandle *cropButton = IupButton("Crop", NULL);

    IupSetAttributeHandle(cropButton, "CROP_X", xInput);
    IupSetAttributeHandle(cropButton, "CROP_Y", yInput);
    IupSetAttributeHandle(cropButton, "CROP_W", wInput);
    IupSetAttributeHandle(cropButton, "CROP_H", hInput);

    IupSetCallback(cropButton, "ACTION", (Icallback)crop_clb);

    Ihandle *box = IupVbox(
        IupHbox(IupLabel("X: "), xInput, NULL),
        IupHbox(IupLabel("Y: "), yInput, NULL),
        IupHbox(IupLabel("Width: "), wInput, NULL),
        IupHbox(IupLabel("Height: "), hInput, NULL),
        cropButton,
        NULL
    );

    IupSetAttribute(box, "GAP", "8");
    IupSetAttribute(box, "MARGIN", "15x15");

    Ihandle *dlg = IupDialog(box);
    IupSetAttribute(dlg, "TITLE", "Crop Image");
    IupSetAttribute(dlg, "SIZE", "250x180");

    IupPopup(dlg, IUP_CENTER, IUP_CENTER);
    IupDestroy(dlg);

    return IUP_DEFAULT;
}

int brightnessDialog_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_DEFAULT;

    Ihandle *input = IupText(NULL);
    IupSetAttribute(input, "MASK", IUP_MASK_INT);
    IupSetAttribute(input, "VISIBLECOLUMNS", "6");

    Ihandle *apply = IupButton("Apply", NULL);
    IupSetAttributeHandle(apply, "MY_INPUT_TXT", input);
    IupSetCallback(apply, "ACTION", (Icallback)brightness_clb);

    Ihandle *box = IupVbox(
        IupHbox(IupLabel("Brightness (-255 to 255): "), input, NULL),
        apply,
        NULL
    );

    IupSetAttribute(box, "GAP", "10");
    IupSetAttribute(box, "MARGIN", "15x15");

    Ihandle *dlg = IupDialog(box);
    IupSetAttribute(dlg, "TITLE", "Brightness Adjustment");
    IupSetAttribute(dlg, "SIZE", "300x120");

    IupPopup(dlg, IUP_CENTER, IUP_CENTER);
    IupDestroy(dlg);

    return IUP_DEFAULT;
}