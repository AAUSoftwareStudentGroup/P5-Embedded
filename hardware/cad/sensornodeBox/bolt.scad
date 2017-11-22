include <variables.scad>

bolt();

module bolt() {
	color("Silver")
	union() {
		for (i = [0:2]) {
			rotate([0, 0, 180/3*i])
			cube([boltWidth, boltSegmentWidth, boltHeight],center=true);
		}
	}
}