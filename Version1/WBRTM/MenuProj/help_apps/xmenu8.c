/*
 * Copyright 1989, 1992 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/*
 * xmenu7.c - SimpleMenu menu
 */

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/bitmaps/xlogo16>

#include <X11/Xaw/Box.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

#define NUM_MENU_ITEMS 12

static String menu_entry_names[] = {
  " quit",
  " item1",
  " item2",
  " item3",
  "line",
  " item5",
  " item6",
  " item7",
  "blank",
  " menu1",
  " menu2",
  " menu3",
};

static Boolean status[NUM_MENU_ITEMS];
static Pixmap mark;

/* ARGSUSED */
static void
MenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;  /* call_data */
{
    int pane_num = (int) client_data;

    printf("Menu item %s has been selected.\n", XtName(w));

    if (pane_num == 0)          /* quit selected. */
        exit(0);
    
    if (status[pane_num]) 
        XtVaSetValues(w, 
                XtNleftBitmap, None,
                NULL);
    else 
        XtVaSetValues(w, 
                XtNleftBitmap, mark,
                NULL);

    status[pane_num] = !status[pane_num];
}

void
main(argc, argv)
char **argv;
int argc;
{
    XtAppContext app_context;
    Widget topLevel, box, menu, button1, button2 , button3 , entry;
    int i;
    Arg arglist[1];

    topLevel = XtVaAppInitialize(
        &app_context,       /* Application context */
        "XMenu7",           /* Application class */
        NULL, 0,            /* command line option list */
        &argc, argv,        /* command line args */
        NULL,               /* for missing app-defaults file */
        NULL);              /* terminate varargs list */

    box = XtCreateManagedWidget(
        "box",  	/* widget name */
        boxWidgetClass, /* widget class */
        topLevel,   	/* parent widget*/
        NULL,   	/* argument list*/
        0   		/* arglist size */
        );

    button1 = XtCreateManagedWidget("nullButton1", 
                          menuButtonWidgetClass, box,
                          arglist, (Cardinal) 0);

    button2 = XtCreateManagedWidget("menuButton", 
                          menuButtonWidgetClass, box,
                          arglist, (Cardinal) 0);
    
    button3 = XtCreateManagedWidget("nullButton3", 
                          menuButtonWidgetClass, box,
                          arglist, (Cardinal) 0);

    menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, 
                          button2, NULL, 0);
  
    for (i = 0; i < NUM_MENU_ITEMS ; i++) {
        String item = menu_entry_names[i];


        if (i == 4)   /* use a line pane */
            entry = XtCreateManagedWidget(item,
                    smeLineObjectClass, menu,
                    NULL, 0);
        else if (i == 8) /* blank entry */
            entry = XtCreateManagedWidget(item, smeObjectClass, menu,
                    NULL, 0);
        else {
            entry = XtCreateManagedWidget(item, smeBSBObjectClass, 
                    menu, NULL, 0);

            XtAddCallback(entry, XtNcallback, MenuSelect, (XtPointer) i);
        }
    }

    mark = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            xlogo16_bits, xlogo16_width, xlogo16_height);

    XtRealizeWidget(topLevel);
    XtAppMainLoop(app_context);
}
