#ifndef _XML_PARSER_
  #define _XML_PARSER

typedef struct{
  void *next;
  char *name;
  char *param;
}XMLAttr;


typedef struct{
  void *next;
  void *subnode;
  char *name;
  XMLAttr *attr;
  char *value;
} XMLNode;


#define MS_BEGIN 1						///< Начало парсинга
#define MS_BEGINTAG 2					///< Начался тег
#define MS_MIDDLETAG 3					///< Середина тега, но еще до имени
#define MS_TAGNAME 4						///< Парсим имя тега
#define MS_ENDTAGNAME 5					///< Закончили парсить имя тега
#define MS_ATTRIBNAME 6					///< Парсим имя аттрибута
#define MS_ENDATTRIBNAME 7				///< Прочитали имя аттрибута
#define MS_ENDEQUALLY 8					///< Прошли знак = между именем аттритута и значением
#define MS_ATTRIBVALUE 9					///< Парсим значение аттрибута (после ")
#define MS_ENDTAG 10						///< Конец тега (для самозакрывающегося после /)
#define MS_TEXT 11						///< Текст


#define TS_INDEFINITE 1					///< Неопределенный, т.к. тег только начали парсить (если начали)
#define TS_NORMAL 2						///< Обычный (не)закрывающийся. Точно не декларация
#define TS_CLOSE 3						///< Закрывающийся
#define TS_EMPTY 4						///< Сам и закрывается
#define TS_DECLARATION 5					///< Декларация, что это XML. Парсер аттрибуты игнорирует

void* XMLDecode(char *buf, int size);
void SaveTree(XMLNode *tree);
void DestroyTree(XMLNode *tree);

char* XML_Get_Attr_Value(char* req_attr_name, XMLAttr* attr_list);
XMLNode* XML_Get_Child_Node_By_Name(XMLNode* node, char* req_node_name);
#endif
