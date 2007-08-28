@echo off
SET zipprog=..\_SVN_Utility\zip.exe

copy alarm_ring\SGOLD\Exe\alarm_ring.elf alarm_ring.elf
copy alarm_ring\SGOLD_X75\Exe\alarm_ring.elf alarm_ring_x75.elf
copy alarm_cfg_editor\SGOLD\Exe\alarm_cfg_editor.elf alarm_cfg_editor.elf
copy alarm_cfg_editor\SL65\Exe\alarm_cfg_editor.elf alarm_cfg_editor_sl65.elf
copy new_alarm\SGOLD\Exe\new_alarm.elf new_alarm.elf
%zipprog% -j sgold.zip alarm_ring.elf
%zipprog% -j sgold.zip alarm_ring_x75.elf
%zipprog% -j sgold.zip alarm_cfg_editor.elf
%zipprog% -j sgold.zip alarm_cfg_editor_sl65.elf
%zipprog% -j sgold.zip new_alarm.elf

copy alarm_ring\NEWSGOLD\Exe\alarm_ring.elf alarm_ring.elf
copy alarm_cfg_editor\NEWSGOLD\Exe\alarm_cfg_editor.elf alarm_cfg_editor.elf
copy new_alarm\NEWSGOLD\Exe\new_alarm.elf new_alarm.elf
%zipprog% -j newsgold.zip alarm_ring.elf
%zipprog% -j newsgold.zip alarm_cfg_editor.elf
%zipprog% -j newsgold.zip new_alarm.elf

del alarm_ring.elf
del alarm_cfg_editor.elf
del alarm_cfg_editor_sl65.elf
del new_alarm.elf