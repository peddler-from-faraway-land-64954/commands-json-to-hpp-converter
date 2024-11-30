#include "code.hpp"
#include <fstream>
#include <ios>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

using json = nlohmann::json;


std::string headers = 
R"(#include <string>
#include <dpp/appcommand.h>
#include <dpp/dispatcher.h>
#include <vector>
#include <unordered_map>

)";

std::string structs = 
R"(
struct arg {
	std::string name;
	std::string description;
	dpp::command_option_type type;
	bool required;
	std::vector<arg> args; 
	arg(std::string name, std::string description, dpp::command_option_type type, bool required, std::vector<arg> args):
		name(name),
		description(description),
		type(type),
		required(required),
		args(args)
	{}
};

struct command {
	std::string name;
	std::string description;
	void (&func)(dpp::cluster&,  const dpp::slashcommand_t&);
	std::vector<arg> args; 
	command(std::string name, std::string description, void (&func)(dpp::cluster&,  const dpp::slashcommand_t&), std::vector<arg> args):
		name(name),
		description(description),
		func(func),
		args(args)
	{}
};

)";


int code_main(nlohmann::json input, std::string output){
	std::vector<std::string> functions;
	const std::unordered_map<std::string, std::string> types = {
		{"sub_command",       "dpp::co_sub_command"},
		{"sub_command_group", "dpp::co_sub_command_group"},
		{"string",            "dpp::co_string"},
		{"integer",           "dpp::co_integer"},
		{"boolean",           "dpp::co_boolean"},
		{"user",              "dpp::co_user"},
		{"channel",           "dpp::co_channel"},
		{"role",              "dpp::co_role"},
		{"mentionable",       "dpp::co_mentionable"},
		{"number",            "dpp::co_number"},
		{"attachment",        "dpp::co_attachment"},
	};
	std::ofstream rfile;
	std::stringstream file;
	rfile.open(output, std::ios::out | std::ios::trunc);
	int indentc = 0;
	file << structs;
	file << "const std::unordered_map<std::string, command> commands = {" << std::endl;
	indentc++;
	auto indent = [&indentc, &file](){
		for (int i=0; i<indentc; i++){
			file << "\t";
		}
	};
	auto deal_with_sub_command = [&file, &types, &indent](json args){
		for (json arg : args){
			indent();
			file << "arg("
				<< '"'
				<< std::string(arg["name"])
				<< "\", \""
				<< std::string(arg["description"])
				<< "\", ";
			assert(arg["type"]!="sub_command");
			assert(arg["type"]!="sub_command_group");
			file << types.at(arg["type"])
				<< ", "
				<< (arg["required"]!=NULL && arg["required"])
				<< ", {";
			file << "})," << std::endl;
		}
	};
	for (json command : input){
		indent();
		file
			<< "{\"" << std::string(command["name"]) << "\", "
			<< "command(" 
			<< '"'
			<< std::string(command["name"])
			<< "\", \""
			<< std::string(command["description"])
			<< "\", ";
		functions.push_back(command["function"]);
		file
			<< std::string(command["function"])
			<< ", {" << std::endl;
		indentc++;
		for (json arg : command["args"]){
			indent();
			file << "arg("
				<< '"'
				<< std::string(arg["name"])
				<< "\", \""
				<< std::string(arg["description"])
				<< "\", "
				<< types.at(arg["type"])
				<< ", "
				<< ((arg.contains("required") && arg["required"])?"true":"false")
				<< ", {" << std::endl;
			indentc++;
			if (arg["type"]=="sub_command"){
				deal_with_sub_command(arg["args"]);
			}
			if (arg["type"]=="sub_command_group"){
				for (json arg2 : arg["args"]){
					indent();
					file << "arg("
						<< '"'
						<< std::string(arg2["name"])
						<< "\", \""
						<< std::string(arg2["description"])
						<< "\", ";
					assert(arg2["type"]=="sub_command");
					file << types.at(arg2["type"])
						<< ", "
						<< ((arg2["required"]!=NULL && arg["required"])?"true":"false")
						<< ", {";
					indentc++;
					if (arg2["type"]=="sub_command"){
						deal_with_sub_command(arg2["args"]);
					}
					indentc--;
					indent();
					file << "})," << std::endl;
				}
			}
			indentc--;
			indent();
			file << "})," << std::endl;
		}
		indentc--;
		indent();
		file << "})}," << std::endl;;
	}
	indentc--;
	indent();
	file << "};";
	rfile << headers;
	for (auto func : functions){
		rfile << "void " << func << "(dpp::cluster&,  const dpp::slashcommand_t&);" << std::endl;
	}
	rfile << file.str();
	rfile.close();
	return 0;
}
