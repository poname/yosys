#ifndef YosysHelper_H
#define YosysHelper_H

#include "odin_types.h"
/* Odin-II exit status enumerator */
enum ODIN_ERROR_CODE {
    SUCCESS,
    ERROR_INITIALIZATION,
    ERROR_PARSE_CONFIG,
    ERROR_PARSE_ARGS,
    ERROR_PARSE_ARCH,
    ERROR_PARSE_BLIF,
    ERROR_ELABORATION,
    ERROR_OPTIMIZATION,
    ERROR_TECHMAP,
    ERROR_SYNTHESIS,
    ERROR_OUTPUT
};

netlist_t* start_odin_ii(int argc, char** argv);
int terminate_odin_ii(netlist_t* odin_netlist);

void set_default_config();
void get_options(int argc, char** argv);

void prepare_odin(int argc, char** argv);

void warmup_odin_ii(int argc, char** argv);
void handle_dsp_blocks();
netlist_t* continue_odin_ii();

#endif
