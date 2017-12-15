include <variables.scad>
include <dimlines.scad>

use <batteryBox.scad>
use <pcb.scad>
use <pcbBox.scad>
use <pcbTopBox.scad>
use <pcbBottomBox.scad>


explode(10, false);
// HoledBox(50, 20, 10, holeRadius, holeRadius*2, true);
// pcbBox();

// translate([0,-60,5])
// translate([0,-60,12])
// pcbBottomBox();
// translate([0,0,-4])
// pcbTopBox();
// translate([0,60,0])
// batteryBoxV2();


// length = 2.5;

// DIM_LINE_WIDTH=.5;
// DIM_HEIGHT=.5;
// translate([-42.5, 24.5, 0])
// rotate([0, 0, 90])
// line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

// translate([42.5, 24.5, 0])
// rotate([0, 0, 90])
// line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

// translate([42.5, 24.5, 0])
// line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);
// translate([42.5, -24.5, 0])
// line(length=20, width=DIM_LINE_WIDTH, height=DIM_LINE_WIDTH);

// translate([42.5+5, 24.5, 0])
// rotate([0, 0, -90])
// dimensions(length=49, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);
// translate([-42.5, 24.5+5, 0])
// dimensions(length=85.0, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);
// translate([42.5-20, -24.5+12.5, 0])
// rotate([0, 0, 90])
// dimensions(length=22.0, width=DIM_LINE_WIDTH*10, height=DIM_LINE_WIDTH*10);


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

	color(alpha=1)
	difference() {
		union() {
			translate([0, 0, -distance*2]) {
				// translate([-PCBNotchWidth, 0, -(stickRadius+PCBToTopSpacing+PCBComponentsHeight)])
				// PCB();
				pcbBottomBox();
			}

			translate([0, 0, -distance*1])
			pcbTopBox();

			// scale([1.1, 0.999, 0.999])
			// rotate([0, 90, 0])
			// cylinder(MaxLength*2, r=stickRadius, center=true, $fn=40);

			translate([0, 0, distance*0])
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
