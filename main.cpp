/*Memory game
 *
 * Description:
 * This program is a basic memory game, which asks the user
 * to input card amount, seed value, amount of players and the name
 * of the players. Then program prints a game board, which shows the user
 * possible coordinates to choose from.
 * Asks the user to input coordinates and checks them for invalid inputs.
 * Then the program turns the card and checks if the cards were the same.
 * If so, program will add a point to assigned player.
 * After every card pair has been guessed correctly, the game will end and
 * the program prints the standings and informs who was the winner and how
 * many pairs the winner had.
 *
 * */





#include "player.hh"
#include "card.hh"
#include <iostream>
#include <vector>
#include <random>

using namespace std;

const string INPUT_AMOUNT_OF_CARDS = "Enter the amount of cards (an even number): ";
const string INPUT_SEED = "Enter a seed value: ";
const string INPUT_AMOUNT_OF_PLAYERS = "Enter the amount of players (one or more): ";
const string INPUT_CARDS = "Enter two cards (x1, y1, x2, y2), or q to quit: ";
const string INVALID_CARD = "Invalid card.";
const string FOUND = "Pairs found.";
const string NOT_FOUND = "Pairs not found.";
const string GIVING_UP = "Why on earth you are giving up the game?";
const string GAME_OVER = "Game over!";

using Game_row_type = vector<Card>;
using Game_board_type = vector<vector<Card>>;

// Muuntaa annetun numeerisen merkkijonon vastaavaksi kokonaisluvuksi
// (kutsumalla stoi-funktiota).
// Jos annettu merkkijono ei ole numeerinen, palauttaa nollan
// (mikä johtaa laittomaan korttiin myöhemmin).
//
// Converts the given numeric string to the corresponding integer
// (by calling stoi).
// If the given string is not numeric, returns 0
// (which leads to an invalid card later).
unsigned int stoiWithCheck(const string& str)
{
    bool is_numeric = true;
    for(unsigned int i = 0; i < str.length(); ++i)
    {
        if(not isdigit(str.at(i)))
        {
            is_numeric = false;
            break;
        }

    }
    if(is_numeric)
    {
        return stoi(str);
    }

    else
    {
        return 0;
    }
}

// Täyttää pelilaudan (kooltaan rows * columns) tyhjillä korteilla.
//
// Fills the game board, the size of which is rows * columns, with empty cards.
void initWithEmpties(Game_board_type& g_board, unsigned int rows, unsigned int columns)
{
    g_board.clear();
    Game_row_type row;
    for(unsigned int i = 0; i < columns; ++i)
    {
        Card card;
        row.push_back(card);
    }
    for(unsigned int i = 0; i < rows; ++i)
    {
        g_board.push_back(row);
    }
}

// Etsii seuraavan tyhjän kohdan pelilaudalta (g_board) aloittamalla
// annetusta kohdasta start ja jatkamalla tarvittaessa alusta.
// (Kutsutaan vain funktiosta initWithCards.)
//
// Finds the next free position in the game board (g_board), starting from the
// given position start and continuing from the beginning if needed.
// (Called only by the function initWithCards.)
unsigned int nextFree(Game_board_type& g_board, unsigned int start)
{
    // Selvitetään annetun pelilaudan rivien ja sarakkeiden määrät
    //
    // Finding out the number of rows and columns of the game board
    unsigned int rows = g_board.size();
    unsigned int columns = g_board.at(0).size();

    // Aloitetaan annetusta arvosta
    //
    // Starting from the given value
    for(unsigned int i = start; i < rows * columns; ++i)
    {
        if(g_board.at(i / columns).at(i % columns).getVisibility() == EMPTY) // vaihdettu
        {
            return i;
        }
    }
    // Jatketaan alusta
    //
    // Continuing from the beginning
    for(unsigned int i = 0; i < start; ++i)
    {
        if(g_board.at(i / columns).at(i % columns).getVisibility() == EMPTY)
        {
            return i;
        }
    }
    // Tänne ei pitäisi koskaan päätyä
    //
    // You should never reach this
    std::cout << "No more empty spaces" << std::endl;
    return rows * columns - 1;
}

// Alustaa annetun pelilaudan (g_board) satunnaisesti arvotuilla korteilla
// annetun siemenarvon (seed) perusteella.
//
// Initializes the given game board (g_board) with randomly generated cards,
// based on the given seed value.
void initWithCards(Game_board_type& g_board, int seed)
{
    // Selvitetään annetun pelilaudan rivien ja sarakkeiden määrät
    //
    // Finding out the number of rows and columns of the game board
    unsigned int rows = g_board.size();
    unsigned int columns = g_board.at(0).size();

    // Arvotaan täytettävä sijainti
    //
    // Drawing a cell to be filled
    std::default_random_engine randomEng(seed);
    std::uniform_int_distribution<int> distr(0, rows * columns - 1);
    // Hylätään ensimmäinen satunnaisluku (joka on aina jakauman alaraja)
    //
    // Wiping out the first random number (that is always the lower bound of the distribution)
    distr(randomEng);

    // Jos arvotussa sijainnissa on jo kortti, valitaan siitä seuraava tyhjä paikka.
    // (Seuraava tyhjä paikka haetaan kierteisesti funktion nextFree avulla.)
    //
    // If the drawn cell is already filled with a card, next empty cell will be used.
    // (The next empty cell is searched for circularly, see function nextFree.)
    for(unsigned int i = 0, c = 'A'; i < rows * columns - 1; i += 2, ++c)
    {
        // Lisätään kaksi samaa korttia (parit) pelilaudalle
        //
        // Adding two identical cards (pairs) in the game board
        for(unsigned int j = 0; j < 2; ++j)
        {
            unsigned int cell = distr(randomEng);
            cell = nextFree(g_board, cell);
            g_board.at(cell / columns).at(cell % columns).setLetter(c);
            g_board.at(cell / columns).at(cell % columns).setVisibility(HIDDEN);
        }
    }
}

// Tulostaa annetusta merkistä c koostuvan rivin,
// jonka pituus annetaan parametrissa line_length.
// (Kutsutaan vain funktiosta print.)
//
// Prints a line consisting of the given character c.
// The length of the line is given in the parameter line_length.
// (Called only by the function print.)
void printLineWithChar(char c, unsigned int line_length)
{
    for(unsigned int i = 0; i < line_length * 2 + 7; ++i)
    {
        cout << c;
    }
    cout << endl;
}

// Tulostaa vaihtelevankokoisen pelilaudan reunuksineen.
//
// Prints a variable-length game board with borders.
void print(const Game_board_type& g_board)
{
    // Selvitetään annetun pelilaudan rivien ja sarakkeiden määrät
    //
    // Finding out the number of rows and columns of the game board
    unsigned int rows = g_board.size();
    unsigned int columns = g_board.at(0).size();

    printLineWithChar('=', columns);
    cout << "|   | ";
    for(unsigned int i = 0; i < columns; ++i)
    {
        cout << i + 1 << " ";
    }
    cout << "|" << endl;
    printLineWithChar('-', columns);
    for(unsigned int i = 0; i < rows; ++i)
    {
        cout << "| " << i + 1 << " | ";
        for(unsigned int j = 0; j < columns; ++j)
        {
            g_board.at(i).at(j).print();
            cout << " ";
        }
        cout << "|" << endl;
    }
    printLineWithChar('=', columns);

}

// Kysyy käyttäjältä tulon ja sellaiset tulon tekijät, jotka ovat
// mahdollisimman lähellä toisiaan.
//
// Asks the desired product from the user, and calculates the factors of
// the product such that the factor as near to each other as possible.
void ask_product_and_calculate_factors(unsigned int& smaller_factor, unsigned int& bigger_factor)
{
    unsigned int product = 0;
    while(not (product > 0 and product % 2 == 0))
    {
        std::cout << INPUT_AMOUNT_OF_CARDS;
        string product_str = "";
        std::getline(std::cin, product_str);
        product = stoiWithCheck(product_str);
    }

    for(unsigned int i = 1; i * i <= product; ++i)
    {
        if(product % i == 0)
        {
            smaller_factor = i;
        }
    }
    bigger_factor = product / smaller_factor;
}


std::vector<std::string> splitNames(std::string line) {
    std::vector <std::string> names_list;

    size_t begin = 0;
    size_t end = 0;
    // Performs a split, finds each space in string and takes every name and adds them to a vector
    // After the whole string has been looped, returns the splitted vector
    while (end != line.size()) {
        end = line.find(" ", begin);
        if (end == std::string::npos) {
            end = line.size();

        }
        if (begin != end) {
            names_list.push_back(line.substr(begin, end-begin));
        begin = end + 1;
    }

}
    return names_list;
}


std::vector<std::string> playerNames(int player_amount) {

    string player_name = ""; 
    // Asks the user to input names for the players
    std::cout << "List " <<player_amount << " players: ";
    std::getline(std::cin, player_name);
    std::vector<std::string> names = splitNames(player_name);




    return names;
}




bool quitFunction(std::string line) {

    // Function is used to find and detect if user inputs a "q" to indicate the program to shut down.
    // Returns a bool which uses EXIT_SUCCESS in main function.
    std::vector <std::string> temp_vector;
    size_t begin = 0;
    size_t end = 0;

    while (end != line.size()) {
        end = line.find(" ", begin);
        if (end == std::string::npos) {
            end = line.size();
        }
        if (begin != end) {
            temp_vector.push_back(line.substr(begin,end-begin));

        }
        begin = end + 1;
    }
    for (string i : temp_vector) {
        if (i == "q") {
            std::cout << GIVING_UP << endl;
            return true;
        }
    }
    return false;
}





std::vector<std::string> splitCoordinates(std::string line) {

    // Takes the coordinates and splits them to a vector as chars. Each coordinate is checked through stoiWithCheck
    // which returns 0 if there was a false input.

    std::vector <std::string> coordinate_vector;
    size_t begin = 0;
    size_t end = 0;
    while (end != line.size()) {
        end = line.find(" ", begin);
        if (end == std::string::npos) {
            end = line.size();
        }
        if (begin != end) {

            int number = stoiWithCheck(line.substr(begin, end-begin));
            if (number > 0) {
                 coordinate_vector.push_back(line.substr(begin, end-begin));                 

            } else {
                std::cout << INVALID_CARD << endl;               
                break;


        }
        begin = end + 1;
    }

}
    return coordinate_vector;

}

bool canContinue(std::string line, bool can_continue) {

    // Determinates boolean can_continue which informs the program if the coordinate input was valid.
    size_t begin = 0;
    size_t end = 0;

    while (end != line.size()) {
        end = line.find(" ", begin);
        if (end == std::string::npos) {
            end = line.size();
        }
        if (begin != end) {
            int number = stoiWithCheck(line.substr(begin, end-begin));
            if (number > 0) {
                 can_continue = true;

            } else {

                can_continue = false;
                break;


        }
        begin = end + 1;
    }

}
    return can_continue;

}

bool coordinateBorders(std::vector<int> int_vector, Game_board_type& game_board, bool can_continue) {


    // Function checks if the coordinates are in the game board. If not, prints INVALID_CARD and makes the user
    // to input new coordinates.
    if (int_vector.size() == 4) {
        if(int_vector.at(0) <= int(game_board.at(0).size()) && int_vector.at(2) <= int(game_board.at(0).size())) {
            if(int_vector.at(1) <= int(game_board.size()) && int_vector.at(3) <= int(game_board.size())) {
                if (!(int_vector.at(0) == int_vector.at(2) && int_vector.at(1) == int_vector.at(3))) {

                } else {
                        std::cout << INVALID_CARD << endl;
                        can_continue = false;
            }
        } else {
                std::cout << INVALID_CARD << endl;
                can_continue = false;
        }
    } else {
            std::cout << INVALID_CARD << endl;
            can_continue = false;
    }
    
    }
    else {
        std::cout << INVALID_CARD << endl;
        can_continue = false;

}
    return can_continue;
}

bool hasWon(Game_board_type& game_board) {


    // Function loops every card of the game board and checks if every card has been already turned.
    // If so, returns boolean and tells the program to indicate the winner.
    int x_size = game_board.at(0).size();
    int y_size = game_board.size();

    for( int y = 0; y < y_size; ++y ) {
        for( int x = 0; x < x_size; ++x ) {
            if( game_board.at(y).at(x).getVisibility() != EMPTY ) {
                return false;
            }
        }
    }
    return true;
}
int turnCard(vector<int>& int_vector,vector <Player>& player_vector, Game_board_type& game_board, bool can_continue,int current_player) {

    // Function used to flip each card pair and to add points if player guesses to correct cards.
    // Also prints standings after each turn.

    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;


    if (int_vector.size() == 4) {
        x1 = int_vector.at(0)-1;
        y1 = int_vector.at(1)-1;
        x2 = int_vector.at(2)-1;
        y2 = int_vector.at(3)-1;
    if (can_continue) {
        // Checks if the card is still hidden, if not asks the player again.
        if (game_board.at(y1).at(x1).getVisibility() != EMPTY) {
            game_board.at(y1).at(x1).turn();
        } else {
            std::cout << INVALID_CARD << std::endl;
            return current_player-1;
        }

        if (game_board.at(y2).at(x2).getVisibility() != EMPTY) {
            game_board.at(y2).at(x2).turn();
        } else {
            std::cout << INVALID_CARD << std::endl;
            return current_player-1;
        }


        // Prints the game board to show how the player guessed.
        print(game_board);
        // Checks if the player found matching cards, if did, will add it to players pairs total.
        if (game_board.at(y1).at(x1).getLetter() != game_board.at(y2).at(x2).getLetter()) {
             std::cout << NOT_FOUND << endl;            

             game_board.at(y1).at(x1).turn();
             game_board.at(y2).at(x2).turn();

         } else {
            std::cout << FOUND << endl;

            player_vector.at(current_player).addCard(game_board.at(y1).at(x1));
            player_vector.at(current_player).addCard(game_board.at(y2).at(x2));


            game_board.at(y1).at(x1).removeFromGameBoard();
            game_board.at(y2).at(x2).removeFromGameBoard();
            current_player -= 1;


    }
        // Prints the current standings of pairs that the players have.
        for (Player& player : player_vector) {
            player.print();
        }
        print(game_board);



        }


}
    return current_player;
}
bool whoWon(vector<Player> player_vector, bool game_loop) {


    // Calculates the highest pair amount and takes the index of the winner in player vector.
    // Returns boolean to make game_loop false, so the program will stop.
    // Also checks if there is tied players and prints accordingly.

    std::cout << GAME_OVER << std::endl;
    int highest_pairs = 0;
    int winner = 0;
    // Player can't be tied alone, so tied value is 1 from the start.
    int tied = 1;
    bool is_tie = false;
    for(int player = 0; player < int(player_vector.size()); ++player) {
        if (int(player_vector.at(player).numberOfPairs()) > highest_pairs) {
            highest_pairs = int(player_vector.at(player).numberOfPairs());
            winner = player;
        } else {

            if (int(player_vector.at(player).numberOfPairs()) == highest_pairs) {
                tied += 1;
                is_tie = true;
            } else {
                continue;

            }

        }
    }
    if (is_tie) {
        std::cout << "Tie of " << tied << " players with " << player_vector.at(winner).numberOfPairs() << " pairs." << std::endl;
    } else {
        std::cout << player_vector.at(winner).getName() << " has won with " << highest_pairs << " pairs." << std::endl;
    }
    return game_loop = false;
}

int main()
{
    Game_board_type game_board;

    unsigned int factor1 = 1;
    unsigned int factor2 = 1;
    ask_product_and_calculate_factors(factor1, factor2);
    initWithEmpties(game_board, factor1, factor2);

    string seed_str = "";
    std::cout << INPUT_SEED;
    std::getline(std::cin, seed_str);
    int seed = stoiWithCheck(seed_str);
    initWithCards(game_board, seed);

    // Takes the player amount and checks that it is valid
    string players_str = "";
    std::vector<string> names;
    unsigned int player_amount = 0;
    while(not (player_amount > 0))
    {
        std::cout << INPUT_AMOUNT_OF_PLAYERS;
        string players_str = "";
        std::getline(std::cin, players_str);
        player_amount = stoiWithCheck(players_str);

    }
    // Asks the player names
    names = playerNames(player_amount);

    //Prints the starting game board.
    print(game_board);

    // Creating a player_vector which consists of objects
    vector<Player> player_vector = {};
    for (string i : names) {
        Player new_player = Player(i);
        player_vector.push_back(new_player);
    }

    // Index for controlling who is in turn.
    int current_player = 0;


    // Runs through the player list and asks coordinates from each player, if unvalid coordinates are
    // given, program prints INVALID_CARD and asks the player to give them again.
    bool game_loop = true;
    while (game_loop) {

        for (current_player = 0; current_player < int(player_vector.size()); ++current_player) {

            bool can_continue = false;
            while (!can_continue) {

                string coordinates = "";
                std::cout << player_vector.at(current_player).getName()<< ": " << INPUT_CARDS;
                std::getline(std::cin, coordinates);
                if (quitFunction(coordinates)) {
                    return EXIT_SUCCESS;
                }
                std::vector<string> coordinate_vector = splitCoordinates(coordinates);

                if (!canContinue(coordinates,can_continue)) {
                    can_continue = false;
                } else {
                    can_continue = true;
                }

                // Forms an int_vector which contains the coordinates as ints, so they can be referenced.
                std::vector<int> int_vector;
                for (string i : coordinate_vector) {
                    int number =  stoiWithCheck(i);
                    int_vector.push_back(number);

                    // Checks if the coordinates are in boundaries of the game board.
                    can_continue = coordinateBorders(int_vector, game_board, can_continue);


    }               // Basically a turn function which turns two cards and checks if they match.
                current_player = turnCard(int_vector,player_vector,game_board,can_continue, current_player);
                if (hasWon(game_board)) {
                    // whoWon loop checks the status of the game board to determine if the game is over.
                    game_loop = whoWon(player_vector, game_loop);
                    return EXIT_SUCCESS;

            }

        }

    }

}
   return EXIT_SUCCESS;

}

















