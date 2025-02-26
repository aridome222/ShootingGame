# ShootingGame
敵の攻撃を避けながら弾を撃って敵を倒すゲームです。

# 1. プログラムの実行方法
このゲームのプログラム構成は次のようになっている。
* main.c （ShootingGameのプログラムファイル）
* time.dat (最速キルタイムを記録するファイル)

ゲームを実行するためにプログラムファイルをコンパイルし、実行ファイルを作成する必要がある。

コンパイル方法は、プログラムファイルを置いた場所で

$ gcc main.c -o main -lm -lcurses

を端末上で実行すると、実行ファイル main が作られる。実行ファイルの実行方法は、

$ ./main

で実行する。なお、time.datは初回実行時に敵を倒すことができた場合に生成される。

# 2. ゲームの遊び方
## 2.1 ゲームのルール
シューティングゲームは、プレイヤーである X を、索敵内に入ると追いかけてくる敵である A の攻撃を受けないように X を上下左右に動かし、さらに上下左右に撃てる弾を A に 5 回当てることで敵を倒すというゲームである (図 1)。
正確に敵に弾を当てるためには、敵の近くで止まって撃つと当たりやすい。遠くから撃って当たることもあるが、弾は 50 発と数が限られており、弾がなくなることは実質敵の攻撃を 3 回受けてゲームオーバーになることと同じである。
敵を倒すことでゲームクリアとなる。ゲームクリア後に、敵を倒すまでにかかったタイムが画面に表示される。もし、そのタイムが一番速いタイムであればtime.datに保存され、新記録達成となる (図 2)。
そうでなければ、そのタイムと過去の一番速いタイムが表示される (図 3)。いかに早く敵を倒せるかがこのゲームの主な目的である。

![a1](https://github.com/aridome222/ShootingGames/assets/128010851/6f8a248f-9407-4891-861e-c7b9ca947399)

![a2](https://github.com/aridome222/ShootingGames/assets/128010851/6bb3e269-700e-4c94-af51-424560ce81c9)

![a3](https://github.com/aridome222/ShootingGames/assets/128010851/c7ec2a38-0fd5-4083-ba85-54f5d701a8b6)

## 2.2 キーボード操作
* w キー：上に移動
一回押せば上に移動し続ける。表示画面より上には進まない。
* s キー：下に移動
一回押せば下に移動し続ける。表示画面より下には進まない。
* a キー：左に移動
一回押せば左に移動し続ける。表示画面より左には進まない。
* d キー：右に移動
一回押せば右に移動し続ける。表示画面より右には進まない。
* l キー：その場に停止
* u キー：上に弾を発射
u を押したままにすると上連射できるが、しばらくの間移動が利かなくなる。
* j キー：下に弾を発射
j を押したままにすると下連射できるが、しばらくの間移動が利かなくなる。
* h キー：左に弾を発射
h を押したままにすると左連射できるが、しばらくの間移動が利かなくなる。
* k キー：右に弾を発射
k を押したままにすると右連射できるが、しばらくの間移動が利かなくなる 。
* q キー：ゲーム終了
