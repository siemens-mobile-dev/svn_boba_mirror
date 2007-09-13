unsigned int SearchNextDisplayLine(VIEWDATA *vd, LINECACHE *p, unsigned int *max_h);
int LineDown(VIEWDATA *vd);
int LineUp(VIEWDATA *vd);
int RenderPage(VIEWDATA *vd, int do_draw);
REFCACHE *FindReference(VIEWDATA *vd, unsigned int ref);
REFCACHE *FindReferenceById(REFCACHE *rf, char *id, char *form_id1, char *form_id2, int tag);
void ChangeRadioButtonState(VIEWDATA *vd, REFCACHE *rf);
void ChangeCheckBoxState(VIEWDATA *vd, REFCACHE *rf);

