#include <iostream>
#include <boost/program_options.hpp>
#include "FirewallSocket.h"
#include "Processor.h"

using namespace std;

int main(int argc, char** argv)
{
    opt::options_description desc("All options");

    string accept_port;
    string source;
    string redirect, to;
    string iptables_rule;
    string snat, dnat, masquerade;

    source = "all";

    desc.add_options()
        ("help,h", "Produce this help message")
        ("list,l", "List all rules")
        ("source", opt::value<string>(&source), "Source IP Address")
        ("allow", opt::value<string>(&accept_port), "Add rule that allows a port")
        ("redirect", opt::value<string>(&redirect), "Add rule that redirects a port")
        ("iptables", opt::value<string>(&iptables_rule), "Add IPTABLES rule directly")
        ("snat", opt::value<string>(&snat), "Add rule that translates source address of packet")
        ("dnat", opt::value<string>(&dnat), "Add rule that translates destination address of packet")
        ("masquerade", opt::value<string>(&masquerade), "Add rule that masquerades source address")
        ("to", opt::value<string>(&to), "Destination")
        ("reload", "Reload the daemon")
        ("delete,d", "Delete the rule")
        ;

    /*const char* test_argv[] = {
        "Firewall.out",
        "--allow 12345",
        "--source localhost"
    };
    int test_argc = 3;*/

    opt::variables_map vm;
    try
    {
        opt::store(opt::parse_command_line(argc, argv, desc), vm);
        opt::notify(vm);
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
        cout << "DFW v2.0 (Written by Donghyun Lee)\n\n" << desc << endl;
        return 1;
    }
    if (vm.count("help"))
    {
        cout << "DFW v2.0 (Written by Donghyun Lee)\n\n" << desc << endl;
        return 1;
    }

    if (vm.count("list"))
    {
        Processor::list();
    }
    else if (vm.count("allow"))
    {
        bool is_delete = vm.count("delete");
        Processor::accept_port(accept_port, source, is_delete);
    }
    else if (vm.count("redirect"))
    {
        bool is_delete = vm.count("delete");
        Processor::redirect(redirect, source, stoi(to), is_delete);
    }
    else if (vm.count("reload"))
    {
        Processor::reload();
    }
    else if (vm.count("iptables"))
    {
        bool is_delete = vm.count("delete");
        Processor::iptables(iptables_rule, is_delete);
    }
    else if (vm.count("snat"))
    {
        bool is_delete = vm.count("delete");
        Processor::snat(snat, source, to, is_delete);
    }
    else if (vm.count("dnat"))
    {
        bool is_delete = vm.count("delete");
        Processor::dnat(dnat, source, to, is_delete);
    }
    else if (vm.count("masquerade"))
    {
        bool is_delete = vm.count("delete");
        Processor::masquerade(masquerade, source, to, is_delete);
    }
    else
    {
        cout << "DFW v2.0 (Written by Donghyun Lee)\n\n" << desc << endl;
        return 1;
    }

    return 0;
}