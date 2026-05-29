Pin layout oben:
2 (36 unten) VCC 24v
14, 15 Vcc 5v
4, 5 Ground
16 Ground 5v
9: AEN+: Motor A => MY-CLK
6: AEN+: Motor B => MX-CLK
PUL+: Motor A => MX-DIR
PUL+: Motor B => MY-DIR

Pin layout unten:
1-4 DC +
5 Leer
6-10 DC -
11-14 ?
15-16 PE
18 BEnable+ | Nr. 1
19,21,23,27,29 Ground
	19 Ground einpeisung
20 (BPull+),
22(BDir+),
24(AEN+),
25(AEN-),
26(APLU+),
28(ADIR+)
30-35 ?
36-39 Vcc 24V
	36 - 37 Vcc einspeisung
40-49 ?
42 Motor B A+
43 Motor B A-
44 Motor B B+
45 Motor B B- (Schrott)
46-47 Motor A A- und A+ (Schrott)
48-49 Motor A B+/B-


