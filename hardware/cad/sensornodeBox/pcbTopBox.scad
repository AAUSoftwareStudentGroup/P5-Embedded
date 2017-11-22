include <variables.scad>
use <pcbBox.scad>

pcbTopBox();

module pcbTopBox() {
	cutoffHeight = MinHeight-(PCBBoardToBoardHeight+PCBComponentsHeight);
	difference() {
		pcbBox();
		
		translate([0, 0, -(cutoffHeight)/2])
		translate([0, 0, -(stickRadius+PCBToTopSpacing+PCBComponentsHeight+PCBBoardToBoardHeight-PCBHeight)])
		scale([1.001, 1.001, 1.001])
		cube([MaxLength, MinWidth, cutoffHeight], center=true);
	}
}
