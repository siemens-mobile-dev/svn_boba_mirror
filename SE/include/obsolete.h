#ifdef __cplusplus

#define DEPRECATE(t,f,n) extern t f##_FUNCTION_IS_DEPRECATED_USE_##n(); static t f(...){ f##_FUNCTION_IS_DEPRECATED_USE_##n(); return (t)0; }

DEPRECATE(int,isFileExist,fstat)
DEPRECATE(void,AddMSGHook,GUIObject_Softkey_SetAction)
DEPRECATE(void,UI_Event_toSIDwData,UI_Event_toBookIDwData)
DEPRECATE(void,AddCommand,GUIObject_Softkey_SetText)
DEPRECATE(void,SoftKey_SetEnable,GUIObject_SoftKey_SetEnable)
DEPRECATE(void,SoftKey_AddErrorStr,GUIObject_SoftKey_AddErrorStr)
DEPRECATE(void,SoftKey_RemoveItem,GUIObject_SoftKey_RemoveItem)
DEPRECATE(void,SoftKey_SetVisible,GUIObject_SoftKey_SetVisible)
DEPRECATE(void,SoftKey_SuppressDefaultAction,GUIObject_SoftKey_SuppressDefaultAction)
DEPRECATE(int,GetFocusetListObjectItem,ListMenu_GetSelectedItem)
DEPRECATE(void,SetHeaderText,DispObject_SetTitleText)
DEPRECATE(void,Hide,BookObj_Hide)
DEPRECATE(void,Show,BookObj_Show)
DEPRECATE(int,REQUEST_PROFILE_SETACTIVEPROFILE,Profile_SetActive)
DEPRECATE(int,JavaDialog_Init,JavaDialog_Open)
DEPRECATE(int,JavaDialog_Uninit,JavaDialog_Close)
DEPRECATE(void,GUIObject_SetHeaderText,DispObject_SetTitleText)

#endif
