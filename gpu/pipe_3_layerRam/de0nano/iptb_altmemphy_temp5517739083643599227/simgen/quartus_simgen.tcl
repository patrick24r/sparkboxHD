package require ::quartus::project
set project_name sdramController_phy_alt_mem_phy_seq_wrapper
if [catch {project_open $project_name}] {
        project_new $project_name
}
set_global_assignment -name "FAMILY" "Cyclone IV E"
export_assignments
set_global_assignment -name "COMPILER_SETTINGS" "sdramController_phy_alt_mem_phy_seq_wrapper"
set_global_assignment -name "SIMULATOR_SETTINGS" "sdramController_phy_alt_mem_phy_seq_wrapper"
set_global_assignment -name VERILOG_FILE "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp5517739083643599227/sdramController_phy_alt_mem_phy_seq_wrapper.v"
set_global_assignment -name VHDL_FILE "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp5517739083643599227/sdramController_phy_alt_mem_phy_seq.vhd"
set_global_assignment -name VERILOG_FILE "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp5517739083643599227/sdramController_phy.v"
set_global_assignment -name VERILOG_FILE "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp5517739083643599227/sdramController_phy_alt_mem_phy.v"
set_global_assignment -name VERILOG_FILE "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp5517739083643599227/sdramController_phy_alt_mem_phy_pll.v"
set_global_assignment -name USER_LIBRARIES "C:/intelFPGA_lite/18.0/ip/altera/ddr_high_perf/lib"
set_global_assignment -name "STRATIX_OPTIMIZATION_TECHNIQUE" "SPEED"
set_global_assignment -name "DEVICE" "AUTO";
project_close
