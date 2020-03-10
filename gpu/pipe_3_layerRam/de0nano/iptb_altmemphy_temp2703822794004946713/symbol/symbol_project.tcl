package require ::quartus::project
set project_name tempproject
if [catch {project_open $project_name}] {
        project_new $project_name
}
export_assignments
set_global_assignment -name "VERILOG_FILE" "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp2703822794004946713/sdramController_phy_alt_mem_phy_seq_wrapper.v";
set_global_assignment -name "VHDL_FILE" "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp2703822794004946713/sdramController_phy_alt_mem_phy_seq.vhd";
set_global_assignment -name "VERILOG_FILE" "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp2703822794004946713/sdramController_phy.v";
set_global_assignment -name "VERILOG_FILE" "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp2703822794004946713/sdramController_phy_alt_mem_phy.v";
set_global_assignment -name "VERILOG_FILE" "C:/sparkboxHD/gpu/pipe_3_layerRam/de0nano/iptb_altmemphy_temp2703822794004946713/sdramController_phy_alt_mem_phy_pll.v";
set_global_assignment -name USER_LIBRARIES "C:/intelFPGA_lite/18.0/ip/altera/ddr_high_perf/lib"
set_global_assignment -name "DEVICE" "AUTO";
project_close
