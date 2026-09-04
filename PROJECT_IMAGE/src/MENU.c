#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <iupim.h>
#include <im.h>
#include <im_image.h>

#include "../include/custom.h"

int open_clb(Ihandle *self)
{
    (void)self;
    Ihandle *file_dlg = IupFileDlg();

    IupSetAttribute(file_dlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(file_dlg, "TITLE", "Select Image");
    IupSetAttribute(file_dlg, "EXTFILTER", "BMP Images (*.bmp)|*.bmp|All Files (*.*)|*.*|");

    IupPopup(file_dlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(file_dlg, "STATUS") != -1)
    {
        char *fileName = IupGetAttribute(file_dlg, "VALUE");

        if (state.currentImageFile != NULL) free(state.currentImageFile);
        if (state.currentImage != NULL) imImageDestroy(state.currentImage);

        for (int i = 0; i < state.undoHistoryCount; i++)
        {
            if (state.undoHistory[i]) imImageDestroy(state.undoHistory[i]);
        }
        state.undoHistoryCount = 0;

        size_t s = strlen(fileName);
        state.currentImageFile = (char *)malloc(sizeof(char) * (s + 1));
        strcpy(state.currentImageFile, fileName);

        int err = 0;
        state.currentImage = imFileImageLoadBitmap(state.currentImageFile, 0, &err);

        if (err != IM_ERR_NONE || state.currentImage == NULL)
        {
            Ihandle *dlg = IupMessageDlg();
            IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
            IupSetAttribute(dlg, "TITLE", "Error!!!");
            IupSetAttribute(dlg, "VALUE", "Couldn't open the image!");
            IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
            IupDestroy(dlg);
            IupDestroy(file_dlg);
            return IUP_DEFAULT;
        }

        updateUIImage(state.imageWidget);
    }

    IupDestroy(file_dlg);
    return IUP_DEFAULT;
}

int save_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_DEFAULT;

    if (state.currentImageFile == NULL || strlen(state.currentImageFile) == 0)
    {
        return saveAs_clb(self);
    }

    int err = imFileImageSave(
        state.currentImageFile,
        "BMP",
        state.currentImage
    );

    if (err != IM_ERR_NONE)
    {
        Ihandle *dlg = IupMessageDlg();
        IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
        IupSetAttribute(dlg, "TITLE", "Error!!!");
        IupSetAttribute(dlg, "VALUE", "Couldn't save the image!");
        IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
        IupDestroy(dlg);
        return IUP_DEFAULT;
    }

    Ihandle *dlg = IupMessageDlg();
    IupSetAttribute(dlg, "DIALOGTYPE", "INFORMATION");
    IupSetAttribute(dlg, "TITLE", "Success!!!");
    IupSetAttribute(dlg, "VALUE", "Successfully saved the image!");
    IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
    IupDestroy(dlg);

    return IUP_DEFAULT;
}

int saveAs_clb(Ihandle *self)
{
    if (!isAppStateValid())
        return IUP_DEFAULT;

    Ihandle *file_dlg = IupFileDlg();

    IupSetAttribute(file_dlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(file_dlg, "TITLE", "Save image as...");
    IupSetAttribute(file_dlg, "EXTFILTER", "BMP Images (*.bmp)|*.bmp|");
    IupSetAttribute(file_dlg, "EXTDEFAULT", "bmp");

    IupPopup(file_dlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(file_dlg, "STATUS") != -1)
    {
        char *fileName = IupGetAttribute(file_dlg, "VALUE");

        int err = imFileImageSave(
            fileName,
            "BMP",
            state.currentImage
        );

        if (err)
        {
            Ihandle *dlg = IupMessageDlg();
            IupSetAttribute(dlg, "DIALOGTYPE", "WARNING");
            IupSetAttribute(dlg, "TITLE", "Error!!!");
            IupSetAttribute(dlg, "VALUE", "Couldn't save the image!");
            IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
            IupDestroy(dlg);
        }
        else
        {
            if (state.currentImageFile != NULL) free(state.currentImageFile);

            size_t s = strlen(fileName);
            state.currentImageFile = (char *)malloc(sizeof(char) * (s + 1));
            strcpy(state.currentImageFile, fileName);

            Ihandle *dlg = IupMessageDlg();
            IupSetAttribute(dlg, "DIALOGTYPE", "INFORMATION");
            IupSetAttribute(dlg, "TITLE", "Success!!!");
            IupSetAttribute(dlg, "VALUE", "Successfully saved the image!");
            IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
            IupDestroy(dlg);

            updateUIImage(self);
        }
    }

    IupDestroy(file_dlg);
    return IUP_DEFAULT;
}

int ext_clb(Ihandle *self)
{
    (void)self;
    return IUP_CLOSE;
}