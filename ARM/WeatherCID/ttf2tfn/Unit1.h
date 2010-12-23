//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TComboBox *ComboBox1;
        TEdit *Edit1;
        TComboBox *ComboBox2;
        TButton *Button1;
        TImage *Image1;
  TEdit *Edit2;
  TButton *Button2;
  TButton *Button3;
  TButton *Button4;
  TOpenDialog *OpenDialog1;
        void __fastcall Button1Click(TObject *Sender);
  void __fastcall ComboBox1Change(TObject *Sender);
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Button4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
