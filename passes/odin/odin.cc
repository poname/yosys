#include "kernel/yosys.h"

#include "libs/ODIN_II/YosysHelper.h"

// #include "libs/ODIN_II/odin_ii.h"
// #include "libs/ODIN_II/odin_types.h"
// #include "libs/ODIN_II/Verilog.hpp"
// #include "libs/ODIN_II/config_t.h"    // configuration
// #include "libs/ODIN_II/odin_util.h"   // get_directory
// #include "libs/ODIN_II/odin_error.h"  // error_message
// #include "libs/ODIN_II/hard_blocks.h" // hard_block_names

#include "libs/libvtrutil/vtr_util.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct OdinPass : public Pass {
	OdinPass() : Pass("odin", "ODIN_II Partial Mapper for Yosys :)") { }
	void help() override
	{
		log("ODIN_II help will be written here\n");
	}
	void execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		log_header(design, "Executing ODIN_II pass.\n");

		/* pass arguments here to odin */
    	std::vector<std::string> arguments = 
		{
			"libs/ODIN_II/odin_ii", 							//	pass the odin location
        	// "-a", "4bit_adder_double_chain_arch.xml",		//	pass the arch file
			"-b", "passes/odin/temp.blif",								//	pass the input blif file
			"-o", "passes/odin/mapped.blif",								//	pass the output blif file
			"--coarsen", "--fflegalize"
    	};

		size_t argidx;
		for (argidx = 1; argidx < args.size(); argidx++)
		{

			if (args[argidx] == "-a" && argidx+1 < args.size()) {
				arguments.push_back("-a");
				arguments.push_back(args[++argidx]);
				continue;
			}

			if (args[argidx] == "-b" && argidx+1 < args.size()) {
				arguments.push_back("-b");
				arguments.push_back(args[++argidx]);
				continue;
			}

			if (args[argidx] == "-o" && argidx+1 < args.size()) {
				arguments.push_back("-o");
				arguments.push_back(args[++argidx]);
				continue;
			}

		}
		extra_args(args, argidx, design);

		char **arg_list;
    	int arg_len;

    	/* converting into format digestible by odin */
	    arg_list = (char**)malloc(arguments.size()*sizeof(char*));
		arg_len = arguments.size();

		for( int i=0; i<arg_len; i ++)
	    {
   	    	arg_list[i] = vtr::strdup(arguments[i].c_str());
    	}

		warmup_odin_ii(arg_len, arg_list);

		Pass::call(design, "read_verilog -nomem2reg misc/vtr_flow/primitives.v");
		Pass::call(design, "setattr -mod -set keep_hierarchy 1 single_port_ram");
		Pass::call(design, "setattr -mod -set keep_hierarchy 1 dual_port_ram");

		handle_dsp_blocks();

		Pass::call(design, "read_verilog -nomem2reg passes/odin/arch_dsp.v");
		// Pass::call(design, "read_verilog -nomem2reg passes/odin/arch_dsp.v");

		Pass::call(design, "read_verilog -sv -nolatches ../vtr-verilog-to-routing/ODIN_II/regression_test/benchmark/verilog/large/des_area.v");
		// Pass::call(design, "read_verilog -sv -nolatches ../my/arm_core.v");

		Pass::call(design, "hierarchy -check -auto-top -purge_lib");

		Pass::call(design, "autoname");

		Pass::call(design, "proc -norom; opt;");
		// Pass::call(design, "proc -norom");

		Pass::call(design, "fsm; opt;");

		Pass::call(design, "memory_collect; memory_dff -no-rw-check; opt;");

		Pass::call(design, "check");

		Pass::call(design, "techmap -map libs/ODIN_II/techlib/adff2dff.v");
		Pass::call(design, "techmap -map libs/ODIN_II/techlib/adffe2dff.v");

		Pass::call(design, "techmap */t:$shift */t:$shiftx");

		// Pass::call(design, "memory_bram -rules libs/ODIN_II/techlib/mem_rules.txt");
		// Pass::call(design, "techmap -map libs/ODIN_II/techlib/mem_map.v");

		Pass::call(design, "flatten");

		Pass::call(design, "pmuxtree");

		Pass::call(design, "wreduce");

		Pass::call(design, "opt -undriven -full; opt_muxtree; opt_expr -mux_undef -mux_bool -fine;;;");

		Pass::call(design, "autoname");

		Pass::call(design, "stat");

		Pass::call(design, "ls");

		Pass::call(design, "write_blif -blackbox -param -impltf passes/odin/temp.blif");

		//set_default_config();
		//prepare_odin();

		// Pass::call(design, "read_verilog -nomem2reg misc/vtr_flow/primitives.v");

//		Pass::call(design, "setattr -mod -set keep_hierarchy 1 " + std::string(SINGLE_PORT_RAM_string));
//		Pass::call(design, "setattr -mod -set keep_hierarchy 1 " + std::string(DUAL_PORT_RAM_string));

//		Verilog::Writer vw = Verilog::Writer();
    	// vw._create_file(configuration.dsp_verilog.c_str());
		//vw._create_file("passes/odin/arch_dsp.v");

		// t_arch arch;
    	// t_model* hb = Arch.models;
    	// while (hb) {
        // 	// declare hardblocks in a verilog file
        // 	if (strcmp(hb->name, SINGLE_PORT_RAM_string) && strcmp(hb->name, DUAL_PORT_RAM_string) && strcmp(hb->name, "multiply") && strcmp(hb->name, "adder"))
        //     	vw.declare_blackbox(hb->name);

        // 	hb = hb->next;
    	// }
		//do_verilog();
    	// vw._write(NULL);

		
    	// Pass::call(design, "read_verilog -nomem2reg " + configuration.dsp_verilog);
		//Pass::call(design, "read_verilog -nomem2reg passes/odin/arch_dsp.v");

		netlist_t* odin_netlist = continue_odin_ii();
		terminate_odin_ii(odin_netlist);

		// netlist_t* odin_netlist = start_odin_ii(arg_len, arg_list);
   		// terminate_odin_ii(odin_netlist);

		Pass::call(design, "design -reset");
		Pass::call(design, "read_blif -sop passes/odin/mapped.blif");

		log("ODIN_II pass finished.\n");
	}
} OdinPass;

PRIVATE_NAMESPACE_END