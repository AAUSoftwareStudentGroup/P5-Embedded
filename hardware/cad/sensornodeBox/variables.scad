innerWidth = 38;
innerLength = 80;
innerHeight = 25;
minEdgeWidth = 2.5;
largeEdgeWidth = 6;
holeRadius = 2;
boxSpacing = 4;
stickRadius = 7.1; // fixed by table

sensorScrewHoleRadius = 2.5/2;

// PCBVariables
	PCBWidth = 35.2;
	PCBBoardLength = 70;
	PCBLength = 80;// 70;
	PCBHeight = 1.5;
	PCBBoardToBoardHeight = 13.9; // height of the two layers of pcb and the pinheaders between them
	
	PCBToBottomSpacing = 3; // space for wires on bottom of PCB
	PCBToTopSpacing = 2; // Air gap between pcb components and stick
	PCBNotchWidth = 1.5; // user definable
	PCBComponentsHeight = 17.1-PCBBoardToBoardHeight; // Height of pCB components on top of top layer pcb

	PCBSensorBoardLength = 15.75;
	PCBSensorBoardWidth = 21;
	PCBSensorBoardOverhang = 7.4; // The length of sensor board that overhangs the main print board
	PCBSensorBoardYOffset = -1.5/2;
	PCBSensorBoardHoleRadius = 1.5;
	PCBSensorBoardHoleCenterYDistance = 12.3+PCBSensorBoardHoleRadius*2;

// BatteryBoxVariables
	batteryRadius = 18.3/2;
	batteryLenght = 65;

// Bolts
	boltHeight = 2.4;
	boltWidth = 5.4;
	boltSegmentWidth = 3.2;

// Switch
	SwitchBaseLength = 9.8;
	SwitchBaseWidth = 13.1;
	SwitchBaseHeight = 8;

	SwitchCylinderRadius = 6/2;
	SwitchCylinderLength = 9;
	SwitchLegLength = 4;

// switch_alt
	SwitchAltWidth = 13;
	SwitchAltHeight = 6;
	SwitchAltDepth = 6;
	SwitchPinLength = 4;
	SwitchAltFlipperHeight = 7;


MaxLength = 85; // fixed by table
MinWidth = max(batteryRadius*4, PCBWidth)+2*largeEdgeWidth;
MinHeight = max(batteryRadius*2, PCBToTopSpacing+PCBComponentsHeight+PCBBoardToBoardHeight+PCBToBottomSpacing)+stickRadius+minEdgeWidth;
