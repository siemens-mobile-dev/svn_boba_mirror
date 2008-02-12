unsigned int SearchNextDisplayLine(VIEWDATA *vd, LINECACHE *p, unsigned int *max_h);
int LineDown(VIEWDATA *vd);
int LineUp(VIEWDATA *vd);
int RenderPage(VIEWDATA *vd, int do_draw);
REFCACHE *FindReference(VIEWDATA *vd, unsigned int ref);
int FindReferenceById(VIEWDATA *vd, int i, unsigned int id, unsigned int form_id1, unsigned int form_id2, int tag);
void ChangeRadioButtonState(VIEWDATA *vd, REFCACHE *rf);
void ChangeCheckBoxState(VIEWDATA *vd, REFCACHE *rf);

void ScrollDown(int dy,VIEWDATA *vd);
void ScrollUp(int dy,VIEWDATA *vd);
