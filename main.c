/*
＊画面のサイズは最低横62×縦32必要ですが、全画面が一番よりお楽しみいただけます！
このゲームはシューティングゲームです。敵の攻撃を避けて、弾を敵に５回当てるとゲームクリアです。弾は初めに５０発所持しています。敵の攻撃を３回受けるとゲームオーバーとなります。（Ｘはプレイヤー、Ａは敵）
＜操作キー＞
・wキー：上に移動
・sキー：下に移動
・aキー：左に移動
・dキー：右に移動
・lキー：その場に停止

・uキー：上に弾を発射
・jキー：下に弾を発射
・hキー：左に弾を発射
・kキー：右に弾を発射

・qキー：ゲーム終了
左手と右手の薬指と中指と人差し指で操作することを推奨します。（lキーは左手の小指で）
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <math.h>
// レベルが高いほどAを捕捉するのが難しくなる
#define REVEL 1   // REVEL 1 (Easy)|REVEL 2 (Normal)|REVEL 3 (Hard)|REVEL 4 (No hope)
#define X_HP 3    // XのHP（Aからの攻撃に耐えられる回数）
#define A_HP 5    // AのHP（Xからの攻撃に耐える回数）
#define BULLET 50 // 弾の数
#define B_SPEED 1 // 弾のスピード

typedef struct POINT
{
    int x; // X座標
    int y; // Y座標
} Point;

// プロトタイプ宣言
int getchar_point(Point a);
int draw_point(Point a, char c);
int check_in_out(Point a);

void shoot_point(Point *a, Point *b, int *d);

int main(void)
{
    int i;
    int max_x, max_y;           // 画面のサイズ
    int wait_interval = 100000; // 待ち時間 0.1秒
    int wait_last = 2;          // 終了時の待ち時間 2秒
    int dir_x = 0, dir_y = 0;   // プレイヤーが移動する向き
    int dis_x = 0, dis_y = 0;   // プレイヤーと敵とのx, y距離
    int status;                 // 画面の内外判定
    int counter = 0;            // 弾の使った数
    int attack_counter = 0;     // 敵に弾を当てた回数
    int damage_counter = 0;     // 敵にダメージをあたえられた回数
    int left[BULLET];           // 弾が移動する向き
    int right[BULLET];
    int up[BULLET];
    int down[BULLET];
    int current_time;        // 敵を倒すのにかかった時間
    int fastest_time = 10e8; // 上記で過去の一番早いタイム

    char key; // 限定された入力キーの保存
    char tmp; // 移動キー保存用

    Point p, q;      // pはプレイヤー、qは敵
    Point r[5];      // rはqのコピー用
    Point b[BULLET]; // bは弾

    FILE *fp;

    time_t t1, t2;

    srand((unsigned)time(NULL)); // 乱数

    /* 画面制御を行う準備 */
    initscr();
    noecho();
    timeout(0);
    curs_set(0);
    cbreak();
    getmaxyx(stdscr, max_y, max_x);

    /* 点の位置（ＸとＡ）をセット */ // （Ｘはプレイヤー、Ａは敵）
    p.x = max_x / 2;
    p.y = max_y / 2;
    do
    {
        q.x = rand() % max_x;
        q.y = rand() % max_y;
        dis_x = fabs(p.x - q.x); // ＸとＡのx座標の差の絶対値
        dis_y = fabs(p.y - q.y); // ＸとＡのy座標の差の絶対値
    } while (dis_x <= 30 || dis_y <= 15); // ＸとＡの位置が近ければ繰り返す

    draw_point(p, 'X'); // 画面中央にXを書く
    draw_point(q, 'A'); // ランダムにAを書く
    refresh();          // 画面を更新

    /* 本体前の準備 */
    for (i = 0; i < BULLET; i++)
    { // 配列の初期化、バグが起こらない値にする
        left[i] = -1;
        right[i] = -1;
        up[i] = -1;
        down[i] = -1;
    }

    t1 = time(NULL); // タイム測定開始

    /* 本体 */
    while (1)
    {
        key = getch(); // キーボードから１文字入力
        if (key != EOF && (key == 'a' || key == 's' || key == 'd' || key == 'w'))
        {              // a,s,d,wいづれかの文字を次の入力時まで保存、他のキーなら保存しない
            tmp = key; // 移動キーをコピーするのはXを画面内に留まらせるため
        }
        draw_point(p, ' '); // 現在の位置のXを消去

        // Xの操作
        if (key == 'd')
        {
            dir_x = 1; // dなら右へ移動
            dir_y = 0;
        }
        if (key == 'a')
        {
            dir_x = -1; // aなら左へ移動
            dir_y = 0;
        }
        if (key == 'w')
        {
            dir_y = -1; // wなら上へ移動
            dir_x = 0;
        }
        if (key == 's')
        {
            dir_y = 1; // sなら下へ移動
            dir_x = 0;
        }
        if (key == 'l')
        {
            dir_y = 0; // lなら停止
            dir_x = 0;
        }
        if (key == 'q')
            break;         // qなら終了
        p.x = p.x + dir_x; // dir_xがずっと足される
        p.y = p.y + dir_y; // dir_yがずっと足される

        // 弾を撃つ操作
        if (key == 'h' && counter < BULLET)
        {                                 // 撃った数が最初に所持している弾の数と同じになると、それ以降は撃てない
            left[counter] = counter;      // 何番目の弾がどの向きに撃たれたのかを保存しておく
            shoot_point(&p, b, &counter); // 初めはcounter=0で、shoot_point関数内でcounter++される
        }
        else if (key == 'k' && counter < BULLET)
        {
            right[counter] = counter;
            shoot_point(&p, b, &counter);
        }
        else if (key == 'u' && counter < BULLET)
        {
            up[counter] = counter;
            shoot_point(&p, b, &counter);
        }
        else if (key == 'j' && counter < BULLET)
        {
            down[counter] = counter;
            shoot_point(&p, b, &counter);
        }

        for (i = 0; i < counter; i++)
        { // 撃った弾を動かし続ける
            if (right[i] == i)
            { // 右
                draw_point(b[i], ' ');
                b[i].x = b[i].x + B_SPEED;
                draw_point(b[i], '.');
            }
            else if (left[i] == i)
            { // 左
                draw_point(b[i], ' ');
                b[i].x = b[i].x - B_SPEED;
                draw_point(b[i], '.');
            }
            else if (down[i] == i)
            { // 下
                draw_point(b[i], ' ');
                b[i].y = b[i].y + B_SPEED;
                draw_point(b[i], '.');
            }
            else if (up[i] == i)
            { // 上
                draw_point(b[i], ' ');
                b[i].y = b[i].y - B_SPEED;
                draw_point(b[i], '.');
            }
        }

        // Ａの動き
        draw_point(q, ' ');      // 現在の位置のＡを消去
        dis_x = fabs(p.x - q.x); // ＸとＡのx座標の差の絶対値
        dis_y = fabs(p.y - q.y); // ＸとＡのy座標の差の絶対値
        if (dis_x <= 30 && dis_y <= 15 && rand() % 2 == 0)
        { // 索敵内ならＸを追いかける
            if (p.x == q.x && p.y < q.y)
            { // Ｘが上にいるとき
                q.y = q.y - REVEL;
            }
            else if (p.x == q.x && p.y > q.y)
            { // Ｘが下にいるとき
                q.y = q.y + REVEL;
            }
            else if (p.x < q.x && p.y == q.y)
            { // Ｘが左にいるとき
                q.x = q.x - REVEL;
            }
            else if (p.x > q.x && p.y == q.y)
            { // Ｘが右にいるとき
                q.x = q.x + REVEL;
            }
            else if (p.x < q.x && p.y < q.y)
            { // Ｘが左上にいるとき
                if (rand() % 2 == 0)
                { // 上か左どちらかに移動すれば近付く
                    q.x = q.x - REVEL;
                }
                else
                {
                    q.y = q.y - REVEL;
                }
            }
            else if (p.x > q.x && p.y < q.y)
            { // Ｘが右上にいるとき
                if (rand() % 2 == 0)
                { // 上か右どちらかに移動すれば近付く
                    q.x = q.x + REVEL;
                }
                else
                {
                    q.y = q.y - REVEL;
                }
            }
            else if (p.x < q.x && p.y > q.y)
            { // Ｘが左下にいるとき
                if (rand() % 2 == 0)
                { // 下か左どちらかに移動すれば近付く
                    q.x = q.x - REVEL;
                }
                else
                {
                    q.y = q.y + REVEL;
                }
            }
            else if (p.x > q.x && p.y > q.y)
            { // Ｘが右下にいるとき
                if (rand() % 2 == 0)
                { // 下か右どちらかに移動すれば近付く
                    q.x = q.x + REVEL;
                }
                else
                {
                    q.y = q.y + REVEL;
                }
            }
        }
        else if (rand() % 2 == 0)
        { // 索敵外ならランダムに動く
            if (rand() % 4 == 0)
            { // ランダムに上下左右に動く
                q.x = q.x + REVEL;
            }
            else if (rand() % 4 == 1)
            {
                q.x = q.x - REVEL;
            }
            else if (rand() % 4 == 2)
            {
                q.y = q.y + REVEL;
            }
            else if (rand() % 4 == 3)
            {
                q.y = q.y - REVEL;
            }
        }

        // 画面外にでそうなAを画面内にとどめる
        if ((status = draw_point(q, 'A')) == 1)
        {

            r[1].x = q.x + REVEL;
            r[1].y = q.y;
            if (status = draw_point(r[1], 'A') != 1)
            {
                draw_point(r[1], ' ');
                q.x = r[1].x;
                q.y = r[1].y;
            }

            r[2].x = q.x - REVEL;
            r[2].y = q.y;
            if (status = draw_point(r[2], 'A') != 1)
            {
                draw_point(r[2], ' ');
                q.x = r[2].x;
                q.y = r[2].y;
            }

            r[3].x = q.x;
            r[3].y = q.y + REVEL;
            if (status = draw_point(r[3], 'A') != 1)
            {
                draw_point(r[3], ' ');
                q.x = r[3].x;
                q.y = r[3].y;
            }

            r[4].x = q.x;
            r[4].y = q.y - REVEL;
            if (status = draw_point(r[4], 'A') != 1)
            {
                draw_point(r[4], ' ');
                q.x = r[4].x;
                q.y = r[4].y;
            }

            draw_point(q, 'A');
        }

        // 弾がAに当たったときの処理
        for (i = 0; i < counter; i++)
        {
            if (status = check_in_out(b[i]) == 1)
            { // 弾が画面内であるか判定、そうでなければ最初に戻る
                continue;
            }
            else if (getchar_point(b[i]) == 'A')
            {                     // 弾の移動先にAがあるか判定
                ++attack_counter; // 敵へのアタック回数を増やす
                if (attack_counter < A_HP)
                {
                    flash();
                }
                else
                {
                    dir_x = 0;
                    dir_y = 0;
                    flash();
                    beep();          // 音を鳴らす
                    t2 = time(NULL); // タイム測定終了
                    clear();
                    move(max_y / 2, (max_x / 2) - 5);
                    printw("Game Clear!");
                    refresh();
                    sleep(wait_last);       // 少し待つ
                    endwin();               /* 画面制御の終了 */
                    current_time = t2 - t1; // タイム計算
                    // 最速タイムの読み取り
                    if ((fp = fopen("time.dat", "r")) != NULL) // ファイルが存在する場合
                    {
                        fscanf(fp, "%d", &fastest_time); // 最速タイムを読み込む
                        fclose(fp);
                    }
                    else
                    {
                        // ファイルが存在しない場合は最速タイムの記録ファイルを新しく作成し、初期値を設定
                        fp = fopen("time.dat", "w");
                        if (fp == NULL)
                        {
                            // ファイルが開けない場合のエラーメッセージ
                            printf("Error: could not create file 'time.dat'\n");
                            return 1;
                        }
                        fastest_time = current_time;
                        fprintf(fp, "%d\n", fastest_time);
                        fclose(fp);
                        printf("NEW RECORD!: %d [sec]\n", fastest_time); // 新記録を表示
                        return 0;
                    }

                    // タイムの更新
                    if (current_time < fastest_time)
                    { // 過去のタイムよりも速ければ更新
                        fastest_time = current_time;
                        fp = fopen("time.dat", "w");                     // ファイルを開く
                        fprintf(fp, "%d\n", fastest_time);               // ファイル書き込み
                        fclose(fp);                                      // ファイルを閉じる
                        printf("NEW RECORD!: %d [sec]\n", fastest_time); // 新記録を表示
                        return 0;
                    }
                    else
                    {
                        printf("record update failure\n");
                        printf("fastest_time: %d [sec]\n", fastest_time); // 過去の最速タイム
                        printf("current_time: %d [sec]\n", current_time); // 直前のタイム
                        return 0;
                    }
                }
            }
        }

        // Xが画面外にでそうなとき
        status = draw_point(p, 'X'); // 新しい場所にXを書く
        if (status == 1)
        { // 画面外に出そうな時はその場に留まる

            if (tmp == 'd')
            {
                p.x--;
                draw_point(p, 'X');
            }
            else if (tmp == 'a')
            {
                p.x++;
                draw_point(p, 'X');
            }
            else if (tmp == 'w')
            {
                p.y++;
                draw_point(p, 'X');
            }
            else if (tmp == 's')
            {
                p.y--;
                draw_point(p, 'X');
            }
        }

        // 敵に攻撃を与えられたときの処理
        if (getchar_point(q) == 'X')
        { // Aの移動先にXがあると停止
            ++damage_counter;
            if (damage_counter < X_HP)
            {
                flash();
            }
            else
            {
                dir_x = 0;
                dir_y = 0;
                flash();
                beep(); // 音を鳴らす
                clear();
                move(max_y / 2, (max_x / 2) - 4);
                printw("Game over!");
                refresh();
                sleep(wait_last); // 少し待つ
                endwin();         /* 画面制御の終了 */
                return 0;
            }
        }

        refresh();             // 画面を更新
        usleep(wait_interval); // 少し待つ
    }

    /* 画面制御の終了 */
    endwin();

    printf("You pushed 'q' key\n");
    printf("End\n");

    return 0;
}

/* 指定された点に書かれた文字を読み取る関数 */
int getchar_point(Point a)
{
    int c;
    move(a.y, a.x);
    c = inch();
    return c;
}

/* 画面内であれば文字を書く関数 */
int draw_point(Point a, char c)
{
    int status;
    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);
    if ((0 <= a.y) && (a.y < max_y) && (0 <= a.x) && (a.x < max_x))
    {
        status = 0;
        move(a.y, a.x); // 移動（縦、横の順）
        addch(c);       // １文字書く
    }
    else
    {
        status = 1;
    }
    return status;
}

/* 銃弾を撃つ関数 */
void shoot_point(Point *a, Point *b, int *d)
{
    if (*d < BULLET)
    { // 新しい銃弾を撃つ
        b[*d].x = a->x;
        b[*d].y = a->y;
        ++*d; // 新しい銃弾を撃つたびに銃弾の数を数える
    }
}

/* 銃弾が画面内であるかどうかを判定する関数 */
int check_in_out(Point a)
{
    int status;
    int max_y, max_x;

    getmaxyx(stdscr, max_y, max_x);
    if ((0 <= a.y) && (a.y < max_y) && (0 <= a.x) && (a.x < max_x))
    {
        status = 0;
    }
    else
    {
        status = 1;
    }
    return status;
}
