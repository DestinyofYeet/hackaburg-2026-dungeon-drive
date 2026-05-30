#include <./BOSL2/std.scad>

diff("neg")
cuboid([40, 40, 5])
attach(TOP)
cuboid([20, 20, 145], anchor=BOTTOM)
attach(TOP)
up(0.1)
tag("neg")
// cyl(r=5, h=4, anchor=TOP);
cyl(d=8, h=4, anchor=TOP);
