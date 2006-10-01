extern void PlayTone(int id); // 0 == stop playing
/*
00 = nothing, but stops all running sounds 
01 = network found 
02 = high beep (e.g. starting dictating machine) 
03 = battery empty 
04 = key click 
05 = low beep 
06 = calling sound (endless) 
07 = melody classic nr. 12 (profile independent) (endless) 

These sounds are very quiet, you have loudness like in calls, 
only listenable if your ear is at the phone speaker: 
08 = connection active beep (5 seconds long) 
09 = 5x long low beep 
0A = 0Bh 5x 
0B = like 0Ch, but only 1x 
0C = busy signal (3x) 
0D = connection lost (3x doo-da-dit) 
0E = dood-dood with 1sec pause, endless 
0F-10h = nothing 
11 = ringing sound (endless) 
12 = ringing sound (endless) but starts a second later 
13 = call minutes beep 

These sounds have normal loudness again: 
14 = calling sound (endless) (from active profile) 
15 = 07h 
16-17h = nothing 
18 - 24h = some standard dialing tones 
25 = call minutes beep (loud) 
26 = triple low beep (05h) 
27 = triple call minutes beep (loud) 
28 = waving sirene (endless) 
29 = nothing 
2a = connecting sound (doo-de-li-dood) 
2b = phone hangs up 
2c = 25h 
2d = calling sound (endless) 
2e = calling sound (endless) 
2f = vibrating (endless) 
30 = long high beep, very loud 
31 = appointment sound (endless) (from active profile) 
32 = appointment sound (endless) (from active profile) 
33 = low beep (05h) 
34 = sms-sound (from active profile) 
35 = sms-sound (from active profile) 
36 = CB-sound (from active profile) 
37 = CB-Sound (from active profile) 

these following sounds are interesting: 
38 = high (and loud) da-dit 
39 = like 38h, but doubled 
3a = police sirene (3x) 
3b = 4x very high and loud beep 
3c = high fanfare 
3d = like high beep (02h) 
3e = like high beep (02h), but longer 
3f = double 02h 
40 = nothing 
41 = beep 1x (very quiet) 
42 = cool and short *dring*, nice 
43 = phone hangs up 
44 = phone hangs up 
45 = phone hangs up 
46 = phone hangs up 
47-... = didn't test it 
*/