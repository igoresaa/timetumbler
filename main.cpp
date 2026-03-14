#include <print>
#include <fstream>
#include <random>
#include <chrono>
#include <iostream>
#include <vector>
#include <filesystem>

const std::string pathToNotes{std::string(std::getenv("HOME")) + "/CLionProjects/project/data/"};

enum class CompletionStatus {
    Completed,
    NotCompleted,
    Neutral,
};

std::string_view strForIsCompleted(CompletionStatus is_completed) {
    switch (is_completed) {
        case CompletionStatus::Completed:
            return "Completed";
        case CompletionStatus::NotCompleted:
            return "NotCompleted";
        case CompletionStatus::Neutral:
            return "Neutral";
        default:
            return "Unknown";
    }
}

int getID() {
    std::random_device rd{};
    std::seed_seq seed{
        rd(), rd(), rd(), rd(),
        static_cast<std::seed_seq::result_type>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        )
    };
    std::uniform_int_distribution diapason{1, 100000000};
    std::mt19937 gen{seed};
    int id{diapason(gen)};
    return id;
}

struct Note {
    int id{};
    std::string name{};
    CompletionStatus isCompleted{};
    std::string description{};

    void createNote() const {
        std::ofstream note(pathToNotes + name + ".txt");
        if (!note) {
            std::println("Error creating note file {}", name);
            return;
        }
        std::println(note, "{}\n{}\n{}", id, strForIsCompleted(isCompleted), description);
        std::println("Note: {} successfully created", name);
    }

    void getData() {
        id = getID();
        std::print("Please enter the name of the note you want to create: ");
        std::getline(std::cin, name);
        std::print("Please enter the description of the note you want to create: ");
        std::getline(std::cin, description);
        std::string input{};
        while (true) {
            std::print("The note is [c]ompleted; [n]ot [c]ompleted; or [n]eutral: ");
            std::getline(std::cin, input);
            if (input == "c") {
                isCompleted = CompletionStatus::Completed;
                break;
            } else if (input == "nc") {
                isCompleted = CompletionStatus::NotCompleted;
                break;
            } else if (input == "n") {
                isCompleted = CompletionStatus::Neutral;
                break;
            } else {
                std::println("Invalid input");
                continue;
            }
        }
    }

    static void printNoteDescription() {
        std::print("Please enter the name of the note you want to read: ");
        std::string name{};
        std::getline(std::cin, name);
        std::ifstream note(pathToNotes + name + ".txt");
        if (!note) {
            std::println("Error reading note file {}", name);
            return;
        }
        std::string line{};
        std::getline(note, line);
        std::getline(note, line);
        // We read the 3rd line, since the description begins with it.
        while (std::getline(note, line)) {
            std::println("{}", line);
        }
    }

    static void printNoteList() {
        std::vector<std::string> notes{};
        for (auto &entry: std::filesystem::directory_iterator{pathToNotes}) {
            notes.push_back(entry.path().stem().string());
        }
        for (auto note: notes) {
            std::println("{}", note);
        }
    }

    static void removeNote() {
        std::print("Please enter the name of the note you want to read: ");
        std::string name{};
        std::getline(std::cin, name);
        std::filesystem::remove(pathToNotes + name + ".txt");
        std::println("Note {} successfully removed", name);
    }

    static void switchCompletionStatus() {
        std::print("Please enter the name of the note you want to switch: ");
        std::string name{};
        std::getline(std::cin, name);
        std::print("Switch to [c]ompleted, [n]ot [c]ompleted or [n]eutral: ");
        std::string input{};
        std::getline(std::cin, input);
        std::ifstream note(pathToNotes + name + ".txt");
        if (!note) {
            std::println("Error reading note file {}", name);
        }
        std::string line{};
        std::vector<std::string> lines{};
        while (std::getline(note, line)) {
            lines.push_back(line);
        }
        // edit second string because completion status is on this line
        while (true) {
            if (input == "nc") {
                lines[1] = "NotCompleted";
                break;
            } else if (input == "n") {
                lines[1] = "Neutral";
                break;
            } else if (input == "c") {
                lines[1] = "Completed";
                break;
            } else {
                std::println("Invalid input");
                continue;
            }
        }
        std::ofstream temp(pathToNotes + name + ".txt.temp");
        if (!temp) {
            std::println("Error creating temp file {}", name);
            return;
        }
        for (auto &arrayLine : lines) {
            std::println(temp, "{}", arrayLine);
        }
        std::filesystem::rename(pathToNotes + name + ".txt.temp", pathToNotes + name + ".txt");
        std::println("Successfully switched note {}", name);
    }
};

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::println("Usage: {} -argument", argv[0]);
        return EXIT_SUCCESS;
    }
    std::string args{argv[1]};
    Note note{};
    if (args == "-create") {
        note.getData();
        note.createNote();
    }
    if (args == "-list") {
        note.printNoteList();
    }
    if (args == "-desc") {
        note.printNoteDescription();
    }
    if (args == "-rm") {
        note.removeNote();
    }
    if (args == "-switch") {
        note.switchCompletionStatus();
    }
    return EXIT_SUCCESS;
}
