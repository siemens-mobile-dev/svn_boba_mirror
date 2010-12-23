object Form1: TForm1
  Left = 269
  Top = 118
  BorderStyle = bsSingle
  Caption = 'TTF 2 TFN'
  ClientHeight = 535
  ClientWidth = 703
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
    Top = 0
    Width = 703
    Height = 470
    Align = alClient
  end
  object Panel1: TPanel
    Left = 0
    Top = 470
    Width = 703
    Height = 65
    Align = alBottom
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
      OnChange = ComboBox1Change
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
    object Edit2: TEdit
      Left = 8
      Top = 40
      Width = 153
      Height = 21
      MaxLength = 15
      TabOrder = 4
    end
    object Button2: TButton
      Left = 224
      Top = 32
      Width = 75
      Height = 25
      Caption = 'ShowTTF'
      TabOrder = 5
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 304
      Top = 32
      Width = 75
      Height = 25
      Caption = 'ShowCFNT'
      TabOrder = 6
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 448
      Top = 32
      Width = 75
      Height = 25
      Caption = 'Create CFNT'
      TabOrder = 7
      OnClick = Button4Click
    end
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = '*.cfnt'
    Filter = '*.cfnt|*.cfnt|*.*|*.*'
    Left = 8
    Top = 8
  end
end
