include <variables.scad>

module batteries() {
	color("Crimson") {
		translate([0, -batteryRadius-.1, 0])
		battery();
		translate([0, batteryRadius+.1, 0])
		battery();
	}
}

module battery() {
	rotate([0, 90, 0])
	cylinder(batteryLenght, r=batteryRadius, center=true, $fn=40);
}