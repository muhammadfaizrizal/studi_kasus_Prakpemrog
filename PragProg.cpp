#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>

using namespace std;

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define ORANGE "\033[38;5;208m"


int flip = 0;

struct Card{
    char symbol;
    bool opened;
};

struct Score{
    string name;
    int moves;
    int time;
};


Card board[3][4];
char cardSymbols[6];
char boardSymbols[12];
int moves = 0;
int totalOpened = 0;
const int maxCards = 12;

void clearL(){
    system("clear");
}

void loadCardSymbols(){
    ifstream file("cardset.txt");
    if(!file.is_open()){
        cout << "File cardset.txt tidak ditemukan!" << endl;
        cout << "Buat file cardset.txt dengan isi: A B C D E F G H" << endl;
        exit(1);
    }

    for(int i = 0; i < 6; i++){
        file >> cardSymbols[i];
    }
    file.close();
}

void duplicateCards(){
    int index = 0;
    for(int i = 0; i < 6; i++){
       boardSymbols[index] = cardSymbols[i];
        index++;
        boardSymbols[index] = cardSymbols[i];
        index++;
    }
}

void shuffleBoard(){
    for(int i = 0; i < maxCards; i++){
        int randomIndex = rand() % maxCards;
        char temp = boardSymbols[i];
        boardSymbols[i] = boardSymbols[randomIndex];
        boardSymbols[randomIndex] = temp;
    }
}

void fillBoard(){
    int index = 0;
    for (int r = 0; r < 3; r++){
        for (int c = 0; c < 4; c++) {
            board[r][c].symbol = boardSymbols[index];
            board[r][c].opened = false;
            index++;
        }
    }
}

void printBoard(){
    string color;

    if(flip % 2 == 0){
        color = GREEN;
    }else{
        color = YELLOW;
    }

    flip++;
    cout << "\n    \t\t    0 1 2 3" << endl;
    for(int r = 0; r < 3; r++){
        cout << "\t\t";
        cout << r << " | ";
        for(int c = 0; c < 4; c++){
            if(board[r][c].opened){
                cout << color << board[r][c].symbol << RESET;
            }else{
                cout << "*";
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void getValidCoordinate(int &row, int &col){
    do{
        cout << "\tMasukkan baris dan kolom : ";
        cin >> row >> col;

        if(row < 0 || row > 2 || col < 0 || col > 3){
            cout << "\tInput di luar range! Ulangi.\n";
        }else if(board[row][col].opened){
            cout << "\tKartu sudah terbuka! Pilih yang lain.\n";
        }
    }while(row < 0 || row > 2 || col < 0 || col > 3 || board[row][col].opened);
}

bool checkMatch(int r1, int c1, int r2, int c2){
    if(board[r1][c1].symbol == board[r2][c2].symbol){
        board[r1][c1].opened = true;
        board[r2][c2].opened = true;
        totalOpened += 2;
        return true;
    }else{
        return false;
    }
}

void saveScore(string name, int moves, int duration){
    ofstream file("score.txt", ios::app);
    file << name << " " << moves << " " << duration << endl;
    file.close();
}

void showHighScore(){
    ifstream file("score.txt");
    if(!file.is_open()){
        cout << "\n\nBelum ada skor tersimpan!\n\n";
        return;
    }

    Score scores[100];
    int count = 0;
    while (file >> scores[count].name >> scores[count].moves >> scores[count].time) {
        count++;
    }
    file.close();

    for(int i = 0; i < count - 1; i++){
        for(int j = i + 1; j < count; j++){
            if((scores[i].moves > scores[j].moves) || (scores[i].moves == scores[j].moves && scores[i].time > scores[j].time)) {
                Score tmp = scores[i];
                scores[i] = scores[j];
                scores[j] = tmp;
            }
        }
    }

    clearL();
    cout << "\n================== HIGH SCORE ==================\n\n";
    int limit = (count < 5) ? count : 5;
    for(int i = 0; i < limit; i++){
        cout << i+1 << ". " << scores[i].name << " - " << scores[i].moves << " langkah - " << scores[i].time << " detik\n";
    }
    cout << endl;
    cout << "================================================\n\n";
}


int main() {
    srand(time(0));
    int menu;

    cout << "================================================\n";
    cout << "=============== MEMORY CARD GAME ===============\n";
    cout << "================================================\n";
    do{
        cout << "1. Main Game\n";
        cout << "2. Lihat High Score\n";
        cout << "3. Keluar\n";
        cout << "Pilih menu: ";
        cin >> menu;
        
        if(menu == 1){
            moves = 0;
            totalOpened = 0;
            
            loadCardSymbols();
            duplicateCards();
            shuffleBoard();
            fillBoard();
            
            string nama;
            cout << "Masukkan nama pemain: ";
            cin >> nama;
            auto start = chrono::high_resolution_clock::now();
            
            int r1, c1, r2, c2;
            
            while(totalOpened < maxCards){
                clearL();
                cout << "================================================\n";
                cout << "=============== MEMORY CARD GAME ===============\n";
                cout << "================================================\n";
                printBoard();
                cout << BLUE << "\tPilih kartu pertama!\n" << RESET;
                getValidCoordinate(r1, c1);
                board[r1][c1].opened = true;
                printBoard();

                cout << BLUE << "\tPilih kartu kedua!\n" << RESET;
                getValidCoordinate(r2, c2);
                board[r2][c2].opened = true;
                printBoard();
        
                moves++;
        
                if(!checkMatch(r1, c1, r2, c2)){
                    cout << "\tTidak cocok! tekan Enter untuk lanjut...";
                    cin.get(); cin.get();
                    board[r1][c1].opened = false;
                    board[r2][c2].opened = false;
                }else{
                    cout << "Cocok!\n";
                }

            }
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::seconds>(end - start);
            cout << "\n Selamat! Kamu menang dengan " << moves << " langkah!" << " dengan durasi" << duration.count() << endl;
            saveScore(nama, moves, duration.count());
            showHighScore();
        } else if(menu == 2){
            showHighScore();
        }
    }while(menu != 3);

    cout << "Terima kasih telah bermain!\n";
    return 0;
}
