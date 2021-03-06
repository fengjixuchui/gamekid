#include <gamekid/cpu/cpu.h>
#include <gamekid/runner.h>
#include <gamekid/utils/files.h>
#include <iostream>
#include "gamekid/utils/str.h"
#include "gamekid/cpu/operands_container.h"
#include "window.h"
#include <thread>
#include "gamekid/io/video/tile.h"
#include <atomic>
#undef main

void welcome();
std::vector<std::string> get_user_command();

using command = std::function<void(gamekid::runner& runner, const std::vector<std::string>& arguments)>;

// Commands 
void run(gamekid::runner& runner, const std::vector<std::string>& args);
void add_breakpoint(gamekid::runner& runner, const std::vector<std::string>& args);
void regs(gamekid::runner& runner, const std::vector<std::string>& args);
void next(gamekid::runner& runner, const std::vector<std::string>& args);
void list(gamekid::runner& runner, const std::vector<std::string>& args);
void debugger_exit(gamekid::runner& runner, const std::vector<std::string>& args);
void help(gamekid::runner& runner, const std::vector<std::string>& args);
void view(gamekid::runner& runner, const std::vector<std::string>& args);
void del(gamekid::runner& runner, const std::vector<std::string>& args);
void breakpoints(gamekid::runner& runner, const std::vector<std::string>& args);
void dump_screen(gamekid::runner& runner, const std::vector<std::string>& args);

const std::map<std::string, command> commands =
{
    { "run", run },
    { "break", add_breakpoint },
    { "regs", regs },
    { "next", next },
    { "list", list },
    { "exit", debugger_exit },
    { "help", help },
    { "view", view},
    { "del", del},
    { "breakpoints", breakpoints},
    { "dump_screen", dump_screen}
};

bool debugger_running;

int main(const int argc, const char* argv[]) {
    debugger_running = true;

    welcome();

    std::string filename(argv[1]);
    gamekid::rom::cartridge cart(gamekid::utils::files::read_file(filename));
    gamekid::runner r(std::move(cart));


    while (debugger_running) {
        std::vector<std::string> command = get_user_command();

        auto key_value_pair = commands.find(command[0]);

        if (key_value_pair == commands.end()) {
            std::cout << "Unknown command '" << command[0] << "'" << std::endl;
        }
        else {
            try {
                key_value_pair->second(r, command);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    exit(0);
}

void welcome() {
    const char* gamekid_art =
        "                            _    _     _ \n"
        "  __ _  __ _ _ __ ___   ___| | _(_) __| |\n"
        " / _` |/ _` | '_ ` _ \\ / _ \\ |/ / |/ _` |\n"
        "| (_| | (_| | | | | | |  __/   <| | (_| |\n"
        " \\__, |\\__,_|_| |_| |_|\\___|_|\\_\\_|\\__,_|\n"
        " |___/                                  \n";

    std::cout << gamekid_art << "\n\n" << std::endl;
    std::cout << ">> Welcome To GameKid Debugger~" << std::endl;
}

std::vector<std::string> get_user_command() {
    std::cout << ">> ";
    std::vector<std::string> command;

    do {
        std::string user_input;
        std::cin >> user_input;
        command.push_back(user_input);
    } while (std::cin.peek() != '\n');

    return command;
}

void run(gamekid::runner& runner, const std::vector<std::string>& args) {
    runner.run_until_break();
 }

void add_breakpoint(gamekid::runner& runner, const std::vector<std::string>& args) {
    if (args.size() <= 1) {
        std::cerr << "breakpoint requires an address" << std::endl;
        return;
    }

    const word address = gamekid::utils::convert::to_number<word>(args[1], 16);
    runner.add_breakpoint(address);
    std::cout << "set breakpoint at address 0x" << gamekid::utils::convert::to_hex<word>(address) << std::endl;
}

void regs(gamekid::runner& runner, const std::vector<std::string>& args) {
    constexpr size_t regs_per_line = 4;
    const std::vector<gamekid::cpu::reg*>& regs = runner.cpu().regs;

    for (size_t i = 0; i < regs.size(); ++i) {
        std::cout << regs[i]->name() << ": " << std::hex << regs[i]->load_as_word() << " ";

        if ((i + 1) % regs_per_line == 0) {
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
}

void next(gamekid::runner& runner, const std::vector<std::string>& args) {
    runner.next();
    std::cout << runner.list(runner.cpu().PC.load(), 1)[0] << std::endl;
}

void list(gamekid::runner& runner, const std::vector<std::string>& args) {
    word count;
    word address = runner.cpu().PC.load();

    
    if (args.size() == 2) {
        count = gamekid::utils::convert::to_number<word>(args[1]);
    }
    else if (args.size() == 3) {
        address = gamekid::utils::convert::to_number<word>(args[1], 16);
        count = gamekid::utils::convert::to_number<word>(args[2]);
    }
    else {
        count = 10;
    }

    std::vector<std::string> list = runner.list(address, count);

    for (auto& op : list) {
        std::cout << op << std::endl;
    }
}

void debugger_exit(gamekid::runner& runner, const std::vector<std::string>& args) {
    debugger_running = false;
}

void help(gamekid::runner& runner, const std::vector<std::string>& args) {
    for (auto& command_pair : commands) {
        std::cout << command_pair.first << std::endl;
    }
}

void view(gamekid::runner& runner, const std::vector<std::string>& args) {
    if (args.size() <= 1) {
        std::cerr << "Missing <address_to_view> argument." << std::endl;
        std::cout << "view <address_to_view> [length_to_view]" << std::endl;
        return;
    }

    const word address_to_view = gamekid::utils::convert::to_number<word>(args[1], 16);

    const word length_to_view =
        args.size() >= 3 ? gamekid::utils::convert::to_number<word>(args[2]) : 64;

    std::vector<byte> mem_view = runner.dump(address_to_view, length_to_view);

    for (word i = 0; i < mem_view.size(); ++i) {
        // if it is a start of a new line
        if (i % 16 == 0) {
            if (i != 0) {
                // print a new line if it is not the first line
                std::cout << std::endl;
            }
            // print the address
            std::cout << gamekid::utils::convert::to_hex<word>(address_to_view + i) << ": ";
        }

        // print the memory byte
        std::cout << gamekid::utils::convert::to_hex<byte>(mem_view[i]) << " ";
    }

    std::cout << std::endl;
}

void del(gamekid::runner& runner, const std::vector<std::string>& args) {
    if (args.size() <= 1) {
        std::cerr << "Missing breakpoint to delete" << std::endl;
        std::cout << "del <address_to_delete>" << std::endl;
        return;
    }

    // delete all 
    if (args[1] == "*") {
        if (runner.breakpoints().empty()) {
            std::cout << "No breakpoints to delete." << std::endl;
            return;
        }

        for (word addr : runner.breakpoints()) {
            std::cout << "Deleting 0x" << gamekid::utils::convert::to_hex(addr) << std::endl;
        }

        runner.delete_all_breakpoints();
        return;
    }

    const word breakpoint_address = gamekid::utils::convert::to_number<word>(args[1], 16);

    try {
        runner.delete_breakpoint(breakpoint_address);
    }
    catch (const std::exception& e) {
        std::cerr << "Cannot delete breakpoint: " << e.what() << std::endl;
        return;
    }

    std::cout << "deleted breakpoint at 0x" <<
        gamekid::utils::convert::to_hex<word>(breakpoint_address) << std::endl;
}

void breakpoints(gamekid::runner& runner, const std::vector<std::string>& args) {
    const std::set<word>& bps = runner.breakpoints();

    if (bps.empty()) {
        std::cout << "No breakpoints set" << std::endl;
        return;
    }

    for (word bp : bps) {
        std::cout << "Breakpoint at address 0x" << gamekid::utils::convert::to_hex<word>(bp) << std::endl;
    }
}

const std::array<byte, 4> palette = {0b00, 0b11, 0b11, 0b11 };

void write_tile(gamekid::debugger::window& wnd, const gamekid::video::io::tile& t, gamekid::debugger::point p) {

    for (byte y = 0; y<8; ++y) {
        for (byte x = 0; x<8; ++x) {
            const byte color_index = t.get_color(x, y);
            const byte color_value = palette.at(color_index);
            const SDL2pp::Color& color = gamekid::debugger::colors.at(color_value);
            wnd.put_pixel(gamekid::debugger::point(p.x + x, p.y + y), color);
        }
    }
}

void dump_screen(gamekid::runner& runner, const std::vector<std::string>& args) {
    gamekid::debugger::window wnd;
    
    std::vector<byte> tile_data = runner.dump(0x8000, 0x1000);
    std::vector<byte> tile_map = runner.dump(0x9800, 1024);

    auto tile_data_ptr = reinterpret_cast<std::array<gamekid::video::io::tile, 256>*>(tile_data.data());
    
    for (int y=0; y<32; ++y) {
        for (int x=0; x<32; ++x) {
            const byte tile_index = tile_map.at(y * 32 + x);
            write_tile(wnd, tile_data_ptr->at(tile_index), gamekid::debugger::point(x*8, y*8));
        }
    }

    wnd.show();
    wnd.render();

    while (wnd.poll_events()) {
        SDL_Delay(500);
    }

}
