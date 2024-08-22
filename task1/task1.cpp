// Task ONE: Directory Reconstruction
// Aidan Hall

#include <charconv>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <optional>
#include <span>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std;

// TYPES

struct Arena {
  static constexpr size_t ALLOCATION_LEN = 1024 * 1024 * 1024;
  char *base;
  size_t free_space;
  Arena() {
    base = new char[ALLOCATION_LEN];
    free_space = 0;
  }
  ~Arena() { delete[] base; }

  inline void push_char(const char c) {
    base[free_space] = c;
    free_space -= 1;
  }

  inline string_view store_name(const string_view name) {
    char *stored_start = &base[free_space];
    copy(name.begin(), name.end(), stored_start);
    free_space += name.size();
    return string_view{stored_start, name.size()};
  }
};

struct Directory {
  string_view name;
  // child directories
  unordered_map<string_view, size_t> children;
  // If parent == own index == 0, this Directory is the root.
  size_t parent;
  // whether ls has been run in this directory.
  bool listed;
  Directory(size_t parent, string_view name)
      : parent(parent), listed(false), children{}, name(name) {}
};

// GLOBALS

vector<Directory> directories = {
    // The root directory
    Directory(0, "")};

Arena names_arena;

std::array<char, FILENAME_MAX> filename_buffer;
std::array<char, 255> ls_listing_buffer;
std::optional<std::string> deepest_nest_file = nullopt;

size_t working_directory = 0;
size_t current_depth = 0;
uint64_t max_nesting_depth = 0;
// The most deeply nested directory.
size_t max_nesting_index = 0;
uintmax_t total_depth = 0;
uintmax_t file_count = 0;
size_t input_line = 0;

// FUNCTIONS

// Create a new child directory of parent, named 'name', with depth 'depth'
size_t add_child(size_t parent, string_view name, size_t depth) {
  // Add the child directory to the end of the directories vector.
  size_t new_dir_location = directories.size();
  directories.push_back(Directory(parent, name));

  // Create a reference to the child in the current directory's entry.
  directories[parent].children[name] = new_dir_location;

  // Update deepest nested directory if the new directory is deeper
  if (depth >= max_nesting_depth) {
    max_nesting_depth = depth;
    max_nesting_index = new_dir_location;
    // This is the new deepest directory; we don't know what files it has yet.
    deepest_nest_file = nullopt;
  }

  return new_dir_location;
}

// cd to 'destination', which will have depth 'depth'
size_t cd_child(size_t cwd, string_view destination, size_t depth) {
  current_depth = depth;

  if (directories[cwd].children.contains(destination)) {
    // Child directory already exists
    return directories[cwd].children.at(destination);
  }

  // Child with that name not present: set it up.

  return add_child(cwd, names_arena.store_name({destination}), depth);
}

// Return the index of the working directory after running 'cd destination' in
// 'working_directory'
size_t change_directory(size_t working_directory, string_view destination) {
  switch (destination.length()) {
  case 0:
    cerr << "no destination to cd\n";
    abort();
  case 1:
    if (destination[0] == '/') {
      // Go to the root.
      current_depth = 0;
      return 0;
    } else {
      return cd_child(working_directory, destination, current_depth + 1);
    }
    break;
  case 2:
    if (destination.compare("..") == 0) {
      current_depth -= 1;
      return directories[working_directory].parent;
    }
    [[fallthrough]];
  default:
    return cd_child(working_directory, destination, current_depth + 1);
  }
}

string_view read_word(std::span<char> buffer) {
  size_t offset = 0;
  while (!cin.eof() && !isspace(cin.peek()) && cin.peek() != EOF) {
    buffer[offset] = cin.get();
    offset++;
  }

  return string_view{&buffer[0], offset};
}

inline void read_to_eol() {
  // Read the rest of the line.
  while (cin.get() != '\n' && !cin.eof())
    ;
  input_line++;
}

inline void skip_spaces() {
  while (isspace(cin.peek()) && !cin.eof())
    cin.get();
}

void print_deepest_path() {
  std::stack<size_t> deep_path;
  size_t current_dir = max_nesting_index;
  // Accumulate the deepest path
  while (current_dir != 0) {
    deep_path.push(current_dir);
    current_dir = directories[current_dir].parent;
  }

  // Print the directories
  while (deep_path.size() > 0) {
    cout << '/' << directories[deep_path.top()].name;
    deep_path.pop();
  }

  cout << '/';

  // Print the name of a file in the deepest path, if there is one.
  if (deepest_nest_file != std::nullopt) {
    cout << *deepest_nest_file;
  }
}

int main() {

  while (!cin.eof() && !(cin.peek() == EOF)) {
    char prompt[2];
    prompt[0] = cin.get();
    prompt[1] = cin.get();
    if (prompt[0] != '$' || prompt[1] != ' ') {
      cerr << "Invalid prompt format on line " << input_line << '\n';
      return 1;
    }

    // Read the first character of the command.
    char command = cin.get();
    // Read the second character of the command; we don't need it.
    cin.get();

    switch (command) {
    case 'c': {
      // cd

      // Read the spaces before the argument
      skip_spaces();

      string_view argument = read_word(span{filename_buffer});
      working_directory = change_directory(working_directory, argument);

      read_to_eol();
      break;
    }
    case 'l': {
      // ls
      read_to_eol();

      if (directories[working_directory].listed) {
        // We've already listed this directory; nothing to do.
        // Read through until the next command prompt
        while (cin.peek() != '$') {
          read_to_eol();
        }

      } else {

        // Process the directory listing
        while (cin.peek() != '$' && !cin.eof()) {
          // Parse the line
          string_view listing = read_word(span{ls_listing_buffer});
          skip_spaces();
          string_view name = read_word(span{filename_buffer});
          read_to_eol();

          if (listing.compare("dir") == 0) {
            // Add the child if it wasn't already added.
            if (!directories[working_directory].children.contains(name)) {
              add_child(working_directory, names_arena.store_name(name),
                        current_depth + 1);
            }
          } else {
            // file entry
            file_count += 1;
            total_depth += current_depth;
            // Store the name of a file in the deepest-nested directory, but
            // only one.
            if (working_directory == max_nesting_index &&
                deepest_nest_file == nullopt) {
              deepest_nest_file = name;
            }
          }
        }

        directories[working_directory].listed = true;
      }
      break;
    }
    default:
      cerr << "Unknown shell command prefix: " << command << '\n';
      abort();
    }
  }

  cout << file_count << ",\"";
  print_deepest_path();
  cout << "\"," << std::fixed << std::setprecision(1)
       << (double)total_depth / file_count << '\n';
}
