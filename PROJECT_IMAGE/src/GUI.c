#include <stdio.h>
#include <stdlib.h>

#include <iup.h>
#include <iupim.h>
#include <im.h>
#include <im_image.h>

#include "../include/custom.h"

void setupGui()
{
    Ihandle *window;
    Ihandle *vbox;
    Ihandle *hbox;
    Ihandle *menu;

    /* FILE MENU */
    Ihandle *item_open = IupItem("Open", NULL);
    IupSetCallback(item_open, "ACTION", (Icallback)open_clb);

    Ihandle *item_save = IupItem("Save", NULL);
    IupSetCallback(item_save, "ACTION", (Icallback)save_clb);

    Ihandle *item_save_as = IupItem("Save As", NULL);
    IupSetCallback(item_save_as, "ACTION", (Icallback)saveAs_clb);

    Ihandle *item_exit = IupItem("Exit", NULL);
    IupSetCallback(item_exit, "ACTION", (Icallback)ext_clb);

    Ihandle *file_menu = IupMenu(
        item_open,
        item_save,
        item_save_as,
        IupSeparator(),
        item_exit,
        NULL
    );

    Ihandle *file_submenu = IupSubmenu("File", file_menu);

    /* EDIT MENU */
    Ihandle *item_grayScale = IupItem("Grayscale", NULL);
    IupSetCallback(item_grayScale, "ACTION", (Icallback)grayScale_clb);

    Ihandle *item_inversion = IupItem("Inversion", NULL);
    IupSetCallback(item_inversion, "ACTION", (Icallback)Inversion_clb);

    Ihandle *item_horizontalFlip = IupItem("Horizontal Flip", NULL);
    IupSetCallback(item_horizontalFlip, "ACTION", (Icallback)horizontalFlip_clb);

    Ihandle *item_verticalFlip = IupItem("Vertical Flip", NULL);
    IupSetCallback(item_verticalFlip, "ACTION", (Icallback)verticalFlip_clb);

    Ihandle *item_rotate90 = IupItem("Rotate 90deg", NULL);
    IupSetCallback(item_rotate90, "ACTION", (Icallback)rotate90_clb);

    Ihandle *item_blur = IupItem("Blur", NULL);
    IupSetCallback(item_blur, "ACTION", (Icallback)blur_clb);

    Ihandle *item_crop = IupItem("Crop", NULL);
    IupSetCallback(item_crop, "ACTION", (Icallback)cropDialog_clb);

    Ihandle *item_brightness = IupItem("Brightness Adjustment", NULL);
    IupSetCallback(item_brightness, "ACTION", (Icallback)brightnessDialog_clb);

    Ihandle *item_undo = IupItem("Undo", NULL);
    IupSetCallback(item_undo, "ACTION", (Icallback)undo_clb);

    Ihandle *edit_menu = IupMenu(
        item_grayScale,
        item_inversion,
        item_horizontalFlip,
        item_verticalFlip,
        item_rotate90,
        item_blur,
        IupSeparator(),
        item_crop,
        item_brightness,
        IupSeparator(),
        item_undo,
        NULL
    );

    Ihandle *edit_submenu = IupSubmenu("Edit", edit_menu);

    menu = IupMenu(
        file_submenu,
        edit_submenu,
        NULL
    );

    /* IMAGE AREA */
    Ihandle *label = IupLabel("Image View:");

    // IupLabel ব্যবহার যা ইমেজ ঠিকভাবে দেখাবে
    state.imageWidget = IupLabel(NULL);
    IupSetAttribute(state.imageWidget, "RASTERSIZE", "400x400");
    IupSetAttribute(state.imageWidget, "BGCOLOR", "40 40 40");
    IupSetAttribute(state.imageWidget, "ALIGNMENT", "ACENTER:ACENTER");

    // লেবেল এবং ইমেজ ভিউয়ারকে ভার্টিক্যালি সাজানো
    vbox = IupVbox(
        label,
        state.imageWidget,
        NULL
    );
    IupSetAttribute(vbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(vbox, "GAP", "10");

    // updateUIImage() পরে এই vbox-এর ভেতরে imageWidget রিপ্লেস করতে পারবে
    state.imageContainer = vbox;

    // ডানে ও বামে Fill দিয়ে কন্টেন্টকে অনুভূমিকভাবে (Horizontally) ঠিক মাঝখানে রাখা
    hbox = IupHbox(
        IupFill(),
        vbox,
        IupFill(),
        NULL
    );

    // নিচে Fill দিয়ে কন্টেন্টকে স্ক্রিনের উপরের দিকে ফিক্সড রাখা
    Ihandle *main_vbox = IupVbox(
        hbox,
        IupFill(),
        NULL
    );
    IupSetAttribute(main_vbox, "MARGIN", "15x15");

    window = IupDialog(main_vbox);

    IupSetAttributeHandle(window, "MENU", menu);
    IupSetAttribute(window, "TITLE", "Image Manipulation Software");
    IupSetAttribute(window, "SIZE", "700x500");

    IupShowXY(window, IUP_CENTER, IUP_CENTER);

    IupMainLoop();
}