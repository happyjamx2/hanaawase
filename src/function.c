#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/hanaawase.h"

//  Virtual Terminal機能を有効にする
void virtualTerminal( void ) {

    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(stdOut, &consoleMode);
    SetConsoleMode(stdOut, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

//  エスケープシーケンスを標準出力する
void escapeSequence( char const *string, char const character ) {

    printf("\x1b[");
    printf(string);
    putchar(character);
}

//  RGBを指定して文字の色を変更する
void changeColorRGB( const int *code ) {

    char buf[64];

    snprintf(buf, 64, "38;2;%d;%d;%d", RGB[*code].r, RGB[*code].g, RGB[*code].b);
    escapeSequence(buf, 'm');
}

//  色情報をリセットする
void resetColor( void ) {

    escapeSequence("0", 'm');
}

//  カーソルを右に移動する
void moveCursorRight( int column ) {

    char buf[8];

    snprintf(buf, 8, "%d", column);
    escapeSequence(buf, 'C');
}

//  値をセットする
void setValue( field *ba, players *player, game *hanaawase ) {

    for (int i = 0; i < SIZE; i++) {
        ba->bafuda[i] = 0;
        ba->bMonth[i] = 0;
    }
    ba->qBafuda = 0;

    for (int i = 0; i < GUEST; i++) {
        for (int j = 0; j < SIZE; j++) {
            player[i].motifuda[j] = 0;
            player[i].mMonth[j] = 0;
        }
        for (int j = 0; j < TEFUDA; j++) {
            player[i].tefuda[j] = 0;
            player[i].tMonth[j] = 0;
        }
        player[i].flag = 0;
        player[i].qMotifuda = 0;
        player[i].qTefuda = 0;
        player[i].handPoint = 0;
    }

    for (int i = 0; i < DECK; i++) {
        hanaawase->fuda[i] = 0;
    }
    hanaawase->judgement = DONE;
}

//  1~48を重複しないでランダムに生成する
int  generateNumber( game *hanaawase ) {

    int N;

    do {
        N = rand() % 48 + 1;
    } while (hanaawase->fuda[N]);
    hanaawase->fuda[N]++;

    return N;
}

//  プレーヤーの名前を表示する
void printPlayerName( int playerId ) {

    for (int i = 0; name.string[playerId][i] != '\0'; i++) {
        printf("%c", name.string[playerId][i]);
    }
}

//  札の名前を表示する
void printFudaName( const int *cardId, const int *suit, int length ) {

    int column;

    if (length) {
        column = (length - hanafuda.length[*cardId]) / 2;
        if (column) {
            moveCursorRight(column);
        }
    }
    changeColorRGB(suit);
    for (int i = 0; hanafuda.string[*cardId][i] != '\0'; i++) {
        printf("%c", hanafuda.string[*cardId][i]);
    }
    resetColor();
    if (length) {
        if (column) {
            moveCursorRight(column);
        }
    }
}

//  順番を決める
void decideTurn( players *player, game *hanaawase ) {

    static int count = 0;

    if (count) {

        int tmp[GUEST] = {0};
        int index = 0;
        int playerId;
        int point = -10000;
        int start;

        for (int i = 0; i < GUEST; i++) {
            if (point < player[i].score) {
                point = player[i].score;
                playerId = i;
            }
        }
        for (int i = 0; i < GUEST; i++) {
            if (playerId == hanaawase->order[i]) {
                start = i;
                break;
            }
        }        
        for (int i = start; i < GUEST; i++) {
            tmp[index] = hanaawase->order[i];
            index++;
        }
        for (int i = 0; i < start; i++) {
            tmp[index] = hanaawase->order[i];
            index++;
        }
        for (int i = 0; i < GUEST; i++) {
            hanaawase->order[i] = tmp[i];
        }
    } else {

        status decision;
        char sign = '"';
        int flg[GUEST];
        int month[GUEST];
        int number[GUEST];
        int cardId;
        int column;
        int point;
        int suit;

        do {
            decision = DONE;
            for (int i = 0; i < GUEST; i++) {
                printPlayerName(i);
                column = NAME - name.length[i];
                if (column) {
                    moveCursorRight(column);
                }
                printf(": %c", sign);
                cardId = generateNumber(hanaawase);
                suit = (cardId - 1) % 12 + 1;
                printFudaName(&cardId, &suit, CHARACTER1);
                printf("%c を引きました。\n", sign);
                number[i] = cardId;
                month[i] = suit;
            }
            for (int i = 0; i < GUEST - 1; i++) {    
                for (int j = i + 1; j < GUEST; j++) {
                    if (month[i] == month[j] && hanafuda.point[number[i]] == hanafuda.point[number[j]]) {
                        decision = RETRY;
                        break;
                    }
                }
                if (decision) {
                    printf("\nもう1度やり直します。\n");
                    break;
                }
            }
        } while (decision);
        for (int i = 0; i < GUEST; i++) {
            flg[i] = 1;
        }
        for (int i = 0; i < GUEST; i++) {
            suit = MONTH;
            point = -10000;
            for (int j = 0; j < GUEST; j++) {
                if (flg[j]) {
                    if (suit > month[j]) {
                        suit = month[j];
                        point = hanafuda.point[number[j]];
                        hanaawase->order[i] = j;
                    } else if (suit == month[j] && point < hanafuda.point[number[j]]) {
                        suit = month[j];
                        point = hanafuda.point[number[j]];
                        hanaawase->order[i] = j;
                    }
                }
            }
            flg[hanaawase->order[i]] = 0;
        }
        for (int i = 0; i < DECK; i++) {
            hanaawase->fuda[i] = 0;
        }
    }
    count++;

    printf("\n順番は以下の通りになりました。\n");
    for (int i = 0; i < GUEST; i++) {
        printf("%d.", i + 1);
        printPlayerName(hanaawase->order[i]);
        printf("\n");
    }
}

//  スートに変換する
void convertToSuit( int *cardId, int *suit, int start, int end ) {

    for (int i = start; i < end; i++) {
        suit[i] = 0;
    }
    for (int i = start; i < end; i++) {
        suit[i] = (cardId[i] - 1) % 12 + 1;
    }
}

//  花札を配る
void dealHanahuda( field *ba, players *player, game *hanaawase ) {

    status deal;
    int suit[MONTH] = {0};

    while (1) {
        deal = DONE;
        for (int i = 0; i < BAFUDA; i++) {
            ba->bafuda[i] = generateNumber(hanaawase);
            ba->qBafuda++;
        }
        convertToSuit(ba->bafuda, ba->bMonth, 0, ba->qBafuda);
        for (int i = 0; i < ba->qBafuda; i++) {
            suit[ba->bMonth[i]]++;
        }
        for (int i = 0; i < MONTH; i++) {
            if (suit[i] >= 4) {
                deal = RETRY;
            }
            suit[i] = 0;
        }
        for (int i = 0; i < GUEST; i++) {
            for (int j = 0; j < TEFUDA; j++) {
                player[i].tefuda[j] = generateNumber(hanaawase);
                player[i].qTefuda++;
            }
            convertToSuit(player[i].tefuda, player[i].tMonth, 0, player[i].qTefuda);
            for (int j = 0; j < player[i].qTefuda; j++) {
                suit[player[i].tMonth[j]]++;
            }
            for (int j = 0; j < MONTH; j++) {
                if (suit[j] >= 4) {
                    deal = RETRY;
                }
                suit[j] = 0;
            }
        }
        if (!deal) {
            break;
        }
        printf("\n札が4枚揃ったので配り直します。\n");
        setValue(ba, player, hanaawase);
    }
}

//  ハイフンを表示する
void printHyphen( const int *count ) {

    for (int i = 0; i < *count; i++) {
        printf("-");
    }
}

//  場札、手札、持札を表示
void printHanafuda( const field *ba, const players *player, const playerID *playerId ) {

    int cardId;
    int column;
    int count;
    int maxLine;

    printf("\n");
    count = (CHARACTER2 - 4) / 2;
    printHyphen(&count);
    printf("場札");
    printHyphen(&count);
    moveCursorRight(COLUMN);
    count = (CHARACTER3 - name.length[*playerId] - 6) / 2;
    printHyphen(&count);
    printPlayerName(*playerId);
    printf("の手札");
    printHyphen(&count);
    for (int i = 0; i < GUEST; i++) {
        moveCursorRight(COLUMN);
        count = (CHARACTER2 - name.length[i] - 6) / 2;
        printHyphen(&count);
        printPlayerName(i);
        printf("の持札");
        printHyphen(&count);
    }
    printf("\n");

    maxLine = ba->qBafuda;
    for (int i = 0; i < GUEST; i++) {
        if (maxLine < player[i].qTefuda) {
            maxLine = player[i].qTefuda;
        }
    }
    for (int i = 0; i < GUEST; i++) {
        if (maxLine < player[i].qMotifuda) {
            maxLine = player[i].qMotifuda;
        }
    }
    for (int line = 0; line < maxLine; line++) {
        if (line >= ba->qBafuda) {
            moveCursorRight(CHARACTER2);
        } else {
            printFudaName(&(ba->bafuda[line]), &(ba->bMonth[line]), CHARACTER2);
        }
        moveCursorRight(COLUMN);
        if (line >= player[*playerId].qTefuda) {
            moveCursorRight(CHARACTER3);
        } else {
            if (line <= 8) {
                printf(" ");
            }
            printf("%d.", line + 1);
            cardId = player[*playerId].tefuda[line];
            column = (CHARACTER3 - hanafuda.length[cardId]) / 2 - 3;
            if (column) {
                moveCursorRight(column);
            }
            printFudaName(&(player[*playerId].tefuda[line]), &(player[*playerId].tMonth[line]), 0);
            column = (CHARACTER3 - hanafuda.length[cardId]) / 2;
            moveCursorRight(column);
        }
        for (int i = 0; i < GUEST; i++) {
            moveCursorRight(COLUMN);
            if (line >= player[i].qMotifuda) {
                moveCursorRight(CHARACTER2);
            } else {
                printFudaName(&(player[i].motifuda[line]), &(player[i].mMonth[line]), CHARACTER2);
            }
        }
        printf("\n");
    }
}

//  場に出す札を決める
void selectTefuda( field *ba, players *player, playerID *playerId ) {

    char sign = '"';
    int cardId = 100;
    int flag = -1;
    int input;
    int qBafuda = ba->qBafuda;
    int qTefuda = player[*playerId].qTefuda;

    printf("\n");
    printPlayerName(*playerId);
    printf("の番です。\n");

    switch (*playerId) {
        case YOU:
            printHanafuda(ba, player, playerId);
            printf("\n何枚目の札を場に出しますか? ");
            while (1) {
                scanf("%d", &input);
                if (input > 0 && input <= qTefuda) {
                    break;
                }
                printf("有効な値ではありません。");
            }
            input--;
            flag = input;
            break;
        case COM1:
        case COM2:
        case COM3:
        case COM4:
        case COM5:
        case COM6:
            for (int i = 0; i < qTefuda; i++) {
                for (int j = 0; j < qBafuda; j++) {
                    if (player[*playerId].tMonth[i] == ba->bMonth[j]) {
                        if (cardId > ba->bafuda[j]) {
                            cardId = ba->bafuda[j];
                            flag = i;
                        }
                    }
                }
            }
            if (flag == -1) {
                cardId = 0;
                for (int i = 0; i < qTefuda; i++) {
                    if (cardId < player[*playerId].tefuda[i]) {
                        cardId = player[*playerId].tefuda[i];
                        flag = i;
                    }
                }
            }
            printf("%c", sign);
            printFudaName(&(player[*playerId].tefuda[flag]), &(player[*playerId].tMonth[flag]), CHARACTER1);
            printf("%c を出しました。\n", sign);
            break;
        default:
            break;
    }
    player[*playerId].flag = flag;
}

//  札を移動する
void moveHanafuda( field *ba, players *player, playerID *playerId, int *cardId, int *suit ) {

    int select[3] = {0};
    int count = 0;
    int qBafuda = ba->qBafuda;
    int qTefuda = player[*playerId].qTefuda;
    int qMotifuda = player[*playerId].qMotifuda;

    for (int i = 0; i < qBafuda; i++) {
        if (*suit == ba->bMonth[i]) {
            select[count] = i;
            count++;
        }
    }
    
    if (count == 3) {

        int tmp[BAFUDA] = {0};
        int index = 0;

        for (int i = 0; i < count; i++) {
            player[*playerId].motifuda[qMotifuda] = ba->bafuda[select[i]];
            qMotifuda++;
        }
        for (int i = 0; i < qBafuda; i++) {
            if (i != select[0] && i != select[1] && i != select[2]) {
                tmp[index] = ba->bafuda[i];
                index++;
            }
        }
        for (int i = 0; i < qBafuda; i++) {
            ba->bafuda[i] = tmp[i];
        }
        qBafuda -= count;
        player[*playerId].motifuda[qMotifuda] = *cardId;
        qMotifuda++;
        convertToSuit(ba->bafuda, ba->bMonth, 0, qBafuda);
        convertToSuit(player[*playerId].motifuda, player[*playerId].mMonth, player[*playerId].qMotifuda, qMotifuda);
    } else {

        int column;
        int input;
        int flag;

        if (count == 2) {
            if (*playerId == YOU) {
                printf("\nどの札に合わせますか?\n");
                for (int i = 0; i < count; i++) {
                    printf("%d.", i + 1);
                    column = (CHARACTER1 - hanafuda.length[ba->bafuda[select[i]]]) / 2;
                    if (column) {
                        moveCursorRight(column);
                    }
                    printFudaName(&(ba->bafuda[select[i]]), &(ba->bMonth[select[i]]), 0);              
                    printf("\n");
                }
                while (1) {
                    scanf("%d", &input);
                    if (input > 0 && input <= count) {
                        break;
                    }
                    printf("有効な値ではありません。");
                }
                input--;
                flag = select[input];
            } else if (*playerId != YOU) {
                flag = select[0];
                if (ba->bafuda[flag] > ba->bafuda[select[1]]) {
                    flag = select[1];
                }
            }
        } else if (count == 1) {
            flag = select[0];
        }
        if (count) {  /* 場札と一致する手札がある場合 */
            player[*playerId].motifuda[qMotifuda] = ba->bafuda[flag];
            qMotifuda++;
            player[*playerId].motifuda[qMotifuda] = *cardId;
            qMotifuda++;
            qBafuda--;
            for (int i = flag; i < qBafuda; i++) {
                ba->bafuda[i] = ba->bafuda[i + 1];
                ba->bMonth[i] = ba->bMonth[i + 1];
            }
            ba->bafuda[qBafuda] = 0;
            ba->bMonth[qBafuda] = 0;
            convertToSuit(player[*playerId].motifuda, player[*playerId].mMonth, player[*playerId].qMotifuda, qMotifuda);
        } else {  /* 場札と一致する手札が1枚もない場合 */
            ba->bafuda[qBafuda] = *cardId;
            ba->bMonth[qBafuda] = *suit;
            qBafuda++;
        }
    }
    ba->qBafuda = qBafuda;
    player[*playerId].qTefuda = qTefuda;
    player[*playerId].qMotifuda = qMotifuda;
}

//  選択した手札を場に出す
void playTefuda( field *ba, players *player, playerID *playerId ) {

    int flag = player[*playerId].flag;
    int qTefuda = player[*playerId].qTefuda;

    moveHanafuda(ba, player, playerId, &(player[*playerId].tefuda[flag]), &(player[*playerId].tMonth[flag]));

    qTefuda--;
    for (int i = flag; i < qTefuda; i++) {
        player[*playerId].tefuda[i] = player[*playerId].tefuda[i + 1];
        player[*playerId].tMonth[i] = player[*playerId].tMonth[i + 1];
    }
    player[*playerId].tefuda[qTefuda] = 0;
    player[*playerId].tMonth[qTefuda] = 0;

    player[*playerId].flag = -1;
    player[*playerId].qTefuda = qTefuda;
}

//  山札を場に出す
void drawYamafuda( field *ba, players *player, playerID *playerId, game *hanaawase ) {

    char sign = '"';
    int cardId;
    int suit;

    cardId = generateNumber(hanaawase);
    suit = (cardId - 1) % 12 + 1;
    printf("%c", sign);
    printFudaName(&cardId, &suit, CHARACTER1);
    printf("%c を引きました。\n", sign);

    moveHanafuda(ba, player, playerId, &cardId, &suit);
}

//  役を判断する
void judgeHand( players *player, playerID *playerId, game *hanaawase ) {

    int card[DECK] = {0};
    int cardId;
    int column;

    int qMotifuda = player[*playerId].qMotifuda;

    printPlayerName(*playerId);
    column = NAME - name.length[*playerId];
    if (column) {
        moveCursorRight(column);
    }
    printf(": ");

    player[*playerId].score = 0;
    for (int i = 0; i < qMotifuda; i++) {
        cardId = player[*playerId].motifuda[i];
        card[cardId]++;
        player[*playerId].score += hanafuda.point[cardId];
    }

    if (player[*playerId].score <= 20) {
        printf("フケ ");
        hanaawase->judgement = RETRY;
    } else {
        if (card[1] && card[3] && card[8] && card[11] && card[12]) {
            player[*playerId].handPoint += 200;
            printf("五光 ");
        } else if (card[1] && card[3] && card[8] && card[12]) {
            player[*playerId].handPoint += 60;
            printf("四光 ");
        } else if (card[1] && card[8] && card[12]) {
            player[*playerId].handPoint += 20;
            printf("松桐坊主 ");
        }
        int tan = 0;
        for (int i = 0; i < qMotifuda; i++) {
            cardId = player[*playerId].motifuda[i];
            if (cardId >= 13 && cardId <= 22 && cardId != 20) {
                tan++;
            }
        }
        switch (tan) {
            case 9:
            case 8:
            case 7:
                player[*playerId].handPoint += 40;
                printf("七短 ");
                break;
            case 6:
                player[*playerId].handPoint += 30;
                printf("六短 ");
                break;
            default:
                break;
        }
        if (card[13] && card[14] && card[15]) {
            player[*playerId].handPoint += 40;
            printf("赤短 ");
        }
        if (card[18] && card[21] && card[22]) {
            player[*playerId].handPoint += 40;
            printf("青短 ");
        }
        if (card[16] && card[17] && card[19]) {
            player[*playerId].handPoint += 20;
            printf("くさ ");
        }
        if (card[1] && card[2] && card[3]) {
            player[*playerId].handPoint += 30;
            printf("表菅原 ");
        } 
        if (card[3] && card[8] && card[9]) {
            player[*playerId].handPoint += 40;
            printf("のみ ");
        } else if (card[3] && card[9]) {
            player[*playerId].handPoint += 20;
            printf("花見で一杯 ");
        } else if (card[8] && card[9]) {
            player[*playerId].handPoint += 20;
            printf("月見で一杯 ");
        }
        if (card[6] && card[7] && card[10]) {
            player[*playerId].handPoint += 20;
            printf("猪鹿蝶 ");
        }
        if (card[4] && card[16] && card[28] && card[40]) {
            player[*playerId].handPoint += 20;
            printf("藤シマ ");
        }
        if (card[12] && card[24] && card[36] && card[48]) {
            player[*playerId].handPoint += 20;
            printf("桐シマ ");
        }
        if (card[11] && card[23] && card[35] && card[47]) {
            player[*playerId].handPoint = 0;
            printf("雨シマ役流し ");
        }
    }
}

//  得点を計算する
void calculateScore( players *player ) {

    int column;

    for (int i = 0; i < GUEST; i++) {
        for (int j = 0; j < GUEST; j++) {
            if (i == j) {
                player[i].score += (GUEST - 1) * player[j].handPoint; 
            } else {
                player[i].score -= player[j].handPoint;
            }
        }
    }
    for (int i = 0; i < GUEST; i++) {
        player[i].score -= (264 + GUEST - 1) / GUEST; 
    }

    for (int i = 0; i < GUEST; i++) {
        player[i].totalScore += player[i].score;
    }

    for (int i = 0; i < GUEST; i++) {
        printPlayerName(i);
        column = NAME - name.length[i];
        if (column) {
            moveCursorRight(column);
        }
        printf(": %d", player[i].score);
        printf("点\n");
    }
}

//  順位を決める
void decideRanking( players *player, game *hanaawase ) {

    int flg[GUEST];
    int column;
    int maxTotalScore;
    int playerId;
    int rank;

    for (int i = 0; i < GUEST; i++) {
        flg[i] = 1;
    }
    for (int i = 0; i < GUEST; i++) {
        maxTotalScore = -10000;
        for (int j = 0; j < GUEST; j++) {
            if (flg[j]) {
                if (maxTotalScore < player[j].totalScore) {
                    maxTotalScore = player[j].totalScore;
                    hanaawase->ranking[i] = j;
                }
            }
        }
        flg[hanaawase->ranking[i]] = 0;
    }

    for (int i = 0; i < GUEST; i++) {
        if (!i) {
            rank = i + 1;
            printf("%d位 ", rank);
        } else {
            if (player[hanaawase->ranking[i]].totalScore == player[hanaawase->ranking[i - 1]].totalScore) {
                printf("%d位 ", rank);
            } else {
                rank = i + 1;
                printf("%d位 ", rank);
            }
        }
        playerId = hanaawase->ranking[i];
        printPlayerName(playerId);
        column = NAME - name.length[playerId];
        if (column) {
            moveCursorRight(column);
        }
        printf(" %d点\n", player[playerId].totalScore);
    }
}
