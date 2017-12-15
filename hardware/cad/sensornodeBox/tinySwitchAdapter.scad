innerHeight =  3.75;
innerWidth  =  8.55;
outerWidth  = 13.00;
outerHeight =  5.75;

depth = 2.6;
// translate([32, 0, 0])

rotate([0, 90, 0])
difference() {
	cube([depth, outerWidth, outerHeight], center=true);
	scale([1.1, 1, 1])
	cube([depth, innerWidth, innerHeight], center=true);
}