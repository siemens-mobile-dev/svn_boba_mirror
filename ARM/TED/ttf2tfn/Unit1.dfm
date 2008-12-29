object Form1: TForm1
  Left = 275
  Top = 188
  BorderStyle = bsSingle
  Caption = 'TTF 2 TFN'
  ClientHeight = 423
  ClientWidth = 592
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 41
    Width = 592
    Height = 382
    Align = alClient
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 592
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object ComboBox1: TComboBox
      Left = 8
      Top = 8
      Width = 329
      Height = 21
      ItemHeight = 13
      TabOrder = 0
      Text = 'MS Sanf Serif'
    end
    object Edit1: TEdit
      Left = 344
      Top = 8
      Width = 25
      Height = 21
      TabOrder = 1
      Text = '16'
    end
    object ComboBox2: TComboBox
      Left = 376
      Top = 8
      Width = 65
      Height = 21
      ItemHeight = 13
      TabOrder = 2
      Text = 'Normal'
    end
    object Button1: TButton
      Left = 448
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Convert'
      Default = True
      TabOrder = 3
      OnClick = Button1Click
    end
  end
end
