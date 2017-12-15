include <variables.scad>
use <battery.scad>
use <box.scad>
use <switch.scad>

batteryBoxV2();

module batteryBoxV2() {
	// color(alpha=.4)

	// translate([0, 0, -(batteryRadius+stickRadius/2)])
	// batteries();

	// insetDepth = 7.5;
	insetDepth = 8;
	difference() {
		difference() {
			// Minimum height box
			CylinderPunchedBox(MaxLength, MinWidth, stickRadius-boxSpacing/2+batteryRadius*2+minEdgeWidth, holeRadius, holeRadius*2, true);
			// Symetric height box
			// CylinderPunchedBox(MaxLength, MinWidth, MinHeight, holeRadius, holeRadius*2);
			
			
			color("Red")
			// translate([30, 0, 0])
			translate([-insetDepth/2, 0, -(stickRadius+batteryRadius)])
			scale([0.999, 1.04, 1.04]) {
				translate([-batteryLenght/2, (batteryRadius+.5), 0])
				rotate([0, 90, 0])
				cylinder(h=batteryLenght+insetDepth, r=batteryRadius, $fn=100);
				translate([-batteryLenght/2, -(batteryRadius+.5), 0])
				rotate([0, 90, 0])
				cylinder(h=batteryLenght+insetDepth, r=batteryRadius, $fn=100);
			}
		}
		union() {
			// big access openning
			translate([0, 0, -(batteryRadius+stickRadius)/2])
			scale([1, 1, 1.001])
			cube([batteryLenght+insetDepth, batteryRadius*4-2, batteryRadius+stickRadius], center=true);

			// inset cable tunnel
			color("Green")
			translate([0, 0, batteryRadius+stickRadius/2]) {
				translate([(batteryLenght+insetDepth)/2, 0, -(stickRadius+batteryRadius*2)+1])
				cube([insetDepth,batteryRadius, batteryRadius*2+stickRadius], center = true);
				translate([-(batteryLenght+insetDepth)/2, 0, -(stickRadius+batteryRadius*2)+1])
				cube([insetDepth,batteryRadius, batteryRadius*2+stickRadius], center = true);
			}
			
			color("Black")
			translate([MaxLength/2-SwitchAltDepth/2, 0, -(stickRadius+SwitchAltHeight/2+3)])
			scale(1.025)
			switch_alt();
		}
	}
}
