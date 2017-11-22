include <variables.scad>

translate([0, -10, 0])
switch();
translate([0, 10, 0])
switch_alt();

module switch() {
	translate([-SwitchCylinderLength, 0, 0]) {
		translate([-SwitchBaseLength/2, 0, 0]) {
			color("Blue")
			cube([SwitchBaseLength, SwitchBaseWidth, SwitchBaseHeight], center=true);
			translate([-6, 0, 0])
			cube([SwitchLegLength, 0.75, 2], center=true);
			translate([-6, 4, 0])
			cube([SwitchLegLength, 0.75, 2], center=true);
			translate([-6, -4, 0])
			cube([SwitchLegLength, 0.75, 2], center=true);
		}
		rotate([0, 90, 0]) {
			cylinder(SwitchCylinderLength, r=SwitchCylinderRadius, center=false, $fn=20);
			translate([0, 0, SwitchCylinderLength])
			cylinder(10, r1=2.5/2, r2=3/2, center=false, $fn=20);
		}
	}
}

module switch_alt() {

	// SwitchAltWidth = 13;
	// SwitchAltHeight = 6;
	// SwitchAltDepth = 6;
	// SwitchPinLength = 4;
	// SwitchAltFlipperHeight = 7;
	cube([SwitchAltDepth,SwitchAltWidth, SwitchAltHeight], center=true);
}