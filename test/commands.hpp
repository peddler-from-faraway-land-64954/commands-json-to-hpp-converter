#include <string>
#include <dpp/appcommand.h>
#include <dpp/dispatcher.h>
#include <vector>
#include <unordered_map>

void ping(dpp::cluster&,  const dpp::slashcommand_t&);
void color(dpp::cluster&,  const dpp::slashcommand_t&);
void set(dpp::cluster&,  const dpp::slashcommand_t&);

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

const std::unordered_map<std::string, command> commands = {
	{"ping", command("ping", "Ping pong!", ping, {
	})},
	{"color", command("color", "Set color", color, {
		arg("color", "Color name or hex", dpp::co_string, false, {
		}),
	})},
	{"set", command("set", "Set config", set, {
		arg("max_roles", "set max roles", dpp::co_sub_command, false, {
			arg("number", "number of roles", dpp::co_integer, 1, {}),
		}),
	})},
};