include <variables.scad>

PCB();

module PCB() {
	pinheaderHeight = PCBBoardToBoardHeight-2*PCBHeight;
	pinheaderLength = 38;
	pinheaderWidth = 2.4;
  color(alpha=0.5)
	translate([-(PCBLength-PCBBoardLength), 0, -(PCBBoardToBoardHeight)]) {
		color("Orange")
		translate([(PCBLength-PCBBoardLength)/2, 0, PCBHeight/2])
		cube([PCBBoardLength, PCBWidth, PCBHeight], center=true);
		
		translate([0, 0, PCBHeight]) {
			color("Grey") {
				translate([0, (PCBWidth-pinheaderWidth)/2-PCBNotchWidth, pinheaderHeight/2])
				cube([pinheaderLength, pinheaderWidth, pinheaderHeight], center=true);
				translate([0, -(PCBWidth-pinheaderWidth)/2+PCBNotchWidth, pinheaderHeight/2])
				cube([pinheaderLength, pinheaderWidth, pinheaderHeight], center=true);
			}
			color("Black")
			translate([0, 0, pinheaderHeight+PCBHeight/2])
			cube([55, 32.5, PCBHeight], center=true);

			color("Blue")
			translate([-PCBSensorBoardOverhang, PCBSensorBoardYOffset, 0])
			translate([PCBLength/2, 0, 0])
			translate([PCBSensorBoardLength/2, 0, pinheaderHeight+PCBHeight/2])
			difference() {
				cube([PCBSensorBoardLength, PCBSensorBoardWidth, PCBHeight], center=true);
				translate([PCBSensorBoardLength/2-1-PCBSensorBoardHoleRadius,  (PCBSensorBoardHoleCenterYDistance/2), 0])
				cylinder(10, r=PCBSensorBoardHoleRadius, center=true, $fn=20);
				translate([PCBSensorBoardLength/2-1-PCBSensorBoardHoleRadius, -(PCBSensorBoardHoleCenterYDistance/2), 0])
				cylinder(10, r=PCBSensorBoardHoleRadius, center=true, $fn=20);
			}
			
			color("Grey")
			translate([34, 0, pinheaderHeight/2])
			cube([pinheaderWidth, 20, pinheaderHeight], center=true);
		}
	}
}