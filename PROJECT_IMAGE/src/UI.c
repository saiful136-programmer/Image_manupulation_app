#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iup.h>
#include <im.h>
#include <im_process.h>
#include <im_image.h>
#include <iupim.h>

#include "../include/custom.h"

void updateUIImage(Ihandle *self)
{
    (void)self;
    if (state.currentImage == NULL || state.imageWidget == NULL)
    {
        printf("[updateUIImage] currentImage or imageWidget is NULL, aborting.\n");
        return;
    }

    int orig_w = state.currentImage->width;
    int orig_h = state.currentImage->height;

    printf("[updateUIImage] source image: %dx%d, color_space=%d, data_type=%d\n",
           orig_w, orig_h, state.currentImage->color_space, state.currentImage->data_type);

    if (orig_w <= 0 || orig_h <= 0)
    {
        printf("[updateUIImage] invalid source dimensions, aborting.\n");
        return;
    }

    int target_w = 400;
    int target_h = 400;

    if (orig_w > orig_h)
    {
        target_h = (orig_h * 400) / orig_w;
    }
    else
    {
        target_w = (orig_w * 400) / orig_h;
    }

    if (target_w < 1) target_w = 1;
    if (target_h < 1) target_h = 1;

    // ১. RGB ও BYTE ফরম্যাট নিশ্চিত করা
    imImage *workImg = state.currentImage;
    imImage *convertedImg = NULL;

    if (state.currentImage->color_space != IM_RGB || state.currentImage->data_type != IM_BYTE)
    {
        printf("[updateUIImage] source is not RGB/BYTE, converting...\n");

        convertedImg = imImageCreate(orig_w, orig_h, IM_RGB, IM_BYTE);
        if (!convertedImg)
        {
            printf("[updateUIImage] imImageCreate(convertedImg) FAILED.\n");
            return;
        }

        int ok = imProcessConvertColorSpace(state.currentImage, convertedImg);
        if (!ok)
        {
            printf("[updateUIImage] imProcessConvertColorSpace FAILED (returned %d).\n", ok);
            imImageDestroy(convertedImg);
            return;
        }

        workImg = convertedImg;
    }

    // ২. রিসাইজ
    imImage *resizedImg = imImageCreate(target_w, target_h, workImg->color_space, workImg->data_type);
    if (!resizedImg)
    {
        printf("[updateUIImage] imImageCreate(resizedImg) FAILED.\n");
        if (convertedImg) imImageDestroy(convertedImg);
        return;
    }

    int resizeOk = imProcessResize(workImg, resizedImg, 0);
    if (!resizeOk)
    {
        printf("[updateUIImage] imProcessResize FAILED (returned %d).\n", resizeOk);
        imImageDestroy(resizedImg);
        if (convertedImg) imImageDestroy(convertedImg);
        return;
    }

    if (convertedImg)
    {
        imImageDestroy(convertedImg);
    }

    // ৩. IM Planar Data থেকে Packed RGB বাইট বাফার তৈরি
    int total_pixels = target_w * target_h;
    unsigned char *rgb_data = (unsigned char *)malloc((size_t)total_pixels * 3);
    if (!rgb_data)
    {
        printf("[updateUIImage] malloc(rgb_data) FAILED.\n");
        imImageDestroy(resizedImg);
        return;
    }

    unsigned char *r = (unsigned char *)resizedImg->data[0];
    unsigned char *g = (unsigned char *)resizedImg->data[1];
    unsigned char *b = (unsigned char *)resizedImg->data[2];

    if (!r || !g || !b)
    {
        printf("[updateUIImage] resizedImg planes are NULL (r=%p g=%p b=%p) - "
               "image is probably not 3-plane RGB. Aborting.\n", (void*)r, (void*)g, (void*)b);
        free(rgb_data);
        imImageDestroy(resizedImg);
        return;
    }

    for (int i = 0; i < total_pixels; i++)
    {
        rgb_data[i * 3 + 0] = r[i];
        rgb_data[i * 3 + 1] = g[i];
        rgb_data[i * 3 + 2] = b[i];
    }

    // ৪. IUP Native Image অবজেক্ট তৈরি
    Ihandle *new_img = IupImageRGB(target_w, target_h, rgb_data);
    free(rgb_data);
    imImageDestroy(resizedImg);

    if (!new_img)
    {
        printf("[updateUIImage] IupImageRGB FAILED to create image handle.\n");
        return;
    }

    printf("[updateUIImage] new IUP image created: %dx%d, rebuilding label widget...\n", target_w, target_h);

    // ৫. পুরোনো bitmap হ্যান্ডেল ও পুরোনো Label উইজেট মনে রাখা
    Ihandle *old_bitmap = (Ihandle *)IupGetAttributeHandle(state.imageWidget, "IMAGE");
    Ihandle *old_widget = state.imageWidget;

    IupSetHandle("BMP_DISPLAY_IMAGE", new_img);

    // GTK ড্রাইভারে ম্যাপ হয়ে যাওয়া Label-এর উপর dynamically IMAGE বসালে
    // অনেক সময় রিফ্রেশ হয় না, তাই সম্পূর্ণ নতুন Label বানানো হচ্ছে,
    // আর IMAGE ম্যাপ করার আগেই সেট করা হচ্ছে (এটা সবসময় কাজ করে)।
    Ihandle *new_widget = IupLabel(NULL);
    IupSetAttribute(new_widget, "RASTERSIZE", "400x400");
    IupSetAttribute(new_widget, "BGCOLOR", "40 40 40");
    IupSetAttribute(new_widget, "ALIGNMENT", "ACENTER:ACENTER");
    IupSetAttribute(new_widget, "IMAGE", "BMP_DISPLAY_IMAGE");

    if (state.imageContainer)
    {
        IupAppend(state.imageContainer, new_widget);
        IupMap(new_widget);

        IupDetach(old_widget);
        IupDestroy(old_widget);
    }
    else
    {
        printf("[updateUIImage] WARNING: state.imageContainer is NULL, "
               "cannot rebuild the label widget properly.\n");
        IupDestroy(new_widget);
        new_widget = old_widget;
        IupSetAttribute(new_widget, "IMAGE", "BMP_DISPLAY_IMAGE");
    }

    state.imageWidget = new_widget;

    if (old_bitmap && old_bitmap != new_img)
    {
        IupDestroy(old_bitmap);
    }

    // ৬. GUI জোরপূর্বক রিড্র করা
    Ihandle *dlg = IupGetDialog(state.imageWidget);
    if (dlg)
    {
        IupRefresh(dlg);
    }
    IupUpdate(state.imageWidget);

    printf("[updateUIImage] done.\n");
}