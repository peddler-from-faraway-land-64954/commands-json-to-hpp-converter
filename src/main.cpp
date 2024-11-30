#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "code.hpp"

namespace po = boost::program_options;
using json = nlohmann::json;

int main(int ac, char** av){
	std::string inputpath="commands.json";
	std::string output="commands.hpp";
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("output", po::value<std::string>(), "set output file path")
		("v", po::value<int>()->default_value(2), "set logging level")
		;
	po::variables_map vm;
	po::positional_options_description p;
	p.add("input-file", -1);

	po::store(po::command_line_parser(ac, av).
			options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}
	if (vm.count("output")) {
		output = vm["output"].as<std::string>();
	}
	if (vm.count("input-file")) {
		inputpath = vm["input-file"].as<std::string>();
	}
	if (vm.count("v")) {
		boost::log::trivial::severity_level sev;
		switch (vm["v"].as<int>()) {
			case 0:
				sev = boost::log::trivial::fatal;
				break;
			case 1:
				sev = boost::log::trivial::error;
				break;
			case 2:
				sev = boost::log::trivial::warning;
				break;
			case 3:
				sev = boost::log::trivial::info;
				break;
			case 4:
				sev = boost::log::trivial::debug;
				break;
			case 5:
				sev = boost::log::trivial::trace;
				break;
			default:
				BOOST_LOG_TRIVIAL(warning) << "Invalid verbosity level";

		}
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= sev);
	}
	json config = {
	};
	try {
		std::ifstream localconfig(inputpath, std::ifstream::in);
		if (!localconfig.fail()) {
			BOOST_LOG_TRIVIAL(trace) << inputpath << " file found.";
			config=json::parse(localconfig);
		} else {
			BOOST_LOG_TRIVIAL(warning) << inputpath << " file found.";
			return -1;
		}
	} catch (json::parse_error& ex){
		BOOST_LOG_TRIVIAL(warning) << inputpath << " not valid json";
		return -1;
	}
	return code_main(config, output);
}
