include <variables.scad>
use <pcbBox.scad>

pcbBottomBox();

module pcbBottomBox() {
	union() {
		cutoffHeight = stickRadius+PCBToTopSpacing+PCBBoardToBoardHeight+PCBComponentsHeight-PCBHeight;
		difference() {
			pcbBox();
			
			// cutoff the top		
			translate([0, 0, -(cutoffHeight)/2])
			scale([1.001, 1.001, 1.001])
			cube([MaxLength, MinWidth, cutoffHeight], center=true);
		}
		// Sensor board support block
		{
			platformHeight = PCBToBottomSpacing+PCBBoardToBoardHeight-PCBHeight;
			platformLenght = PCBSensorBoardLength-PCBSensorBoardOverhang;
			platformWidth = PCBSensorBoardWidth*1.1;

			translate([0, 0, (platformHeight)/2])
			translate([0, 0, -(MinHeight-minEdgeWidth)])
			translate([-(PCBLength-PCBBoardLength+PCBNotchWidth), 0, 0])
			translate([(platformLenght/2), 0, 0])
			translate([PCBLength/2, 0, 0])
			translate([0, PCBSensorBoardYOffset, 0])
			scale([1.01, 1, 1])

			translate([1, 0, 0])
			difference() {
				cube([platformLenght, 22, platformHeight], center=true);

				translate([0, PCBSensorBoardHoleCenterYDistance/2, 3+2])
				cylinder(h=11, r=PCBSensorBoardHoleRadius, center=true, $fn=20);
				translate([0, -PCBSensorBoardHoleCenterYDistance/2, 3+2])
				cylinder(h=11, r=PCBSensorBoardHoleRadius, center=true, $fn=20);
			}
		}
	}

	// translate([-PCBNotchWidth, 0, -(stickRadius+PCBToTopSpacing+PCBComponentsHeight)])
	// PCB();
}