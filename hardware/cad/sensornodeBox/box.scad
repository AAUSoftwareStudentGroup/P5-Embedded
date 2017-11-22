include <variables.scad>
use <bolt.scad>

CylinderPunchedBox(MaxLength, MinWidth, stickRadius-boxSpacing/2+batteryRadius*2+minEdgeWidth, holeRadius, holeRadius*2, true);

module HoledBox(length, width, height, holeRadius, holePadding, NutIndents) {
	difference() {
		cube([length, width, height], center = true);
		
		scale([1,1,1.001])
		union() { // subtractions
			// holes
			translate([
				 (length-holePadding-holeRadius*2)/2, 
				 (width-holePadding-holeRadius*2)/2, 
				-(height)/2])
			cylinder(height+.1, r=holeRadius, $fn=16);
			translate([
				-(length-holePadding-holeRadius*2)/2, 
				 (width-holePadding-holeRadius*2)/2, 
				-(height)/2])
			cylinder(height+.1, r=holeRadius, $fn=16);
			translate([
				 (length-holePadding-holeRadius*2)/2, 
				-(width-holePadding-holeRadius*2)/2, 
				-(height)/2])
			cylinder(height+.1, r=holeRadius, $fn=16);
			translate([
				-(length-holePadding-holeRadius*2)/2, 
				-(width-holePadding-holeRadius*2)/2, 
				-(height)/2])
			cylinder(height+.1, r=holeRadius, $fn=16);
		}

		if(NutIndents) {
			translate([0, 0, -(height-boltHeight+0.001)/2]) {
				translate([(length-holePadding-holeRadius*2)/2, (width-holePadding-holeRadius*2)/2, ])
				scale([1.05, 1.05, 1.1])
				bolt();
				translate([(length-holePadding-holeRadius*2)/2, -(width-holePadding-holeRadius*2)/2, ])
				scale([1.05, 1.05, 1.1])
				bolt();
				translate([-(length-holePadding-holeRadius*2)/2, (width-holePadding-holeRadius*2)/2, ])
				scale([1.05, 1.05, 1.1])
				bolt();
				translate([-(length-holePadding-holeRadius*2)/2, -(width-holePadding-holeRadius*2)/2, ])
				scale([1.05, 1.05, 1.1])
				bolt();
			}
		}
	}
}

module CylinderPunchedBox(length, width, height, holeRadius, holePadding, NutIndents) {
	difference() {
		translate([0, 0, -(height+boxSpacing)/2])
		HoledBox(length, width, height, holeRadius, holePadding, NutIndents);

		scale([1.001, 1, 1])
		rotate([0, 90, 0])
		cylinder((length), r=stickRadius, center=true, $fn=40);
	}
}