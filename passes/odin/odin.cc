#include "kernel/yosys.h"

#include "libs/ODIN_II/odin_ii.h"
#include "libs/ODIN_II/odin_types.h"

#include "libs/libvtrutil/vtr_util.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct OdinPass : public Pass {
	OdinPass() : Pass("odin", "ODIN_II Partial Mapper for Yosys :)") { }
	void help() override
	{
		log("\n");
	}
	void execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		log_header(design, "Executing FAKE pass (faking...).\n");

		/* pass arguments here to odin */
    	std::vector<std::string> arguments = 
		{
			"libs/ODIN_II/odin_ii", 							//	pass the odin location
        	// "-a", "/home/casa/Desktop/poname/my/4bit_adder_double_chain_arch.xml",											//	pass the arch file
			// "-b", "/home/casa/Desktop/poname/my/simple.blif",		//	pass the input blif file
			// "-o", "/home/casa/Desktop/poname/my/mapped.blif",		//	pass the output blif file
			"--coarsen"
    	};

		std::string xml_arch_addr = "";

		size_t argidx;
		for (argidx = 1; argidx < args.size(); argidx++)
		{
			// log(args[argidx].c_str());
			// if (args[argidx] == "-check") {
			// 	flag_check = true;
			// 	continue;
			// }

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

			// if (args[argidx] == "-map" && argidx+1 < args.size()) {
			// 	map_files.push_back(args[++argidx]);
			// 	continue;
			// }
			// if (args[argidx] == "-max_iter" && argidx+1 < args.size()) {
			// 	max_iter = atoi(args[++argidx].c_str());
			// 	continue;
			// }
		}
		extra_args(args, argidx, design);

		bool did_something = true;
		int count = 0;

		while (did_something)
		{
			did_something = false;

		}

		

		char **arg_list;
    	int arg_len;

    	/* converting into format digestible by odin */
	    arg_list = (char**)malloc(arguments.size()*sizeof(char*));
		arg_len = arguments.size();

		std::cout << "Running ./Odin ";
    	for( int i=0; i<arg_len; i ++)
	    {
    	    arg_list[i] = vtr::strdup(arguments[i].c_str());
	        std::cout << arg_list[i] << " ";
    	}

		netlist_t* odin_netlist = start_odin_ii(arg_len, arg_list);
    	terminate_odin_ii(odin_netlist);

		log("%s\n", xml_arch_addr.c_str());

		log("Faked %d unique modules.\n", count);
	}
} OdinPass;

PRIVATE_NAMESPACE_END