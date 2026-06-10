/*
    MASTERMIND - The game
    Version: 1.0
    Written by: S. Sampreet Vasishta
    Date: 10/06/2026

    Can you guess the correct combination with as few guesses as possible?
*/

import std;

enum class Attempt_Entry_State {
    CORRECT_POSITION,
    PRESENT_WRONG_POSITION,
    NOT_PRESENT
};

std::vector<int> random_sequence(int length, int min, int max);
std::vector<int> input_sequence(int length, const std::string& prompt, std::function<bool(int)> valid_condition, const std::string& invalid);
std::vector<Attempt_Entry_State> mastermind_compare(std::vector<int> attempt, std::vector<int> correct);

int main() try {
    constexpr int game_size{4};     // Number of digits in correct answer and allowed for each attempt; Must be >= 1
    constexpr int max_turns{10};    // Maximum number of attempts allowed before "Game Over"; Must be >= 1
    constexpr int digits_range{3};  // Each digit in the correct answer will range from 0 to <digits_range>; Must be >= 0 and <= 9

    std::cout << "\t---\tMASTERMIND\t---\n";
    std::cout << "Can you guess the correct combination of " << game_size << (game_size != 1 ? " digits" : " digit") << " (ranging from 0 to " << digits_range << ") within " << max_turns << (max_turns != 1 ? " attempts?\n\n" : " attempt?\n\n");

    // Randomly initializing the correct answer
    std::vector<int> correct_answer = random_sequence(game_size, 0, digits_range);

    // Game loop
    bool game_win = false;
    for(int turn = 1; turn <= max_turns; ++turn) {

        // Input attempt
        std::vector<int> attempt = input_sequence(
            game_size,
            "Attempt " + std::to_string(turn) + ": ",
            [](int value){ return value >= 0 && value <= digits_range; },
            "Invalid input. Please enter " + std::to_string(game_size) + (game_size != 1 ? " digits" : " digit") + " from 0 to " + std::to_string(digits_range) + "."
        );

        // Compare attempt with correct answer and display result
        std::vector<Attempt_Entry_State> attempt_comparison = mastermind_compare(attempt, correct_answer);
        int num_correct{0}, num_present{0};
        for(auto entry : attempt_comparison) {
            if(entry == Attempt_Entry_State::CORRECT_POSITION)
                ++num_correct;
            if(entry == Attempt_Entry_State::PRESENT_WRONG_POSITION)
                ++num_present;
        }

        if(num_correct == game_size) {
            game_win = true;
            break;
        } else {
            std::cout << num_correct << (num_correct != 1 ? " digits are" : " digit is") << " in the correct position.\n";
            std::cout << num_present << (num_present != 1 ? " digits are" : " digit is") << " present in the correct answer, but in the wrong place.\n";
        }

    }

    // End of game
    if(game_win)
        std::cout << "\nWell done! You have correctly guessed the sequence.\n";
    else {
        std::cout << "\nGAME OVER: You have used all of your attempts but could not guess the correct answer.\n";
        std::cout << "The correct answer was ";
        for(auto digit : correct_answer)
            std::cout << digit << " ";
        std::cout << "\nTry again.\n";
    }

    return 0;
} catch(std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
} catch(...) {
    std::cerr << "Error: Unknown exception.\n";
}


// Generate a random sequence of <length> integers ranging from <min> to <max>
std::vector<int> random_sequence(int length, int min, int max){

    /* Pre-conditions */
    // length must be >= 0 
    if(length < 0)
        throw std::invalid_argument("Length of random sequence cannot be negative.");
    // max must be >= min
    if(max < min)
        throw std::invalid_argument("Maximum possible random digit in a random sequence cannot be lesser than the minimum possible random digit.");

    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine engine(seed);
    std::uniform_int_distribution<int> distribution(min, max);
    std::vector<int> sequence(length);
    for(int i = 0; i < length; ++i)
        sequence[i] = distribution(engine);

    /* Post-conditions */
    // Size of sequence must be <length>
    if(sequence.size() != length)
        throw std::runtime_error("Random sequence did not end up with the required number of values.");
    // Each value of the sequence must be within the range <min> to <max> (inclusive)
    for(auto value : sequence)
        if(value < min || value > max)
            throw std::runtime_error("Values of random sequence did not end up in the correct range.");

    return sequence;
}


// Input a sequence of <length> integers by providing a prompt and handling invalid inputs
std::vector<int> input_sequence(int length, const std::string& prompt, std::function<bool(int)> valid_condition, const std::string& invalid) {

    /* Pre-conditions */
    // length must be >= 0
    if(length < 0)
        throw std::invalid_argument("Length of sequence to be inputted cannot be negative.");
    
    std::vector<int> result(length);
    while(true) try {
        std::cout << prompt;
        for(int i = 0; i < length; ++i) {
            std::cin >> result[i];
            if(!std::cin || !valid_condition(result[i])) 
                throw std::range_error("");
        }
        break;
    } catch(std::range_error) {
        std::cout << invalid << "\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    /* Post-conditions */
    // Size of sequence must be <length>
    if(result.size() != length)
        throw std::runtime_error("Inputted sequence did not end up with the required number of values.");
    // Each value of the sequence must be valid
    for(auto value : result)
        if(!valid_condition(value))
            throw std::runtime_error("Values of inputted sequence did not end up in the correct range.");

    return result;
}


// Compares a given attempt with the given correct answer and returns a vector showing
// which entries of attempt are in the correct position, present but in the worng position and not present
std::vector<Attempt_Entry_State> mastermind_compare(std::vector<int> attempt, std::vector<int> correct) {

    /* Pre-conditions */
    // attempt and correct must have the same number of elements
    if(attempt.size() != correct.size())
        throw std::invalid_argument("Cannot compare sequences of different sizes.");

    std::vector<Attempt_Entry_State> result(attempt.size(), Attempt_Entry_State::NOT_PRESENT);

    for(int i = 0; i < attempt.size(); ++i)
        if(attempt[i] == correct[i])
            result[i] = Attempt_Entry_State::CORRECT_POSITION;

    for(int i = 0; i < attempt.size(); ++i) {
        if(result[i] != Attempt_Entry_State::CORRECT_POSITION) {
            for(int j = 0; j < correct.size(); ++j) {
                if(attempt[i] == correct[j] && result[j] == Attempt_Entry_State::NOT_PRESENT) {
                    result[j] = Attempt_Entry_State::PRESENT_WRONG_POSITION;
                    break;
                }
            }
        }
    }

    return result;
}