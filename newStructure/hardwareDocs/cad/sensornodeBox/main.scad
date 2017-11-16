include <variables.scad>
include <dimlines.scad>

use <batteryBox.scad>
use <pcb.scad>
use <pcbBox.scad>
use <pcbTopBox.scad>
use <pcbBottomBox.scad>


// explode(5, true);
// HoledBox(50, 20, 10, holeRadius, holeRadius*2, true);
// pcbBox();

pcbBottomBox();
// pcbTopBox();
// batteryBoxV2();



/* shows all possibilities
        DIM_CENTER = 0;
        DIM_LEFT = 1;
        DIM_RIGHT = 2;
        DIM_OUTSIDE = 3;
*/

length = 2.5;

DIM_LINE_WIDTH=.5;
DIM_HEIGHT=.5;
translate([-42.5, 24.5, 0])
rotate([0, 0, 90])
line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

translate([42.5, 24.5, 0])
rotate([0, 0, 90])
line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

translate([42.5, 24.5, 0])
line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);
translate([42.5, -24.5, 0])
line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

translate([42.5+5, 24.5, 0])
rotate([0, 0, -90])
dimensions(length=49, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);
translate([-42.5, 24.5+5, 0])
dimensions(length=85.0, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);
translate([42.5-20, -24.5+12.5, 0])
rotate([0, 0, 90])
dimensions(length=22.0, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);


module Text(string, position) {
	translate(position)
	translate([3,0,0])
	rotate([90, 0, 0])
	text(string, size=5, valign="center");

	translate(position)
	color("Red")
	cube(1, center=true);
}

module explode(distance, cut) {

	color(alpha=0.5)
	difference() {
		union() {
			translate([0, 0, -distance*2]) {
				translate([-PCBNotchWidth, 0, -(stickRadius+PCBToTopSpacing+PCBComponentsHeight)])
				PCB();
				pcbBottomBox();
			}

			translate([0, 0, -distance*1])
			pcbTopBox();

			scale([1.1, 0.999, 0.999])
			rotate([0, 90, 0])
			cylinder(MaxLength, r=stickRadius, center=true, $fn=40);

			translate([0, 0, distance*1])
			rotate([180, 0, 0])
			batteryBoxV2();
		}
		if(cut) {
			color("White",alpha=0.5)
			translate([-50, -105, -500])
			cube([100,100,1000]);
		}
	}
}
