#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>
#include <chrono>

void save_cursor_position() {
    // save cursor poisition
    std::wcout << "\033[s" << std::flush;
}

void restore_n_clear_cursor_pos() {
    // Restore cursor position
    std::wcout << "\033[u" << std::flush;
}

// sleep thread in ms
void sleep(long time);

void deck_shifter(std::deque<std::vector<int>> &deck, std::vector<int> elem) {
    deck.pop_back();
    deck.push_front(elem);
}

void print_img(
    // Core image
    const std::vector<std::wstring> &image_lines,
    // raindrop matrix
    const std::deque<std::vector<int>> &raindrop_matrix
) {

    std::wstring to_print;

    // For each line of image_lines
    for (unsigned long x = 0; x < image_lines.size(); ++x) {
        // Get this line of the image
        std::wstring this_line = image_lines[x];
        // and get the matching row of raindrop matrix
        std::vector<int> matching_row = raindrop_matrix[x];
        // For each chacter in this_line, if matching_row[char] is 1, change that char to raindrop
        for (unsigned long int z = 0; z < this_line.length(); ++z) {
            if (matching_row[z] == 1) {
                // Raindrop char
                this_line[z] = '|';
            }
        }
        to_print += this_line;
        to_print += L'\n';
    }

    // Not having std::flush here caused bug for frames to not reset (grows long vertically when printing)
    std::wcout << to_print << std::flush;
}

// linear congruential generator (PRNG)
class LCG {
public:
    LCG(unsigned long seed = 1): _seed(seed) {}

    void main_get_rands(
        // Maximum number of ints to get (this is 15% of the width)
        unsigned long max_num_ints,
        // This is a pointer to update with the random max between 0-15% width
        unsigned long* how_many,
        // Width 
        unsigned long width,
        // pointer to a vector of unsigned longs, this is udpated with the rands
        std::vector<unsigned long*>* res_vec
    ) {

        // First use rng to determine how many random numbers to get
        this->get_rand(max_num_ints, how_many);

        // Resize vec to fill with default values (nulls)
        res_vec->resize(*how_many);

        // Updates each pointer in res_vec with a new random number
        for (unsigned long x = 0; x < *how_many; ++x) {
            // Alloc index with an unsigned long so get_rand gets passed the correct type
            (*res_vec)[x] = new unsigned long;
            // Update index with a rand
            this->get_rand(width, (*res_vec)[x]);
        }
    }

    void get_rand(unsigned long max, unsigned long* res) {

        // update seed
        _seed = ((_seed * 193) + 497);

        *res = _seed;

        // Shift until res less than max
        while (*res > max) {
            *res = *res >> 3;
        }
        if (*res == 0) {
            *res = 1;
        }
    }

private:
    // Seed is updated every time get_rand is called
    unsigned long _seed;
};

// Pass ref to deque and element to add to front
// -- adds "elem" to front
// -- drops last element from end
// ex: deck_shift(<A, B, C>, "hi") becomes <hi, A, B>
void deck_shift(std::deque<std::string> &deck, std::string elem);

int main(int argc, char* argv[]) {

    // set the locale for writing wide chars to stdout
    std::locale::global(std::locale(""));

    // verify filename is passed
    if (argc < 2) {
        std::cerr << "File name must be passed. Ex `./rain file_name.cat`" << '\n';
        return 1;
    }

    // this is pointless but I'm trying to get used to c manual mem
    // allocate mem sizeof arg passed
    char* file_name = (char*) malloc(sizeof(argv[1]));
    // always verify malloc works
    if (file_name == nullptr) {
        std::cerr << "Memory allocation failed at line" << __LINE__ << '\n';
        return 1;
    }

    // copy argv[1] (which is a char pointer) into file_name (which is a char pointer)
    std::strcpy(file_name, argv[1]);

    // Read file
    std::fstream image(file_name);
    free(file_name);

    if (!image.is_open()) {
        std::cerr << "Problem opening file" << '\n';
        return 1;
    }
    
    unsigned long* width = (unsigned long*) malloc(sizeof(unsigned long));
    // am i even using height?
    unsigned long* height = (unsigned long*) malloc(sizeof(unsigned long));
    if (width == nullptr || height == nullptr) {
        std::cerr << "Memory allcocation failed: " << __LINE__ << '\n';
        return 1;
    } else {
        // set starting values (could also use calloc instead of malloc)
        memset(width, 0, sizeof(unsigned long));
        memset(height, 0, sizeof(unsigned long));
    }

    // original state of the image
    std::vector<std::wstring> image_lines;

    std::string line;
    while(getline(image, line)) {
        //rip
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring cline = converter.from_bytes(line);
        // realistically the user should verify every line is same width right
        if (line.length() > *width) {
            *width = cline.length();
        }

        *height = *height + 1;

        // I'm going to need a static reference to the file's lines in memory, for when a raindrop is "removed" to go back to the old char
        image_lines.push_back(cline);
    }

    // Close image no longer needed
    image.close();

    std::deque<std::vector<int>> raindrop_matrix;

    for (unsigned long int x = 0; x < image_lines.size(); ++x) {
        // for each vertical line of the image, put a vec of 0's into raindrop_matrix that is as long as the width
        std::vector<int> line(*width, 0);
        raindrop_matrix.push_back(line);
    }

    LCG rng(69);

    // Here I have the width of the line, I need to get
    // - random number between 15% of the width of the line, which will be
    // The max amount of random ints to get (this is 15% of the width)
    unsigned long* max_num_ints = (unsigned long*) malloc(sizeof(unsigned long));
    *max_num_ints = (*width * 15) / 100;
    unsigned long* how_many = (unsigned long*) malloc(sizeof(unsigned long));

    // The vector of random ints, for ex <8, 3, 1, 8, 12>
    std::vector<unsigned long*>* res_vec = new std::vector<unsigned long*>();
    rng.main_get_rands(*max_num_ints, how_many, *width, res_vec);

    // res_vec now has the correct structure <9, 3, 1, ...>

    // at raindrop_matrix[0], go through and flip each value
    // This updates the raindrop_matrix with the random raindrop placements
    for (unsigned long x = 0; x < res_vec->size(); ++x) {
        raindrop_matrix[0][*(*res_vec)[x]] ^= 1;
    }

    // loop indefinitely
    while (true) {
        // save cursor position
        save_cursor_position();
        // print initial image
        print_img(
            image_lines,
            raindrop_matrix
        );
        // First shift the deque to insert <0..> vec at matrix[0] and move matrix[0] -> [1]
        std::vector<int> new_line(*width, 0);
        deck_shifter(raindrop_matrix, new_line);
        // Updates res_vec with new random numbers for next row of raindrops
        rng.main_get_rands(*max_num_ints, how_many, *width, res_vec);
        // This updates the raindrop_matrix[0] (just inserted) with the new row of rand raindrop locations
        for (unsigned long x = 0; x < res_vec->size(); ++x) {
            raindrop_matrix[0][*(*res_vec)[x]] ^= 1;
        }
        sleep(50);
        // restore and clear cursor pos
        restore_n_clear_cursor_pos();
    }

    // add clean up
    return 0;
}

// sleep thread in ms
void sleep(long time) {
    std::chrono::milliseconds timespan(time);
    std::this_thread::sleep_for(timespan);
}

void deck_shift(std::deque<std::string> &deck, std::string elem) {
    deck.pop_back();
    deck.push_front(elem);
}

