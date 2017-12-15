include <variables.scad>
use <pcb.scad>
use <box.scad>

pcbBox();

module pcbBox() {
	difference() {
		CylinderPunchedBox(MaxLength, MinWidth, MinHeight, holeRadius, holeRadius*2);
		
		// Hollow out inside box space
		CutoutHeight = stickRadius+(PCBBoardToBoardHeight+PCBComponentsHeight)+PCBToBottomSpacing+PCBToTopSpacing;
		scale([1.001, 1.01, 1])
		translate([0, 0, -(CutoutHeight)/2])
		cube([PCBLength, PCBWidth-PCBNotchWidth*2, CutoutHeight], center=true);
		
		// PCB Notches cutout
		translate([-PCBNotchWidth, 0, -(stickRadius+PCBToTopSpacing+PCBComponentsHeight)])
		PCB();		

		// LED hole
		ledSmallRadius = 2;
		ledLargeRadius = 3;
		ledSmallHoleLength = 5;
		ledLargeHoleLength = ((MinWidth-(PCBWidth-PCBNotchWidth*2))/2)-ledSmallHoleLength;
		color("Green")
		translate([-PCBBoardLength/2+5, MinWidth/2, -(stickRadius)])
		rotate([90, 0, 0]){
			scale([1, 1, 1.01])
			translate([0, 0, -0.01])
			cylinder(ledSmallHoleLength, r=ledSmallRadius, center=false, $fn=20);
			translate([0, 0, ledSmallHoleLength])
			cylinder(ledLargeHoleLength, r=ledLargeRadius, center=false, $fn=20);
		}
	}
}