#include <./BOSL2/std.scad>

diff("neg")
cuboid([20, 20, 17], anchor=BOTTOM)
union(){
position(BOTTOM)
cyl(d=8, h=4, anchor=TOP);
attach(TOP)
up(0.1)
tag("neg")
cyl(d=8, h=4, anchor=TOP);
}
