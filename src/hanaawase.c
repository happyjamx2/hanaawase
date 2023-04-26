#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/hanaawase.h"

int main(void) {

    playerID playerId;

    game hanaawase;
    field ba;
    players player[GUEST];

    char string[SIZE];
    int round = 1;

    virtualTerminal();
    srand((unsigned int)time(NULL));
    
    printf("\n花合わせにようこそ。\n");

    for (int i = 0; i < GUEST; i++) {
        player[i].totalScore = 0;
    }

    while (round <= finish) {

        printf("\n試合を開始します。-%d月-\n", round);
        setValue(&ba, player, &hanaawase);

        printf("\n順番を決めます。\n");
        decideTurn(player, &hanaawase);

        printf("\n花札を配ります。\n");
        dealHanahuda(&ba, player, &hanaawase);
 
        playerId = YOU;
        printHanafuda(&ba, player, &playerId);
 
        printf("\n始めます。\n");
        while (player[hanaawase.order[0]].qTefuda) {
            for (int i = 0; i < GUEST; i++) {
                playerId = hanaawase.order[i];
                selectTefuda(&ba, player, &playerId);
                playTefuda(&ba, player, &playerId);
                drawYamafuda(&ba, player, &playerId, &hanaawase);
            }
        }
        printf("\n終わります。\n");

        playerId = YOU;
        printHanafuda(&ba, player, &playerId);

        printf("\n役を判定します。\n");
        for (int i = 0; i < GUEST; i++) {
            playerId = i;
            judgeHand(player, &playerId, &hanaawase);
            printf("\n");
        }

        if (hanaawase.judgement) {
            printf("\n点数に変動はありません。\n");
            decideRanking(player, &hanaawase);
            
            if (round < finish) {
                printf("\n次の対局に進みます。");
                printf("\n何かキーを押してください。\n");
                scanf("%1s%*[^\n]", string); 
            }
            round++;
            continue;
        }

        printf("\n得点を計算します。\n");
        calculateScore(player);

        printf("\n順位は以下の通りになりました。\n");
        decideRanking(player, &hanaawase);

        if (round < finish) {
            printf("\n次の対局に進みます。");
            printf("\n何かキーを押してください。\n");
            scanf("%1s%*[^\n]", string); 
        }
        round++;
    }
    printf("\nありがとうございました。\n");

    return 0;
}