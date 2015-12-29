////////////////
// m3dtob3m.cc
////////////////
////////////////
#ifndef UNICODE
#define UNICODE
#endif
#include "imm_cmd_util.h"
//
#pragma comment(lib, "User32")
#pragma comment(lib, "Effects11")
#pragma comment(lib, "DirectXTK")
#pragma comment(lib, "lua")
////////////////
//
////////////////
////////////////
int main(int argc, char* argv[])
{
	imm::IS_STANDALONE_M3DTOB3M = true;
	imm::IMM_PATH["script"] = "";
	imm::IMM_PATH["output"] = "";
	imm::IMM_PATH["input"] = "";
	if (argc > 1) {
		std::string argv1 = argv[1];
		if (argv1 == "/uselist") {
			std::cout << "m3dtob3m processing..." << std::endl;
			std::wstring dummy_str;
			std::atomic<bool> dummy_bool;
			imm::m3d_util_b3m(dummy_str, dummy_bool);
			std::cout << "m3dtob3m has finished all conversion." << std::endl;
			return 0;
		}
		if (argv1 == "/basic" || argv1 == "/skinned") {
			if (argc == 2) {
				std::cout << "ERROR: Please specify .m3d filename." << std::endl;
				return 0;
			}
			bool is_skinned = false;
			if (argv1 == "/skinned") is_skinned = true;
			std::string m3d_name = argv[2];
			imm::m3d_util_b3m(m3d_name, is_skinned);
			return 0;
		}
	}
	if (argc < 10) {
		std::cout << "M3D to B3M Tool" << std::endl;
		std::cout << "immature engine Utility" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: m3dtob3m <options> <.m3d_filename>" << std::endl;
		std::cout << std::endl;
		std::cout << "    /uselist        use m3d_list.csv declare which files to convert"<< std::endl;
		std::cout << "    /basic          convert signle basic .m3d file to .b3m file" << std::endl;
		std::cout << "    /skinned        convert signle skinned .m3d file to .b3m file" << std::endl;
	}
	return 0;
}