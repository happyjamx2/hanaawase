#ifndef INCLUDE_HANAAWASE
#define INCLUDE_HANAAWASE

#define DECK 49
#define MONTH 13

#define CAPACITY 6
#define GUEST 3
#define BAFUDA 6
#define TEFUDA 7

#define CHARACTER1 14    //  必ず14以上の値とする
#define CHARACTER2 16
#define CHARACTER3 20  //  必ず20以上の値とする
#define COLUMN 2
#define NAME 8          //  最も長いプレイヤーの名前の半角文字数
#define SIZE 32

/*
#define GUEST 2
#define BAFUDA 8
#define TEFUDA 10

#define GUEST 3
#define BAFUDA 6
#define TEFUDA 7

#define GUEST 4
#define BAFUDA 8
#define TEFUDA 5

#define GUEST 5
#define BAFUDA 8
#define TEFUDA 4

#define GUEST 6
#define BAFUDA 12
#define TEFUDA 3
*/

typedef enum PlayerID {
    YOU, COM1, COM2, COM3, COM4, COM5, COM6
} playerID;

typedef enum status {
    DONE, RETRY
} status;

typedef struct Field {
    int bafuda[SIZE]; //  場札の通し番号保存用配列
    int bMonth[SIZE]; //  場札のスート保存用配列
    int qBafuda;      //  場札の枚数
} field;

typedef struct Players {
    int motifuda[SIZE]; //  持札の通し番号保存用配列
    int mMonth[SIZE];   //  場札のスート保存用配列
    int tefuda[TEFUDA]; //  手札の通し番号保存用配列
    int tMonth[TEFUDA]; //  手札のスート保存用配列
    int flag;
    int qMotifuda;  //  持札の枚数
    int qTefuda;    //  手札の枚数
    int handPoint;  //  役の点数
    int score;      //  合計点
    int totalScore; //  最終合計点
} players;

typedef struct Game {
    int fuda[DECK];
    playerID order[GUEST];
    playerID ranking[GUEST];
    status judgement;
} game;

typedef struct PlayerName {
    char string[CAPACITY][SIZE];
    int length[CAPACITY];
} playerName;

typedef struct Card {
    char string[DECK][SIZE]; //  花札の名前
    int length[DECK];        //  名前の長さ
    int point[DECK];         //  花札の点数
} card;

typedef struct Color {
    int r;
    int g;
    int b;
} color;

void virtualTerminal(void);
void escapeSequence(char const *string, char const character);
void changeColorRGB(const int *code);
void resetColor(void);
void moveCursorRight(int column);
void setValue(field *ba, players *player, game *hanaawase);
int  generateNumber(game *hanaawase);
void printPlayerName(int playerId);
void printFudaName(const int *cardId, const int *suit, int length);
void decideTurn(players *player, game *hanaawase);
void convertToSuit(int *cardId, int *suit, int start, int end);
void dealHanahuda(field *ba, players *player, game *hanaawase);
void printHyphen(const int *count);
void printHanafuda(const field *ba, const players *player, const playerID *playerId);
void selectTefuda(field *ba, players *player, playerID *playerId);
void moveHanafuda(field *ba, players *player, playerID *playerId, int *cardId, int *suit);
void playTefuda(field *ba, players *player, playerID *playerId);
void drawYamafuda(field *ba, players *player, playerID *playerId, game *hanaawase);
void judgeHand(players *player, playerID *playerId, game *hanaawase);
void calculateScore(players *player);
void decideRanking(players *player, game *hanaawase);

extern const card hanafuda;
extern const playerName name;
extern const color RGB[MONTH];

extern const int finish;

#endif//INCLUDE_HANAAWASE