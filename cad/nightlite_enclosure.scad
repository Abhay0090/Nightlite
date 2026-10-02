// NightLite Enclosure - OpenSCAD Model

$fn = 60; // High resolution for smooth circles

width = 80;
depth = 65;
base_height = 70;
diffuser_height = 40;
wall = 2.0;

module base_shell() {
    difference() {
        // Main body
        union() {
            // Base cube with rounded corners
            minkowski() {
                cube([width - 4, depth - 4, base_height - 1], center=false);
                cylinder(r=2, h=1);
            }
            // Lip for diffuser to sit on (friction fit)
            translate([wall, wall, base_height]) {
                cube([width - wall*2, depth - wall*2, 3]);
            }
        }
        
        // Hollow interior
        translate([wall, wall, wall]) {
            cube([width - wall*2, depth - wall*2, base_height + 5]);
        }
        
        // OLED Window (Front face)
        translate([26.5, -5, 30]) {
            cube([27.0, 10, 15.0]);
        }
        
        // Buttons (Front face)
        translate([20, -5, 20]) rotate([-90, 0, 0]) cylinder(h=10, d=7);
        translate([40, -5, 20]) rotate([-90, 0, 0]) cylinder(h=10, d=7);
        translate([60, -5, 20]) rotate([-90, 0, 0]) cylinder(h=10, d=7);
        
        // PIR Sensor Dome (Right face)
        translate([width - 5, depth/2, 55]) rotate([0, 90, 0]) cylinder(h=10, d=24);
        
        // USB Slot (Back face)
        translate([width/2 - 6, depth - 5, 2]) {
            cube([12, 10, 8]);
        }
        
        // LDR Hole (Top platform - left side)
        // Creating a small platform for the LDR to peek through
        translate([15, depth/2, base_height - 5]) {
            cylinder(h=10, d=5);
        }
    }
    
    // Stand-offs for ESP32
    // ESP32 DevKit V1: holes are approx 45.4 x 22.8 mm apart
    // Positioned near the back
    standoff_x = width/2 - 45.4/2;
    standoff_y = depth - 5 - 22.8;
    
    translate([standoff_x, standoff_y, wall]) standoff();
    translate([standoff_x + 45.4, standoff_y, wall]) standoff();
    translate([standoff_x, standoff_y + 22.8, wall]) standoff();
    translate([standoff_x + 45.4, standoff_y + 22.8, wall]) standoff();
}

module standoff() {
    difference() {
        cylinder(h=6, d=5);
        cylinder(h=7, d=2.1); // M2 screw hole
    }
}

module diffuser_cap() {
    difference() {
        // Main body
        minkowski() {
            cube([width - 4, depth - 4, diffuser_height - 1], center=false);
            cylinder(r=2, h=1);
        }
        
        // Hollow interior
        translate([wall, wall, -1]) {
            cube([width - wall*2, depth - wall*2, diffuser_height - wall + 1]);
        }
        
        // Lip cutout to fit over base shell
        translate([wall, wall, -1]) {
            cube([width - wall*2, depth - wall*2, 4]);
        }
    }
}

// Render options
part = "both"; // Change to "base" or "diffuser" when exporting

if (part == "base" || part == "both") {
    color("gray") base_shell();
}

if (part == "diffuser" || part == "both") {
    translate([0, 0, part == "both" ? base_height + 10 : 0])
    color("white", 0.5) diffuser_cap();
}
