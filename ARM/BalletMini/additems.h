#include "view.h"

void AddNewStyle(VIEWDATA *vd);

void AddBeginRef(VIEWDATA *vd);

void AddEndRef(VIEWDATA *vd);

void AddTextItem(VIEWDATA *vd, const char *text, int len);

void AddBrItem(VIEWDATA *vd);

void AddPItem(VIEWDATA *vd);

void AddPictureItemIndex(VIEWDATA *vd, int index);

void AddPictureItem(VIEWDATA *d, void *picture);

void AddPictureItemRGBA(VIEWDATA *vd, void *picture, int width, int height);

void AddPictureItemFrame(VIEWDATA *vd,int width,int height);

void AddPictureItemHr(VIEWDATA *vd);

void AddRadioButton(VIEWDATA *vd);

void AddCheckBoxItem(VIEWDATA *vd);

void AddInputItem(VIEWDATA *vd, const char *text, int len);

void AddPassInputItem(VIEWDATA *vd, const char *text, int len);

void AddButtonItem(VIEWDATA *vd, const char *text, int len);

void AddSelectItem();

