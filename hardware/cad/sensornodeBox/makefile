current_dir = $(shell pwd)/

parts = pcbBottomBox pcbTopBox batteryBox
slicingProfile = curaSlicerProfiles/hipsCoarseProfile.ini

all: prepare $(parts)
	@echo "done"

prepare:
	@echo "current dir $(current_dir)"
	@echo "Preparing"
	mkdir -p gcode stl

$(parts):
	@echo "Compiling $@.scad"
	openscad -o $(current_dir)stl/$@.stl $(current_dir)$@.scad
	@echo "Slicing $@.stl"
	cura -s -i $(current_dir)${slicingProfile} -o $(current_dir)gcode/$@.gcode $(current_dir)stl/$@.stl
	@echo "completed $@.gcode"