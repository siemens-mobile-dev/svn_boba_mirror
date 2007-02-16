/*
int main(const char *elf_name, const char *fname)
{
  int ff;
  unsigned int ull;
  ff=fopen("4:\\test.xml",A_ReadOnly+A_BIN,0,&ull);
  int size=lseek(ff,0,2,&ull,&ull);
  void *bf=malloc(size);
  lseek(ff,0,0,&ull,&ull);
  fread(ff,bf,size,&ull);
  fclose(ff,&ull);
  extern void *XMLDecode(char *buf, int size);
  XMLNode *data=XMLDecode(bf,size);
  if(data){
    extern void SaveTree(XMLNode *tree);
    extern void DestroyTree(XMLNode *tree);
    SaveTree(data);
    DestroyTree(data);
  }
  return 0;
}
*/
